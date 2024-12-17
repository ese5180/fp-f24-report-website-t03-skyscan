#include <SPI.h>
#include <RH_RF95.h>
#include <Servo.h>

// LoRa Definitions
#define LISTENING_INTERVAL 3000
#define PACKET_INTERVAL 3000
float frequency = 921.2;

char buf [50];

// LoRa Driver Instance
RH_RF95 rf95(12, 6);

void setup() {
    // Initialize Serial
    SerialUSB.begin(9600);

    Serial1.begin(115200);

    delay(500);

    // Initialize LoRa
    if (!rf95.init()) {
        SerialUSB.println("Radio Init Failed");
        while (1);
    }
    rf95.setFrequency(frequency); 
    SerialUSB.println("LoRa initialized");

}

int receiveLoRaMessage();

void loop() {
  
  if (Serial1.available() > 1) {
    Serial1.readBytes(buf, 2); // Read string from TX (sender)
    rf95.send((uint8_t*)buf, 2);
    rf95.waitPacketSent();
    SerialUSB.println("Message sent");
  }
  receiveLoRaMessage();
  


  
}


// Function to receive a message via LoRa
int receiveLoRaMessage() {
    if (rf95.waitAvailableTimeout(LISTENING_INTERVAL)) {
        uint8_t rec_buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(rec_buf, &len)) {
            SerialUSB.println("Received");
            Serial1.write(rec_buf, len);
            return 1;
        } else {
            SerialUSB.println("Receive failed");
            return 0;
        }
    } else {
        SerialUSB.println("No reply from LoRa server");
        return 0;
    }
}
