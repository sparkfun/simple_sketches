// pressure resistor example
#include <SoftwareSerial.h>

SoftwareSerial display(3, 2);

char flexstring[10];
int flex, degrees;

void setup()
{
  Serial.begin(9600);  // Start serial for output

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
  flex = analogRead(0);

  degrees = map(flex, 695, 853, 0, 90);

  Serial.print(flex);
  Serial.print(" ");
  Serial.println(degrees);
  sprintf(flexstring, "%3d", degrees);

  display.write("flex: ");
  display.write(flexstring);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  delay(100);
}

