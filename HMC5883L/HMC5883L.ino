#include <Wire.h>
#include <HMC5883L.h>

HMC5883L compass;

// Pin definitions - Shift registers:
int enPin = 13;  // Shift registers' Output Enable pin
int latchPin = 12;  // Shift registers' rclk pin
int clkPin = 11;  // Shift registers' srclk pin
int clrPin = 10;  // shift registers' srclr pin
int datPin = 8;  // shift registers' SER pin

void setup(){
  // Setup shift register pins
  pinMode(enPin, OUTPUT);  // Enable, active low, this'll always be LOW
  digitalWrite(enPin, LOW);  // Turn all outputs on
  pinMode(latchPin, OUTPUT);  // this must be set before calling shiftOut16()
  digitalWrite(latchPin, LOW);  // start latch low
  pinMode(clkPin, OUTPUT);  // we'll control this in shiftOut16()
  digitalWrite(clkPin, LOW);  // start sck low
  pinMode(clrPin, OUTPUT);  // master clear, this'll always be HIGH
  digitalWrite(clrPin, HIGH);  // disable master clear
  pinMode(datPin, OUTPUT);  // we'll control this in shiftOut16()
  digitalWrite(datPin, LOW);  // start ser low
  
  // To begin, we'll turn all LEDs on the circular bar-graph OFF
  digitalWrite(latchPin, LOW);  // first send latch low
  shiftOut16(0x0000);
  digitalWrite(latchPin, HIGH);  // send latch high to indicate data is done sending

  Serial.begin(9600);
  Wire.begin();
  
  compass = HMC5883L(); //new instance of HMC5883L library
  setupHMC5883L(); //setup the HMC5883L
}

void loop(){
  float heading = getHeading();
  //Serial.println(heading);

  int toshow = (360 - heading) / 22.5;
  fillTo(toshow);
  Serial.println(toshow);

  delay(100); //only here to slow down the serial print
}

void setupHMC5883L(){
  //Setup the HMC5883L, and check for errors
  int error;  
  error = compass.SetScale(1.3); //Set the scale of the compass.
  if(error != 0) Serial.println(compass.GetErrorText(error)); //check if there is an error, and print if so

  error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
  if(error != 0) Serial.println(compass.GetErrorText(error)); //check if there is an error, and print if so
}

float getHeading(){
  //Get the reading from the HMC5883L and calculate the heading
  MagnetometerScaled scaled = compass.ReadScaledAxis(); //scaled values from compass.
  float heading = atan2(scaled.YAxis, scaled.XAxis);

  // Correct for when signs are reversed.
  if(heading < 0) heading += 2*PI;
  if(heading > 2*PI) heading -= 2*PI;

  return heading * RAD_TO_DEG; //radians to degrees
}

void fillTo(int place) {
  int ledOutput = 0;

  ledOutput = 1 << place;

  digitalWrite(latchPin, LOW);  // first send latch low
  shiftOut16(ledOutput);  // send the ledOutput value to shiftOut16
  digitalWrite(latchPin, HIGH);  // send latch high to indicate data is done sending 
}

void shiftOut16(uint16_t data)
{
  byte datamsb;
  byte datalsb;
  
  // Isolate the MSB and LSB
  datamsb = (data & 0xFF00) >> 8;  // mask out the MSB and shift it right 8 bits
  datalsb = data & 0xFF;  // Mask out the LSB
  
  // First shift out the MSB, MSB first.
  shiftOut(datPin, clkPin, MSBFIRST, datamsb);
  // Then shift out the LSB
  shiftOut(datPin, clkPin, MSBFIRST, datalsb);
}
