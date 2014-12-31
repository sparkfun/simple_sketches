// PIR example
#include <SoftwareSerial.h>

SoftwareSerial display(3, 2);

const int pirPin = 4; 

void setup()
{
  pinMode(pirPin, INPUT);

  display.begin(9600);
  Serial.begin(9600);
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

  int pirVal = digitalRead(pirPin);

  if(pirVal == LOW){ //was motion detected
    Serial.println("Motion Detected!"); 
    display.write("Motion Detected!");
    delay(500);
  } else {
    Serial.println("All Quiet..."); 
    display.write("All Quiet...    ");
    delay(50);
  }
}

