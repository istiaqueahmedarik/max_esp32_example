
#include <Wire.h>
#include "MAX30105.h"
#include <Arduino.h>
#include "WebSocketServer.h"
WebSocketServer wsServer(80, "/ws");

MAX30105 particleSensor;

void setup()
{
    Serial.begin(115200);
    wsServer.begin("Victus_Ext", "1732020@4G");
    Serial.println("MAX30105 Basic Readings Example");
    if (particleSensor.begin() == false)
    {
        Serial.println("MAX30105 was not found. Please check wiring/power. ");
        while (1)
            ;
    }

    particleSensor.setup(); // Configure sensor. Use 6.4mA for LED drive
}

void loop()
{
    // Serial.print(" R[");
    // Serial.print(particleSensor.getRed());
    // Serial.print("] IR[");
    // Serial.print(particleSensor.getIR());
    // Serial.print("] G[");
    // Serial.print(particleSensor.getGreen());
    // Serial.print("]");

    // Serial.println();
    char *msg = (char *)malloc(1000);
    // sprintf(msg, "{\"red\":%d, \"ir\":%d, \"green\":%d}", particleSensor.getRed(), particleSensor.getIR(), particleSensor.getGreen());
    // {"type":"rig","data":{"red":0,"ir":0,"green":0}}
    sprintf(msg, "{\"type\":\"rig\",\"data\":{\"red\":%d,\"ir\":%d,\"green\":%d}}", particleSensor.getRed(), particleSensor.getIR(), particleSensor.getGreen());
    // wsServer.send(msg);
    wsServer.sendMsg(msg);
    delay(200);
}
