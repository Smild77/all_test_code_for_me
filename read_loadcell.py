import minimalmodbus


def read_registers(port, slave_address, register_address, num_registers):
    try:
        # สร้างออบเจกต์ Modbus
        instrument = minimalmodbus.Instrument(
            port, slave_address)  # กำหนดพอร์ตและ Address
        instrument.serial.baudrate = 9600  # Baudrate
        instrument.serial.parity = minimalmodbus.serial.PARITY_NONE  # Parity
        instrument.serial.stopbits = 1  # Stop Bits
        instrument.serial.bytesize = 8  # Data Bits
        instrument.serial.timeout = 1  # Timeout

        # อ่านค่า Holding Registers
        values = instrument.read_registers(
            register_address, num_registers, functioncode=3)
        print(f"Data from {port} (Address {slave_address}): {values}")
    except Exception as e:
        print(f"Error reading from {port} (Address {slave_address}): {e}")


# อ่านค่าจาก COM9 และ COM10
read_registers("COM9", slave_address=5, register_address=0,
               num_registers=2)  # Address 5
read_registers("COM10", slave_address=7, register_address=0,
               num_registers=2)  # Address 7
