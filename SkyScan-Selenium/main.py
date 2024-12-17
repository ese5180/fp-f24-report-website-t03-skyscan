from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
import time

f = open("data.txt")
location = f.readline()
f.close()

wf = open("data.txt", "a")

service = Service(executable_path = "chromedriver.exe")
driver = webdriver.Chrome(service = service)

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
print("Altitude: " + altitude_element.text)
wf.write("\n" + altitude_element.text[:-1] + "\n")

azimuth_element = driver.find_element(By.ID, "azimuth")
print("Azimuth: " + azimuth_element.text)
wf.write(azimuth_element.text[:-1] + "\n")

# input_element = driver.find_element(By.CLASS_NAME, "ui-menu-item-wrapper")
# input_element.click()
# input_element.click()

wf.close()
driver.quit()