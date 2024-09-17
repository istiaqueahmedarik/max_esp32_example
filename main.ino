
#include <Wire.h>
#include "MAX30105.h"
#include <Arduino.h>
#include "WebSocketServer.h"
WebSocketServer wsServer(80, "/ws");

MAX30105 particleSensor;
const int xpin = 4;  // XOUT connected to GPIO 32
const int ypin = 2;  // YOUT connected to GPIO 33
const int zpin = 15;  // ZOUT connected to GPIO 34

int stepCount = 0;    // Step counter
float prevZ = 0;      // Previous Z-axis value for detecting peaks
bool stepDetected = false; // Flag to avoid double-counting steps
const float stepThreshold = 3;  // G-force threshold for a step
const unsigned long debounceDelay = 300;  // 300 ms debounce delay
unsigned long lastStepTime = 0;  // Timestamp of last step detection


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
    analogReadResolution(10);
    particleSensor.setup(); // Configure sensor. Use 6.4mA for LED drive
}

void gyro()
{
// Read analog values from the accelerometer
  int x = analogRead(xpin);  // Read XOUT
  int y = analogRead(ypin);  // Read YOUT
  int z = analogRead(zpin);  // Read ZOUT

  // Convert raw readings into G-force values
  float Gx = ((float)x - 331.5) / 65 * 9.8;
  float Gy = ((float)y - 329.5) / 68.5 * 9.8;
  float Gz = ((float)z - 340) / 68 * 9.8;

  // Current timestamp
  unsigned long currentTime = millis();

  // Step detection based on Z-axis G-force change
  if (abs(Gz - prevZ) > stepThreshold && (currentTime - lastStepTime > debounceDelay)) {
    // A significant change in Z-axis acceleration indicates a step
    stepCount++;
    lastStepTime = currentTime;  // Update the timestamp of the last step
    stepDetected = true;
    Serial.println("Step detected!");
  } else {
    stepDetected = false;
  }

  // Store the current Z-axis value for the next loop iteration
  prevZ = Gz;

  // Output the current acceleration and step count to the serial monitor
  // Serial.print("Gx: ");
  // Serial.print(Gx);
  // Serial.print("\tGy: ");
  // Serial.print(Gy);
  // Serial.print("\tGz: ");
  // Serial.print(Gz);
  Serial.print("\tSteps: ");
  Serial.println(stepCount);


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

    char *msg1 = (char *)malloc(1000);
    sprintf(msg1, "{\"type\":\"gyro\",\"data\":{\"step\":%d}}",stepCount );
    wsServer.sendMsg(msg1);
    delay(200);
}
