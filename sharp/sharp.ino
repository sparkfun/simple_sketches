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

  int calculated = (6762/(reading-9))-4;

  Serial.println(calculated);

  sprintf(cmstring, "%3d", calculated);
  display.write("distance: ");
  display.write(cmstring);
  display.write("cm");

  delay(200);
}

