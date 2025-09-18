import board
import neopixel

pixel = neopixel.NeoPixel(board.NEOPIXEL, 1)
pixel.fill((0, 0, 255))
