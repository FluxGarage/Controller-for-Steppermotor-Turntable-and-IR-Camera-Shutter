/**
  * FLUXGARAGE - ADAVANCED CONTROLLER FOR AUTOMATED TURNTABLE WITH STEPPERMOTOR AND MULTI CAMERA IR CONTROL 
  * 
  * Please note the third party licenses of the included libraries and code snippets:
  * "multi camera IR control" from Sebastian Setz -> https://github.com/dharmapurikar/Arduino/tree/master/libraries/multiCameraIrControl
  * "stepper with your arduino" code snippet from bildr.org -> more information below
  * The piezo sounds are based on Mike Greb's code snippet -> http://michael.thegrebs.com/2009/03/23/playing-a-tone-through-an-arduino-connected-piezo/
  * The Adafruit-RGB-LCD-Shield-Library -> https://github.com/adafruit/Adafruit-RGB-LCD-Shield-Library  
  * For more information visit www.fluxgarage.com
  * 
  * MULTI CAMERA IR CONTROL
  * Name.......:  multiCameraIrControl Library
  * Author.....:  Sebastian Setz
  * Version....:  1.1
  * Date.......:  2010-12-16
  * Project....:  http://sebastian.setz.name/arduino/my-libraries/multi-camera-ir-control
  * Contact....:  http://Sebastian.Setz.name
  * License....:  This work is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
  *               To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-sa/3.0/ or send a letter to
  *               Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
  * Keywords...:  arduino, library, camera, ir, control, canon, nikon, olympus, minolta, sony, pentax, interval, timelapse
  * 
  *
  * STEPPER WITH YOUR ARDUINO
  * ©2011 bildr.org
  * Released under the MIT License - Please reuse change and share
  * Using the easy stepper with your arduino
  * use rotate and/or rotateDeg to controll stepper motor
  * speed is any number from .01 -> 1 with 1 being fastest - 
  * Slower Speed == Stronger movement*
  *
**/


// Library for multi camera control
#include <multiCameraIrControl.h>

// Set Camera Type
Nikon D5000(12);
// Canon D5(12);
// Minolta A900(12);
// Olympus E5(12);
// Pentax K7(12);
// Sony A900(12);

// Library for Display Shield
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

// The Display + Keypad shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// These #defines make it easy to set the display's backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7


// Stepper Motor Pins
#define DIR_PIN 2
#define STEP_PIN 3

// Piezo Sound Pin
#define PIEZOPIN 9

// Button Pin
#define BUTTONPIN 4

//Camera
#define PIN_IR_LED 12 // IR LED for Nikon Camera Shutter


// Turntable controller variables
int shootingsteps = 20; // number of steps, should be 10, 20 or 40
float shootingspeed = 0.03; // rotation speed
int shootingdelay = 1000; // break in milliseconds before and after each rotation
int shootingstatus = 0; // Photoshooting status for stop button (don't change!)


// Set menu variables
int column = 0;
int columnspread = 4; // defining the space between the characters
int columnshift = 1; // shifting the Menu to the right (shifting to the left with negative values)
int valueArray[] = {1, 0, 2, 1}; // Set initial default values from array "realvalues"
int valueContraints[] = {2, 4, 4, 1}; // define how many values are saved in the array

// Set menu shortcuts
char* menunames[] = {"ST","SP","DE","LI"};

// Array with real values, those values will be used in the photoshooting function 
int realvalues[4][5] = {
{10, 20, 40},
{1, 2, 3, 4, 5},
{0, 5, 10, 25, 50},
{0, 1},
};

// custom FluxGarage logo sign for splashscreen
byte fluxlogo[8] = {
  B00000,
  B00100,
  B00100,
  B00100,
  B01010,
  B10001,
  B00000,
};
   


void setup() {

  // Stepper Motor Pins
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 

  // Button Pin
  pinMode(BUTTONPIN, INPUT); 

  // Piezo Sound Pin
  pinMode(PIEZOPIN, OUTPUT);
  
  // Infrared LED Pin
  pinMode(PIN_IR_LED, OUTPUT);

  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  // set up the LCD's background-color
  lcd.setBacklight(WHITE);

  // Displaying welcome-message
  lcd.createChar(0, fluxlogo);
  lcd.setCursor(2,0);
  lcd.write(byte(0));
  lcd.print(" FluxGarage");
  lcd.setCursor(3,1);
  lcd.print("loading...");
  delay(4000);

  // initially displays complete menu value array after initialisation
  initialprint();

}
 
uint8_t i=0;

void loop() {
uint8_t buttons = lcd.readButtons();
if (buttons) {
    
    if (buttons & BUTTON_UP) {
      if (valueArray[column]<valueContraints[column]){valueArray[column]++;} else {valueArray[column]=0;}
    }
    if (buttons & BUTTON_DOWN) {
     if (valueArray[column]>0){valueArray[column]--;} else {valueArray[column]=valueContraints[column];}
    }
    if (buttons & BUTTON_LEFT) {
      if (column>0){column--;} else {column=3;}
      lcd.setCursor(column*columnspread+columnshift, 0);
    }
    if (buttons & BUTTON_RIGHT) {
      if (column<3){column++;} else {column=0;}
      lcd.setCursor(column*columnspread+columnshift, 0);
      shootingstatus=0;
    }
    if (buttons & BUTTON_SELECT) {
      shootingstatus=1; photoshooting();
    }    
    
    // Printing column value from array
    lcd.setCursor(column*columnspread+columnshift, 1);
    lcd.print(realvalues[column][valueArray[column]]);
    lcd.print(" "); // insert blank
    lcd.setCursor(column*columnspread+columnshift, 1); //set cursor back to the right column
    
    // delay for button debouncing
    delay(200);
  }
  
}


// FUNCTIONS //


// Display complete menu value array
void initialprint () {
  lcd.clear();
  // Printing var column initially
    for (column=0;column<4;column++) {
    lcd.setCursor(column*columnspread+columnshift, 0);
    lcd.print(menunames[column]);
  }

  // Printing all values to LCD at once initially
  for (column=0;column<4;column++) {
    lcd.setCursor(column*columnspread+columnshift, 1);
    //lcd.print(valueArray[column]);
    lcd.print(realvalues[column][valueArray[column]]);
  }
  // Setting var column back to 0
  column = 0;
  lcd.setCursor(column*columnspread+columnshift, 1); //set cursor back to the right column
  
  // Activate cursor 
  lcd.cursor(); //  visible cursor
  //lcd.blink(); // blinking cursor
}


// Take a picture
void snap(){
 D5000.shotNow();
 //D5.shotNow();
 //A900.shotNow();
 //E5.shotNow();
 //K7.shotNow();
 //A900.shotNow();
}


// Performing Photoshoot
void photoshooting() {

  // Play start sound
  startsound();

  // turn off light, if defined in the menu
  if (valueArray[3]==0){lcd.setBacklight(LOW);}
  
  // progress information
  lcd.clear();
  lcd.noCursor(); // hide cursor
  lcd.setCursor(0,0);
  lcd.print("Performing");
  lcd.setCursor(0,1);
  lcd.print("Step"); //status information, will be continued in the for loop
  
  // translating variables
  int shootingsteps = realvalues[0][valueArray[0]]; // mumber of steps
  float shootingspeed = realvalues[1][valueArray[1]]*0.01; // rotation speed
  int shootingdelay = realvalues[2][valueArray[2]]*100; // break between each rotation


  //rotate a specific number of microsteps (8 microsteps per step)
  //a 200 step stepper would take 1600 micro steps for one full revolution    
  for(int a=0;a<shootingsteps;a++){
    if (shootingstatus==1) {
      lcd.setCursor(5,1);
      lcd.print(a+1);
      lcd.print(" of ");
      lcd.print(shootingsteps);
      snap(); // Take a picture
      delay(shootingdelay);
      rotateDeg(360/shootingsteps, shootingspeed); // Rotate turntable
      lcd.setCursor(0,0);
      lcd.print("Waiting...");
      delay(shootingdelay);
      lcd.setCursor(0,0);
      lcd.print("Performing");
      stopbutton(); // check if button is pressed and shootingstatus has changed
    }
    else {break;}
  }

  // Clear display, print menu values, activate cursor
  initialprint();
  lcd.cursor();

  // turn display light on
  lcd.setBacklight(HIGH);
  lcd.setBacklight(WHITE);

  delay(500);

  // Play finish sound
  finishsound();

}



// Button sound
void buttonsound() {
    playTone(5, 50);
}

// Start sound
void startsound() {
    playTone(10, 1200);
    delay(50);
    playTone(20, 1500);    
}

// Finish sound
void finishsound() {
    playTone(20, 1500);    
    delay(50);
    playTone(10, 1200);
}

// Interrupt sound
void interruptsound() {
    playTone(30, 500);    
    delay(50);
    playTone(30, 300);
    delay(50);
    playTone(70, 200);
}


// Function that generates the sounds
// duration in mSecs, frequency in hertz
void playTone(long duration, int freq) {
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    while (elapsed_time < duration) {
        digitalWrite(PIEZOPIN,HIGH);
        delayMicroseconds(period / 2);
        digitalWrite(PIEZOPIN, LOW);
        delayMicroseconds(period / 2);
        elapsed_time += (period);
    }
}

void rotate(int steps, float speed){ 
  //rotate a specific number of microsteps (8 microsteps per step) - (negative for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (steps > 0)? HIGH:LOW;
  steps = abs(steps);

  digitalWrite(DIR_PIN,dir); 

  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
} 

void rotateDeg(float deg, float speed){ 
  //rotate a specific number of degrees (negative for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
  int dir = (deg > 0)? HIGH:LOW;
  digitalWrite(DIR_PIN,dir); 

  int steps = abs(deg)*(1/0.225);
  float usDelay = (1/speed) * 70;

  for(int i=0; i < steps; i++){ 
    digitalWrite(STEP_PIN, HIGH); 
    delayMicroseconds(usDelay); 

    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(usDelay); 
  } 
}

void stopbutton () {
  uint8_t i=0;
  uint8_t buttons = lcd.readButtons();

  if (buttons) {
    if (buttons & BUTTON_SELECT) {
      shootingstatus=0;
      interruptsound();
    }    
  }
}