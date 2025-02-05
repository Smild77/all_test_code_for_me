import serial
import time

SERIAL_PORT = "COM8"
BAUD_RATE = 9600
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)

    ser.write(b"m1\n")
    print("ส่งคำสั่ง: m1 (Motor 1 started)")
    time.sleep(3)
    ser.write(b"m2\n")
    print("ส่งคำสั่ง: m2 (Motor 2 started)")
    time.sleep(3)
    ser.write(b"m1s\nm2s\n")
    print("ส่งคำสั่ง: stop all (All motors stopped)")
    ser.close()

except serial.SerialException as e:
    print(f"เกิดข้อผิดพลาด: {e}")
