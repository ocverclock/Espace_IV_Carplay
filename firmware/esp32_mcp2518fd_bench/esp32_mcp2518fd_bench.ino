// ESP32 + MCP2518FD bench monitor for Renault CSW-2000R
// DRAFT: verify module oscillator and power pin before first use.

#ifndef ARDUINO_ARCH_ESP32
  #error "Select an ESP32 board"
#endif

#include <ACAN2517.h>
#include <SPI.h>

// ESP32 VSPI proposal
static const uint8_t MCP_SCK  = 18;
static const uint8_t MCP_MISO = 19;
static const uint8_t MCP_MOSI = 23;
static const uint8_t MCP_CS   = 5;
static const uint8_t MCP_INT  = 27;

// TODO: VERIFY ON THE RECEIVED MODULE.
// Change to OSC_20MHz if the physical oscillator is 20 MHz.
static const ACAN2517Settings::Oscillator MCP_OSC = ACAN2517Settings::OSC_40MHz;

static const uint32_t CAN_BITRATE = 500000;

ACAN2517 canBus(MCP_CS, SPI, MCP_INT);

static void printFrame(const CANMessage &frame) {
  const uint32_t nowUs = micros();

  Serial.printf("%010lu ID=%03lX DLC=%u DATA=",
                (unsigned long) nowUs,
                (unsigned long) frame.id,
                frame.len);

  for (uint8_t i = 0; i < frame.len; i++) {
    if (frame.data[i] < 0x10) Serial.print('0');
    Serial.print(frame.data[i], HEX);
    if (i + 1 < frame.len) Serial.print(' ');
  }

  if (frame.ext) Serial.print(" EXT");
  if (frame.rtr) Serial.print(" RTR");
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("CSW-2000R CAN bench monitor");
  Serial.println("Target: classic CAN 500 kbit/s / Normal20B");

  SPI.begin(MCP_SCK, MCP_MISO, MCP_MOSI);

  ACAN2517Settings settings(MCP_OSC, CAN_BITRATE);

  // Normal CAN mode is essential: unlike ListenOnly, it can ACK valid frames.
  settings.mRequestedMode = ACAN2517Settings::Normal20B;

  const uint32_t errorCode = canBus.begin(settings, [] { canBus.isr(); });

  if (errorCode != 0) {
    Serial.print("CAN init ERROR 0x");
    Serial.println(errorCode, HEX);
    Serial.println("Check SPI wiring, oscillator selection, power and INT pin.");
    while (true) delay(1000);
  }

  Serial.print("CAN init OK, actual bitrate: ");
  Serial.println(settings.actualBitRate());
  Serial.print("Exact bitrate: ");
  Serial.println(settings.exactBitRate() ? "yes" : "no");
  Serial.print("Sample point: ");
  Serial.print(settings.samplePointFromBitStart());
  Serial.println("%");
  Serial.println("Waiting for frames...");
}

void loop() {
  CANMessage frame;

  while (canBus.available()) {
    canBus.receive(frame);
    printFrame(frame);
  }
}
