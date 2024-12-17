#include <SPI.h>
#include <RH_RF95.h>
#include <Servo.h>

// LoRa Definitions
#define LISTENING_INTERVAL 3000
#define PACKET_INTERVAL 3000
uint8_t device_ID = 1;
float frequency = 921.2;

// Servo Definitions
Servo myServoAz; // Servo object
Servo myServoAlt; //other servo
int servoPinAz = 5; // Pin connected to the servo0
int servoPinAlt = 2; // other servo pin
int currentPositionAz = 0; // Start with the servo at the neutral position
int currentPositionAlt = 0;
uint8_t toSend[5];


// LoRa Driver Instance
RH_RF95 rf95(12, 6);

void setup() {
    // Initialize Serial
    SerialUSB.begin(9600);

    //while (!SerialUSB); // Wait for serial port to initialize
    pinMode(4, OUTPUT);
    pinMode(9, OUTPUT);
  
    // Set pins D4 and D9 to HIGH
    digitalWrite(4, LOW); // green, measure_mode (not inverted)
    digitalWrite(9, LOW); // orange, charge_mode (interveted)
    delay(500);

    // Initialize LoRa
    if (!rf95.init()) {
        SerialUSB.println("Radio Init Failed - Freezing");
        while (1);
    }
    rf95.setFrequency(frequency); 
    SerialUSB.println("LoRa initialized");

    // Initialize Servos
    myServoAz.attach(servoPinAz);
    myServoAlt.attach(servoPinAlt);
}

void setServoPosition(uint8_t azimuth, uint8_t altitude);
int receiveLoRaMessage();

void loop() {
  /*
    while(!receiveLoRaMessage()) {

    }
    delay(5000);

    digitalWrite(4, HIGH); // measure_mode (not inverted)
    digitalWrite(9, HIGH);

    uint16_t movingVoltage = (analogRead(A0) << 2);
    uint16_t stationaryVoltage = (analogRead(A1) << 2);
    
    movingVoltage |= (device_ID << 12);
    stationaryVoltage |= (device_ID << 12);

    movingVoltage |= 0b00000001;
    stationaryVoltage |= 0b00000010;

    char movingMsg[2];
    movingMsg[0] = (movingVoltage >> 8) & 0xFF;
    movingMsg[1] = movingVoltage & 0xFF;

    char stationaryMsg[2];
    stationaryMsg[0] = (stationaryVoltage >> 8) & 0xFF;
    stationaryMsg[1] = stationaryVoltage & 0xFF;

    sendLoRaMessage(movingMsg);
    sendLoRaMessage(stationaryMsg);
    */
    // hardware demo code

    uint16_t movingVoltage = (analogRead(A0));
    uint16_t stationaryVoltage = (analogRead(A1));

    SerialUSB.print("Moving Panel Voltage: ");
    SerialUSB.println(movingVoltage);
    SerialUSB.print("Stationary Panel Voltage: ");
    SerialUSB.println(stationaryVoltage);
    SerialUSB.println();
    delay(1000);


    // LoRa Communication
    // sendLoRaMessage("Hello from client");
    // receiveLoRaMessage();
    //myServoAz.writeMicroseconds(400);  
}

// void sendSolarData(uint32_t* data) {
//   memcpy(toSend, data, 5);
//   rf95.send(&toSend, 5);

// }

// Function to send a message via LoRa
void sendLoRaMessage(const char* message) {
    SerialUSB.print("Sending: ");
    rf95.send((uint8_t*)message, 2);
    rf95.waitPacketSent();
    SerialUSB.println("Message sent");
}

void setServoPosition(uint8_t azimuth, uint8_t altitude) {
  //checking for edge cases
  if (altitude > 90) {
    return;
  }
  if (azimuth > 360) {
    return;
  }
  if (azimuth > 180) {
    azimuth = azimuth - 180;
  }

  //delay parameter
  uint8_t speed = 10;

  // currentPositionAz = myServoAlt.read();
  // currentPositionAlt = myServoAlt.read();
  if (altitude > currentPositionAlt) {
    while (altitude > currentPositionAlt) {
      currentPositionAlt++;
      myServoAlt.write(currentPositionAlt);
      delay(speed);
    }
  } else if (currentPositionAlt > altitude) {
    while (currentPositionAlt > altitude) {
      currentPositionAlt--;
      myServoAlt.write(currentPositionAlt);
      delay(speed);
    }
  }

  if (azimuth > currentPositionAz) {
    while (azimuth > currentPositionAz) {
      currentPositionAz++;
      myServoAz.write(currentPositionAz);
      delay(speed);
    }
  } else if (currentPositionAz > azimuth) {
    while (currentPositionAz > azimuth) {
      currentPositionAz--;
      myServoAz.write(currentPositionAz);
      delay(speed);
    }
  }
}

// Function to receive a message via LoRa
int receiveLoRaMessage() {
    if (rf95.waitAvailableTimeout(LISTENING_INTERVAL)) {
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        if (rf95.recv(buf, &len)) {
            SerialUSB.print("Received: ");
            SerialUSB.println((char*)buf);

            uint8_t altitude = (uint8_t)buf[0];
            uint8_t azimuth = (uint8_t)buf[1];
            
            setServoPosition(azimuth, altitude);
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
