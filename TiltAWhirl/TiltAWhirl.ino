#include <SoftwareSerial.h>

SoftwareSerial display(3, 2);

const int tilt_s1 = 4;
const int tilt_s2 = 5;

char* positions[4] = {
  "up   ",
  "left ",
  "right",
  "down "
};

void setup(void) {
  Serial.begin(9600);
  display.begin(9600);

  pinMode(tilt_s1, INPUT);
  pinMode(tilt_s2, INPUT);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  display.write("                "); // clear display
  display.write("                ");


  Serial.println(positions[0]);
  Serial.println(positions[1]);
  Serial.println(positions[2]);
  Serial.println(positions[3]);
  delay(1000);
}

void loop(void) {
  int position = getTiltPosition();
  Serial.println(position);
  Serial.println(positions[position]);
  delay(200);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  display.write(positions[position]);

  delay(200);
}

int getTiltPosition() {
  int s1 = digitalRead(tilt_s1);
  int s2 = digitalRead(tilt_s2);
  return (s1 << 1) | s2;
}

