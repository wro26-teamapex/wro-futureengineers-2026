import pixy
from ctypes import *
import serial
import time

# UART Connection to ESP32 (adjust '/dev/ttyS0' based on your raspi-config)
ser = serial.Serial('/dev/ttyS0', 115200, timeout=1)

# Initialize Pixy2 and set program
pixy.init()
pixy.change_prog("color_connected_components")

class Blocks (Structure):
  _fields_ = [ ("m_signature", c_uint), ("m_x", c_uint), ("m_y", c_uint), ("m_width", c_uint), ("m_height", c_uint) ]

blocks = BlockArray(100)

print("KINETIC Vision Layer Active. Broadcasting...")

while True:
    count = pixy.ccc_get_blocks(100, blocks)
    
    if count > 0:
        # Array is automatically sorted by size. blocks[0] is the largest pillar.
        target_x = blocks[0].m_x
        target_y = blocks[0].m_y
        
        # Format payload: <X:150,Y:80>
        payload = f"<X:{target_x},Y:{target_y}>\n"
        ser.write(payload.encode('utf-8'))
    
    # Throttle to roughly 60Hz to match Pixy2 framerate
    time.sleep(0.016)
