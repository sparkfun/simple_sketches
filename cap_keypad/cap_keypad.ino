#include "mpr121.h"
#include "i2c.h"
#include <SoftwareSerial.h>

#define MPR121_R 0xB5    // ADD pin is grounded
#define MPR121_W 0xB4   // So address is 0x5A

#define PHONE_DIGITS 10  // 10 digits in a phone number

// Match key inputs with electrode numbers
#define STAR 0
#define SEVEN 1
#define FOUR 2
#define ONE 3
#define ZERO 4
#define EIGHT 5
#define FIVE 6
#define TWO 7
#define POUND 8
#define NINE 9
#define SIX 10
#define THREE 11

int irqpin = 3;  // D2

uint16_t touchstatus;
char digit;

SoftwareSerial display(3, 2);

void setup()
{
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH);

  Serial.begin(9600);
  display.begin(9600);

  DDRC |= 0b00010011;
  PORTC = 0b00110000;  // Pull-ups on I2C Bus
  i2cInit();

  delay(100);
  mpr121QuickConfig();

  display.write(254); // move cursor to beginning of first line
  display.write(128);

  display.write("                "); // clear display
  display.write("                ");
}

int dir = 1;
void loop()
{
  char toprint = getDigit();

  if(dir) {
    Serial.print(toprint);
    display.write(toprint);
    delay(100);
    dir = 0;
  } else {
    dir = 1;
  }
}

char getDigit()
{
  checkInterrupt();

  while(checkInterrupt())
    ;

  int touchNumber = 0;

  touchstatus = mpr121Read(0x01) << 8;
  touchstatus |= mpr121Read(0x00);

  for (int j=0; j<12; j++) {  // Check how many electrodes were pressed
    if ((touchstatus & (1<<j)))
      touchNumber++;
  }

  if (touchNumber == 1) {
    if (touchstatus & (1<<STAR))
      digit = '*';
    else if (touchstatus & (1<<SEVEN))
      digit = '7';
    else if (touchstatus & (1<<FOUR))
      digit = '4';
    else if (touchstatus & (1<<ONE))
      digit = '1';
    else if (touchstatus & (1<<ZERO))
      digit = '0';
    else if (touchstatus & (1<<EIGHT))
      digit = '8';
    else if (touchstatus & (1<<FIVE))
      digit = '5';
    else if (touchstatus & (1<<TWO))
      digit = '2';
    else if (touchstatus & (1<<POUND))
      digit = '#';
    else if (touchstatus & (1<<NINE))
      digit = '9';
    else if (touchstatus & (1<<SIX))
      digit = '6';
    else if (touchstatus & (1<<THREE))
      digit = '3';
  } else if (touchNumber == 0)
    ;
  else
    Serial.println("Only touch ONE button!");

  return digit;
}

byte checkInterrupt(void)
{
  if(digitalRead(irqpin)) {
    return 1;
  }

  return 0;
}

