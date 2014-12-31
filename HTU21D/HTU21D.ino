// PIR example
#include <Wire.h>
#include <HTU21D.h>
#include <SoftwareSerial.h>

SoftwareSerial display(3, 2);

//Create an instance of the object
HTU21D myHumidity;

char humstring[10];
char tmpstring[10];

float humidity;
float temperature;

int ihum;
int itemp;

void setup()
{
  Wire.begin();        // Join i2c bus
  Serial.begin(9600);  // Start serial for output

  myHumidity.begin(); // Get sensor online

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
  humidity = myHumidity.readHumidity();
  ihum = humidity;
  sprintf(humstring, "%2d", ihum);

  Serial.print("Humidity: ");
  Serial.println(humstring);
  Serial.print("%");

  display.write("Humidity: ");
  display.write(humstring);

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  delay(250);
}

