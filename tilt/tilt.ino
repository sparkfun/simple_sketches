#include <SoftwareSerial.h>

SoftwareSerial display(3, 2);

void setup(void) {
  Serial.begin(9600);
  display.begin(9600);

  pinMode(4, INPUT);
  digitalWrite(4, HIGH);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  display.write("                "); // clear display
  display.write("                ");
}

void loop(void) {
  display.write(254); // move cursor to beginning of first line
  display.write(128);

  if(digitalRead(4)) {
    display.write("tilted!");
  } else {
    display.write("upright");
  }

  delay(20);
}

