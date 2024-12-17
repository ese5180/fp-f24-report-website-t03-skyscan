## Server code functioning without an external accesible API 

# Reads from nordic, and writes to nordic every 60 seconds with solar data1
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
import time
import serial
import sys

# Function to fetch solar data and send it to the serial port
def get_sun_data(location, ser):
    chrome_options = Options()
    chrome_options.add_argument("--headless")
    chrome_options.add_argument("--no-sandbox")
    chrome_options.add_argument("--disable-dev-shm-usage")
    service = Service(executable_path="chromedriver.exe") 
    driver = webdriver.Chrome(service=service, options=chrome_options)
    
    try:
        # Open the website and perform operations
        driver.get("https://suncalc.org")
        input_element = driver.find_element(By.ID, "c-p-bn")
        input_element.click()
        input_element = driver.find_element(By.ID, "location")
        input_element.clear()
        input_element.send_keys(location)
        time.sleep(1)
        click_element = driver.find_element(By.XPATH, "/html/body/ul/li[1]")
        click_element.click()
        time.sleep(2)
        click_element = driver.find_element(By.XPATH, "/html/body/div[7]/div[2]/span/a[1]")
        click_element.click()
        time.sleep(3)
        altitude_element = driver.find_element(By.ID, "sunhoehe")
        azimuth_element = driver.find_element(By.ID, "azimuth")
        altitude = altitude_element.text[:-1]
        azimuth = azimuth_element.text[:-1]

        # Prepare and send data over serial
        data_to_send = f"{altitude}, {azimuth}\n"
        ser.write(data_to_send.encode('utf-8'))
        print(f"Data sent: {data_to_send}")

    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        driver.quit()


def main():
    location = "Philadelphia, PA"  # Replace with your desired location
    serial_port = "COM10"  # Replace with your specific port
    baud_rate = 115200

    try:
        # Open the serial port
        with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
            print(f"Serial port {serial_port} opened successfully at {baud_rate} baud.")

            # Main loop
            last_fetch_time = 0
            while True:
                # Fetch solar data every 60 seconds
                current_time = time.time()
                if current_time - last_fetch_time >= 60:
                    print(f"Fetching solar...")
                    get_sun_data(location, ser)
                    last_fetch_time = current_time

                # Read data from the serial port
                response = ser.readline().decode('utf-8').strip()
                if response:
                    print(f"Received: {response}")

    except serial.SerialException as e:
        print(f"Error opening serial port {serial_port}: {e}")
    except KeyboardInterrupt:
        print("Exiting...")
        sys.exit(1)


if __name__ == "__main__":
    main()
