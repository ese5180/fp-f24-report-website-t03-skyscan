#include <SPI.h>
#include <LoRa.h>

#define SS_PIN 18
#define RST_PIN 23
#define DIO0_PIN 26

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ;

    LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
    if (!LoRa.begin(915E6))
    {
        Serial.println("LoRa initialization failed!");
        while (1)
            ;
    }
}

void loop()
{
    // Check for incoming UART data
    if (Serial.available())
    {
        String packet = Serial.readStringUntil('\n');
        forwardLoRaPacket(packet);
    }

    // Check for incoming LoRa packets
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String receivedPacket = receiveLoRaPacket();
        forwardUartPacket(receivedPacket);
    }
}

void forwardLoRaPacket(String packet)
{
    LoRa.beginPacket();
    LoRa.print(packet);
    LoRa.endPacket();
    Serial.println("Forwarded LoRa packet: " + packet);
}

String receiveLoRaPacket()
{
    String packet = "";
    while (LoRa.available())
    {
        packet += (char)LoRa.read();
    }
    return packet;
}

void forwardUartPacket(String packet)
{
    Serial.println(packet);
}