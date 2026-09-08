from machine import Pin
import neopixel
import time

# RP2040-Zero / Waveshare-compatible boards: onboard WS2812 on GPIO16
PIXEL_PIN = 16
np = neopixel.NeoPixel(Pin(PIXEL_PIN), 1)

colors = [
    (16, 0, 0),
    (0, 16, 0),
    (0, 0, 16),
    (0, 0, 0),
]

while True:
    for color in colors:
        np[0] = color
        np.write()
        time.sleep(0.5)
