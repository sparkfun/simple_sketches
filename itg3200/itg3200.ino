#include <SPI.h>
#include <Wire.h>

// Pin definitions - Shift registers:
int enPin = 13;  // Shift registers' Output Enable pin
int latchPin = 12;  // Shift registers' rclk pin
int clkPin = 11;  // Shift registers' srclk pin
int clrPin = 10;  // shift registers' srclr pin
int datPin = 8;  // shift registers' SER pin

int show = 0;
int lastMax = 0;


//This is a list of registers in the ITG-3200. Registers are parameters that determine how the sensor will behave, or they can hold data that represent the
//sensors current status.
//To learn more about the registers on the ITG-3200, download and read the datasheet.
char WHO_AM_I = 0x00;
char SMPLRT_DIV= 0x15;
char DLPF_FS = 0x16;
char GYRO_XOUT_H = 0x1D;
char GYRO_XOUT_L = 0x1E;
char GYRO_YOUT_H = 0x1F;
char GYRO_YOUT_L = 0x20;
char GYRO_ZOUT_H = 0x21;
char GYRO_ZOUT_L = 0x22;

//This is a list of settings that can be loaded into the registers.
//DLPF, Full Scale Register Bits
//FS_SEL must be set to 3 for proper operation
//Set DLPF_CFG to 3 for 1kHz Fint and 42 Hz Low Pass Filter
char DLPF_CFG_0 = 1;
char DLPF_CFG_1 = 2;
char DLPF_CFG_2 = 4;
char DLPF_FS_SEL_0 = 8;
char DLPF_FS_SEL_1 = 16;

char itgAddress = 0x69;

// Some of the math we're doing in this example requires the number of bargraph boards
// you have connected together (normally this is one, but you can have a maximum of 8).

void setup()
// Runs once upon reboot
{
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
  
  //Initialize the I2C communication. This will set the Arduino up as the 'Master' device.
  Wire.begin();
  
  //Read the WHO_AM_I register and print the result
  char id=0; 
  id = itgRead(itgAddress, 0x00);  
  Serial.print("ID: ");
  Serial.println(id, HEX);
  
  //Configure the gyroscope
  //Set the gyroscope scale for the outputs to +/-2000 degrees per second
  itgWrite(itgAddress, DLPF_FS, (DLPF_FS_SEL_0|DLPF_FS_SEL_1|DLPF_CFG_0));
  //Set the sample rate to 100 hz
  itgWrite(itgAddress, SMPLRT_DIV, 9);
}

void loop()
// Runs continuously after setup() ends
{
  static int zero = 0;
  
  // Create variables to hold the output rates.
  int xRate, yRate, zRate;
  float range = 3000.0;
  int divisor;

  divisor = range / 8;
  
  //Read the x,y and z output rates from the gyroscope.
  xRate = int(float(readX()) / divisor - 0.5) * -1;
  yRate = int(float(readY()) / divisor - 0.5) * -1;
  zRate = int(float(readZ()) / divisor - 0.5);

  //Print the output rates to the terminal, seperated by a TAB character.
  /*
  Serial.print(xRate);
  Serial.print('\t');
  Serial.print(yRate);
  Serial.print('\t');
  Serial.println(zRate);  
  Serial.print('\t');
  Serial.println(zero);
  */

  fillTo(zRate);

  //Wait 10ms before reading the values again. (Remember, the output rate was set to 100hz and 1reading per 10ms = 100hz.)
  delay(10);
}


// This function will write a value to a register on the itg-3200.
// Parameters:
//   char address: The I2C address of the sensor. For the ITG-3200 breakout the address is 0x69.
//   char registerAddress: The address of the register on the sensor that should be written to.
//   char data: The value to be written to the specified register.
void itgWrite(char address, char registerAddress, char data)
{
  //Initiate a communication sequence with the desired i2c device
  Wire.beginTransmission(address);
  //Tell the I2C address which register we are writing to
  Wire.write(registerAddress);
  //Send the value to write to the specified register
  Wire.write(data);
  //End the communication sequence
  Wire.endTransmission();
}

//This function will read the data from a specified register on the ITG-3200 and return the value.
//Parameters:
//  char address: The I2C address of the sensor. For the ITG-3200 breakout the address is 0x69.
//  char registerAddress: The address of the register on the sensor that should be read
//Return:
//  unsigned char: The value currently residing in the specified register
unsigned char itgRead(char address, char registerAddress)
{
  //This variable will hold the contents read from the i2c device.
  unsigned char data=0;
  
  //Send the register address to be read.
  Wire.beginTransmission(address);
  //Send the Register Address
  Wire.write(registerAddress);
  //End the communication sequence.
  Wire.endTransmission();
  
  //Ask the I2C device for data
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 1);
  
  //Wait for a response from the I2C device
  if(Wire.available()){
    //Save the data sent from the I2C device
    data = Wire.read();
  }
  
  //End the communication sequence.
  Wire.endTransmission();
  
  //Return the data read during the operation
  return data;
}

//This function is used to read the X-Axis rate of the gyroscope. The function returns the ADC value from the Gyroscope
//NOTE: This value is NOT in degrees per second. 
//Usage: int xRate = readX();
int readX(void)
{
  int data=0;
  data = itgRead(itgAddress, GYRO_XOUT_H)<<8;
  data |= itgRead(itgAddress, GYRO_XOUT_L);  
  
  return data;
}

//This function is used to read the Y-Axis rate of the gyroscope. The function returns the ADC value from the Gyroscope
//NOTE: This value is NOT in degrees per second. 
//Usage: int yRate = readY();
int readY(void)
{
  int data=0;
  data = itgRead(itgAddress, GYRO_YOUT_H)<<8;
  data |= itgRead(itgAddress, GYRO_YOUT_L);  
  
  return data;
}

//This function is used to read the Z-Axis rate of the gyroscope. The function returns the ADC value from the Gyroscope
//NOTE: This value is NOT in degrees per second. 
//Usage: int zRate = readZ();
int readZ(void)
{
  int data=0;
  data = itgRead(itgAddress, GYRO_ZOUT_H)<<8;
  data |= itgRead(itgAddress, GYRO_ZOUT_L);  
  
  return data;
}

void fillTo(int place) {
  int ledOutput = 0;

  if(place > 8)
    place = 8;
  if(place < -8)
    place = -8;

  if(place >= 0) {
    for (int i = place; i >= 0; i--)
      ledOutput |= 1 << i;
  } else {
    ledOutput = 32768;
    for (int i = place; i <= 0; i++)
      ledOutput |= (ledOutput >> 1);
  }

  Serial.println(ledOutput);

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

