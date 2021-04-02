from serial import Serial, EIGHTBITS, PARITY_NONE, STOPBITS_ONE
import sys

if len(sys.argv) != 3:
    print("Usage: Logger.py <Device> <Baud rate>")
    exit(1)

ser = Serial(sys.argv[1], sys.argv[2], EIGHTBITS, PARITY_NONE, STOPBITS_ONE, timeout=None)
ser.flushInput()

while True:
    try:
        line = ser.readline()
        print(line.decode("utf-8"), end = '')
    except:
        print("Connection closed")
        break
