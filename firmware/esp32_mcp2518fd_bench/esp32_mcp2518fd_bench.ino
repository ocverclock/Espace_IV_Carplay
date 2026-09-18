// ESP32 + MCP2518FD/ATA6563 decoder for Renault CSW-2000R
// Bench-tested facts from this project:
//   - MCP2518FD oscillator: 40 MHz
//   - CSW bus: classic CAN 2.0B, 500 kbit/s
//   - CSW idle frame: 0x681  F0 0A 0A 01 FF FF FF FF
//
// IMPORTANT:
//   BENCH_MODE = true  -> CSW alone on bench. Normal20B is REQUIRED so the
//                         MCP2518FD ACKs valid CSW frames.
//   BENCH_MODE = false -> passive monitoring on an already active vehicle bus.
//                         ListenOnly does not ACK and cannot disturb arbitration.

#ifndef ARDUINO_ARCH_ESP32
  #error "Select an ESP32 board"
#endif

#include <ACAN2517.h>
#include <SPI.h>

// -----------------------------------------------------------------------------
// ESP32 VSPI wiring used by the project
// -----------------------------------------------------------------------------
static const uint8_t MCP_SCK  = 18;
static const uint8_t MCP_MISO = 19;
static const uint8_t MCP_MOSI = 23;
static const uint8_t MCP_CS   = 5;
static const uint8_t MCP_INT  = 27;

// -----------------------------------------------------------------------------
// Confirmed CAN configuration
// -----------------------------------------------------------------------------
static const ACAN2517Settings::Oscillator MCP_OSC =
  ACAN2517Settings::OSC_40MHz;

static const uint32_t CAN_BITRATE = 500000;

// true for the current CSW bench: ESP32/MCP2518FD is the only other CAN node.
static const bool BENCH_MODE = true;

// Known CSW idle/baseline frame measured with PulseView.
static const uint32_t CSW_ID = 0x681;
static const uint8_t CSW_DLC = 8;
static const uint8_t CSW_IDLE[CSW_DLC] = {
  0xF0, 0x0A, 0x0A, 0x01, 0xFF, 0xFF, 0xFF, 0xFF
};

ACAN2517 canBus(MCP_CS, SPI, MCP_INT);

// -----------------------------------------------------------------------------
// Runtime state
// -----------------------------------------------------------------------------
static uint32_t totalFrames = 0;
static uint32_t idleFrames = 0;
static uint32_t eventFrames = 0;
static uint32_t otherIdFrames = 0;

static uint32_t lastFrameMs = 0;
static uint32_t lastStatsMs = 0;

static bool lastStateWasIdle = true;
static bool haveLastEvent = false;
static CANMessage lastEvent;

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
static bool samePayload(const CANMessage &frame,
                        const uint8_t *data,
                        const uint8_t len) {
  if (frame.len != len) return false;
  for (uint8_t i = 0; i < len; i++) {
    if (frame.data[i] != data[i]) return false;
  }
  return true;
}

static bool sameFramePayload(const CANMessage &a, const CANMessage &b) {
  if (a.id != b.id || a.len != b.len || a.ext != b.ext || a.rtr != b.rtr) {
    return false;
  }

  for (uint8_t i = 0; i < a.len; i++) {
    if (a.data[i] != b.data[i]) return false;
  }
  return true;
}

static void printHexByte(const uint8_t value) {
  if (value < 0x10) Serial.print('0');
  Serial.print(value, HEX);
}

static void printPayload(const CANMessage &frame) {
  for (uint8_t i = 0; i < frame.len; i++) {
    printHexByte(frame.data[i]);
    if (i + 1 < frame.len) Serial.print(' ');
  }
}

static void printId(const CANMessage &frame) {
  if (frame.ext) {
    Serial.printf("%08lX", (unsigned long)frame.id);
  } else {
    Serial.printf("%03lX", (unsigned long)frame.id);
  }
}

static void printDiffAgainstIdle(const CANMessage &frame) {
  if (frame.id != CSW_ID || frame.ext || frame.rtr || frame.len != CSW_DLC) {
    return;
  }

  bool first = true;

  for (uint8_t i = 0; i < CSW_DLC; i++) {
    if (frame.data[i] != CSW_IDLE[i]) {
      if (!first) Serial.print(", ");
      Serial.print('B');
      Serial.print(i);
      Serial.print(':');
      printHexByte(CSW_IDLE[i]);
      Serial.print('>');
      printHexByte(frame.data[i]);
      first = false;
    }
  }

  if (first) {
    Serial.print("none");
  }
}

static void printEvent(const CANMessage &frame) {
  Serial.printf("EVENT t=%010lu ID=",
                (unsigned long)micros());
  printId(frame);
  Serial.print(" DLC=");
  Serial.print(frame.len);
  Serial.print(" DATA=");
  printPayload(frame);

  if (frame.id == CSW_ID && !frame.ext && !frame.rtr && frame.len == CSW_DLC) {
    Serial.print(" DIFF=[");
    printDiffAgainstIdle(frame);
    Serial.print(']');
  }

  if (frame.ext) Serial.print(" EXT");
  if (frame.rtr) Serial.print(" RTR");

  Serial.println();
}

static void handleFrame(const CANMessage &frame) {
  totalFrames++;
  lastFrameMs = millis();

  const bool isCswIdle =
    !frame.ext &&
    !frame.rtr &&
    frame.id == CSW_ID &&
    samePayload(frame, CSW_IDLE, CSW_DLC);

  if (isCswIdle) {
    idleFrames++;

    // Print only the transition back to idle; do not flood the terminal with
    // hundreds of identical baseline frames.
    if (!lastStateWasIdle) {
      Serial.printf("IDLE  t=%010lu ID=681 DATA=F0 0A 0A 01 FF FF FF FF\n",
                    (unsigned long)micros());
    }

    lastStateWasIdle = true;
    haveLastEvent = false;
    return;
  }

  eventFrames++;
  if (frame.id != CSW_ID) otherIdFrames++;

  // Suppress identical retransmissions of the same event payload. A new
  // payload is still printed immediately, which is important for the wheel.
  if (haveLastEvent && sameFramePayload(frame, lastEvent)) {
    lastStateWasIdle = false;
    return;
  }

  printEvent(frame);
  lastEvent = frame;
  haveLastEvent = true;
  lastStateWasIdle = false;
}

static void printStats() {
  const uint32_t now = millis();

  if (now - lastStatsMs < 1000) return;
  lastStatsMs = now;

  Serial.printf(
    "STAT  frames=%lu idle=%lu events=%lu other_id=%lu rx_peak=%lu rx_overflow=%u mode=%u",
    (unsigned long)totalFrames,
    (unsigned long)idleFrames,
    (unsigned long)eventFrames,
    (unsigned long)otherIdFrames,
    (unsigned long)canBus.driverReceiveBufferPeakCount(),
    canBus.hardwareReceiveBufferOverflowCount(),
    (unsigned int)canBus.currentOperationMode()
  );

  if (totalFrames == 0 || now - lastFrameMs > 2000) {
    Serial.print("  NO_CAN_TRAFFIC");
  }

  Serial.println();
}

// -----------------------------------------------------------------------------
// Setup
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("Renault CSW-2000R CAN decoder");
  Serial.println("MCP2518FD + ATA6563 / ESP32");
  Serial.println("CAN: 500 kbit/s classic CAN 2.0B");
  Serial.println("Oscillator: 40 MHz");

  SPI.begin(MCP_SCK, MCP_MISO, MCP_MOSI, MCP_CS);

  ACAN2517Settings settings(MCP_OSC, CAN_BITRATE);

  // Large software RX FIFO: useful during initial reverse engineering.
  settings.mDriverReceiveFIFOSize = 128;
  settings.mControllerReceiveFIFOSize = 32;

  // This sketch never sends application frames. Keep TX allocation minimal.
  settings.mDriverTransmitFIFOSize = 0;
  settings.mControllerTransmitFIFOSize = 1;

  if (BENCH_MODE) {
    // The CSW is alone with our interface: it needs another active node to ACK.
    settings.mRequestedMode = ACAN2517Settings::Normal20B;
    Serial.println("Mode: BENCH / Normal20B (ACK enabled, no application TX)");
  } else {
    // Use this only when connected to an already populated vehicle CAN bus.
    settings.mRequestedMode = ACAN2517Settings::ListenOnly;
    Serial.println("Mode: VEHICLE / ListenOnly (fully passive)");
  }

  const uint32_t errorCode = canBus.begin(settings, [] { canBus.isr(); });

  if (errorCode != 0) {
    Serial.print("CAN INIT ERROR 0x");
    Serial.println(errorCode, HEX);
    Serial.println("Check: SPI wiring, 40 MHz oscillator, power, CS and INT.");
    while (true) delay(1000);
  }

  Serial.print("CAN init OK - actual bitrate: ");
  Serial.println(settings.actualBitRate());

  Serial.print("Exact bitrate: ");
  Serial.println(settings.exactBitRate() ? "yes" : "no");

  Serial.print("Sample point: ");
  Serial.print(settings.samplePointFromBitStart());
  Serial.println("%");

  Serial.println();
  Serial.println("Known idle: ID=681 DLC=8 DATA=F0 0A 0A 01 FF FF FF FF");
  Serial.println("Press ONE CSW control at a time.");
  Serial.println("Only changes are printed; repeated idle/event frames are suppressed.");
  Serial.println();
}

// -----------------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------------
void loop() {
  CANMessage frame;

  while (canBus.receive(frame)) {
    handleFrame(frame);
  }

  printStats();
}
