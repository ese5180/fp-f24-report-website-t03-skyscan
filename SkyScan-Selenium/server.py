from fastapi import FastAPI
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time
import serial

app = FastAPI()

# Function that spins up the Chrome driver, fetches, and returns altitude and azimuth
def get_sun_data(location):
    chrome_options = Options()
    chrome_options.add_argument("--headless")
    chrome_options.add_argument("--no-sandbox")
    chrome_options.add_argument("--disable-dev-shm-usage")
    # Update the path to the macOS-compatible ChromeDriver
    service = Service(executable_path="chromedriver.exe")  # Replace with the correct path
    driver = webdriver.Chrome(service=service, options=chrome_options)
    
    try:
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
        
        # Open serial connection to a macOS serial port
        serial_port = 'COM10'  # Replace with your specific port
        try:
            ser = serial.Serial(serial_port, 115200, timeout=1)
        except serial.SerialException as e:
            print(f"Error opening serial port: {e}")
            return {"error": str(e)}
        
        # Prepare and send data over serial
        data_to_send = f"Altitude: {altitude}, Azimuth: {azimuth}\n"
        ser.write(data_to_send.encode())
        
        # Close the serial connection
        ser.close()
        return {
            "altitude": altitude,
            "azimuth": azimuth
        }
    finally:
        driver.quit()

@app.get("/sun_data")
async def sun_data(location: str):
    return get_sun_data(location)

# Sample endpoint: http://0.0.0.0:8000/sun_data?location=Philadelphia%2C%20PA
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
