#include <SoftwareSerial.h>

SoftwareSerial display(3, 2);
char cmstring[10];

void setup(void) {
  Serial.begin(9600);
  display.begin(9600);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  display.write("                "); // clear display
  display.write("                ");
}

void loop(void) {
  display.write(254); // move cursor to beginning of first line
  display.write(128);

  int reading = analogRead(A5);

  float calculated = (6000/(float)(reading-9))-3.2;

  Serial.println(calculated);

  sprintf(cmstring, "%.3f", calculated);
  display.write("distance: ");
  display.write(cmstring);
  display.write("cm");

  delay(200);
}

