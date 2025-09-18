import time
import board
from adafruit_matrixportal.matrixportal import MatrixPortal

matrixportal = MatrixPortal(status_neopixel=board.NEOPIXEL, debug=True)

# Create a new label with the color and text
matrixportal.add_text(
    text_position=(8, 32),
)

matrixportal.set_text("Hello World")

while True:
    time.sleep(1)