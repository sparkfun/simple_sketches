// PIR example
#include <SoftwareSerial.h>
#include <Wire.h> // Must include Wire library for I2C
#include <SFE_MMA8452Q.h> // Includes the SFE_MMA8452Q library

MMA8452Q accel; // Default MMA8452Q object create. (Address = 0x1D)
SoftwareSerial display(3, 2);

const int irPin = A0;
char tmpstring[10];

void setup()
{
  pinMode(irPin, INPUT);

  display.begin(9600);
  Serial.begin(9600);
  accel.init(); // Default init: +/-2g and 800Hz ODR
  delay(500);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  display.write("                "); // clear display
  display.write("                ");
}

void loop()
{
  display.write(254); // move cursor to beginning of first line
  display.write(128);

  accel.read();

  int xAcceleration = accel.x; // Read in raw x-axis acceleration data
  if(xAcceleration > 1000)
    xAcceleration = 1000;
  if(xAcceleration < 0)
    xAcceleration = 0;

  int toprint = map(xAcceleration, 0, 1000, 0, 90);
  sprintf(tmpstring, "%2d", toprint);
  Serial.print("Acceleration on the x-axis is ");
  Serial.println(toprint);

  display.write("angle: ");
  display.write(tmpstring);

  delay(250);
}

