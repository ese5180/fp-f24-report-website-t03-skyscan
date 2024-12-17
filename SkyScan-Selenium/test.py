import serial
import sys
import serial.tools.list_ports

def main():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(f"Found port: {port.device}")
    # Specify the serial port and baud rate
    serial_port = "COM10"  # Adjust based on your setup
    baud_rate = 115200  # Adjust based on your MCU settings

    try:
        # Open the serial port
        with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
            print(f"Serial port {serial_port} opened successfully at {baud_rate} baud.")

            while True:
                try:
                    # Read user input from the console
                    user_input = input("Enter data to send (or 'exit' to quit): ")

                    # Exit the loop if the user types 'exit'
                    if user_input.lower() == 'exit':
                        print("Exiting...")
                        break

                    # Send data to the MCU
                    ser.write(user_input.encode('utf-8'))
                    print(f"Sent: {user_input}")

                    # Wait for and print the response from the MCU
                    response = ser.readline().decode('utf-8').strip()
                    if response:
                        print(f"Received: {response}")

                except KeyboardInterrupt:
                    print("\nInterrupted by user. Exiting...")
                    break

    except serial.SerialException as e:
        print(f"Error opening serial port {serial_port}: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()