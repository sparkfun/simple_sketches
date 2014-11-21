/****************************************************************
apds9960.ino
APDS-9960 RGB and Gesture Sensor
Shawn Hymel @ SparkFun Electronics
October 29, 2014
https://github.com/sparkfun/APDS-9960_RGB_and_Gesture_Sensor

Shows how to control the MicroView using the APDS-9960 Gesture
Sensor. While idling, the MicroView displays graphs of the
current ambient, red, green, and blue light. When a gesture (UP,
DOWN, LEFT, RIGHT, NEAR, or FAR) is detected, a graphic is drawn
on the MicroView depicting that gesture.

Hardware Connections:

IMPORTANT: The APDS-9960 can only accept 3.3V!
 
 MicroView    APDS-9960 Board  FTDI Breakout  Notes
 
 VIN          VCC              3V3            Power
 GND          GND              GND            Ground
 A4           SDA              -              I2C Data
 A5           SCL              -              I2C Clock
 2            INT              -              Interrupt
 RST          -                DTR            0.1uF in series
 RX           -                RXI            Programming (Rx)
 TX           -                TXO            Programming (Tx)
 
Resources:
Include Wire.h, SFE_APDS-9960.h, and MicroView.h

Development environment specifics:
Written in Arduino 1.0.5
Tested with MicroView

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please
buy us a round!

Distributed as-is; no warranty is given.
****************************************************************/

#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include <MicroView.h>

// Pins
#define APDS9960_INT    2 // Needs to be an interrupt pin

// Constants
#define READ_LIGHT      0    // 1 to read light, 0 to not
#define LIGHT_MAX       2000 // Max value of light readings

// Global Variables
MicroViewWidget *widget_c, *widget_r, *widget_g, *widget_b;
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 0;
uint16_t green_light = 0;
uint16_t blue_light = 0;
int isr_flag = 0;

void setup() {
  
  // Initialize Serial port
  Serial.begin(9960);
  Serial.println();
  Serial.println("MicroView Gesture Demo");
  
  // Initialize MicroView
  uView.begin();
  uView.clear(PAGE);
  
  // Initialize interrupt service routine
  attachInterrupt(0, interruptRoutine, FALLING);
  
#if READ_LIGHT
  // Create sliders
  widget_c = new MicroViewSlider(7, 10, 0, LIGHT_MAX);
  widget_r = new MicroViewSlider(7, 20, 0, LIGHT_MAX);
  widget_g = new MicroViewSlider(7, 30, 0, LIGHT_MAX);
  widget_b = new MicroViewSlider(7, 40, 0, LIGHT_MAX);
#endif
  
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
#if READ_LIGHT
  // Set light gain value
  if ( !apds.setAmbientLightGain(AGAIN_1X) ) {
    Serial.println(F("Something went wrong trying to set gain!"));
  }
  
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }
#endif
  
  // Start running the gesture sensor engine (with interrupts)
  if ( apds.enableGestureSensor(true) ) {
    Serial.println(F("Gesture sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during gesture sensor init!"));
  }
  
  // Wait for initialization and calibration to finish
  delay(500);
}

void loop() {
  
  // If interrupt, display gesture on MicroView
  if ( isr_flag == 1 ) {
    handleGesture();
    isr_flag = 0;
    delay(500);
  }
  
  // Draw title and C, R, G, B labels
  uView.setFontType(0);
  uView.setCursor(16, 0);
  uView.print("Swipe!");
#if READ_LIGHT
  uView.setCursor(0, 10);
  uView.print("C");
  uView.setCursor(0, 20);
  uView.print("R");
  uView.setCursor(0, 30);
  uView.print("G");
  uView.setCursor(0, 40);
  uView.print("B");
  
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {
    if ( ambient_light > LIGHT_MAX ) ambient_light = LIGHT_MAX;
    if ( red_light > LIGHT_MAX ) red_light = LIGHT_MAX;
    if ( green_light > LIGHT_MAX ) green_light = LIGHT_MAX;
    if ( blue_light > LIGHT_MAX ) blue_light = LIGHT_MAX;
    widget_c->setValue(ambient_light);
    widget_r->setValue(red_light);
    widget_g->setValue(green_light);
    widget_b->setValue(blue_light);
  }
#endif
  
  // Update display
  uView.display();
  
  // Wait before next reading
  delay(100);
}

void interruptRoutine() {
  isr_flag = 1;
}

void handleGesture() {
  bool do_clear = true;
  
  // Draw symbol based on gesture
  if ( apds.isGestureAvailable() ) {
    switch ( apds.readGesture() ) {
      case DIR_UP:
        drawArrowUp();
        Serial.println("UP");
        break;
      case DIR_DOWN:
        drawArrowDown();
        Serial.println("DOWN");
        break;
      case DIR_LEFT:
        drawArrowLeft();
        Serial.println("LEFT");
        break;
      case DIR_RIGHT:
        drawArrowRight();
        Serial.println("RIGHT");
        break;
      case DIR_NEAR:
        drawCircle();
        Serial.println("NEAR");
        break;
      case DIR_FAR:
        drawX();
        Serial.println("FAR");
        break;
      default:
        Serial.println("NONE");
        do_clear = false;
    }
  }
  
  // Let symbol sit on screen for a while, then re-draw sliders
  if ( do_clear ) {
    uView.clear(PAGE);
#if READ_LIGHT
    widget_c->reDraw();
    widget_r->reDraw();
    widget_g->reDraw();
    widget_b->reDraw();
#endif
  }
}

void drawArrowUp() {
  uView.clear(PAGE);
  uView.line(27, 43, 37, 43);  // Bottom: over 10
  uView.line(27, 43, 27, 18);  // Legs: Up 25
  uView.line(37, 43, 37, 18);  // Legs: Up 25
  uView.line(27, 18, 17, 18);  // Head: Over 10
  uView.line(37, 18, 47, 18);  // Head: Over 10
  uView.line(17, 18, 32, 3);   // Tip: Over 14, up 15
  uView.line(47, 18, 32, 3);   // Tip: Over 14, up 15
  uView.display();
}

void drawArrowDown() {
  uView.clear(PAGE);
  uView.line(27, 5, 37, 5);  // Bottom: over 10
  uView.line(27, 5, 27, 30);  // Legs: Up 25
  uView.line(37, 5, 37, 30);  // Legs: Up 25
  uView.line(27, 30, 17, 30);  // Head: Over 10
  uView.line(37, 30, 47, 30);  // Head: Over 10
  uView.line(17, 30, 32, 45);   // Tip: Over 14, up 15
  uView.line(47, 30, 32, 45);   // Tip: Over 14, up 15
  uView.display();
}

void drawArrowLeft() {
  uView.clear(PAGE);
  uView.line(52, 19, 52, 29);  // Bottom: up 10
  uView.line(52, 19, 27, 19);  // Legs: over 25
  uView.line(52, 29, 27, 29);  // Legs: over 25
  uView.line(27, 19, 27, 9);   // Head: up 10
  uView.line(27, 29, 27, 39);  // Head: down 10
  uView.line(27, 9, 12, 24);   // Tip: over 15, down 14
  uView.line(27, 39, 12, 24);  // Tip: over 15, down 14
  uView.display();
}

void drawArrowRight() {
  uView.clear(PAGE);
  uView.line(12, 19, 12, 29);  // Bottom: up 10
  uView.line(12, 19, 37, 19);  // Legs: over 25
  uView.line(12, 29, 37, 29);  // Legs: over 25
  uView.line(37, 19, 37, 9);   // Head: up 10
  uView.line(37, 29, 37, 39);  // Head: down 10
  uView.line(37, 9, 53, 25);   // Tip: over 15, down 14
  uView.line(37, 39, 52, 24);  // Tip: over 15, down 14
  uView.display();
}

void drawCircle() {
  uView.clear(PAGE);
  uView.circle(32, 24, 20);
  uView.display();
}

void drawX() {
  uView.clear(PAGE);
  uView.line(12, 4, 52, 44);
  uView.line(12, 44, 52, 4);
  uView.display();
}
