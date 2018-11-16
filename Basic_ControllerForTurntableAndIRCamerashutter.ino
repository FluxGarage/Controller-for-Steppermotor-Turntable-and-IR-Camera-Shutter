/**
  * FLUXGARAGE - SIMPLE PUSHBUTTON CONTROLLER FOR AUTOMATED TURNTABLE WITH STEPPERMOTOR AND MULTI CAMERA IR CONTROL 
  * 
  * Please note the third party licenses of the included libraries and code snippets:
  * "multi camera IR control" from Sebastian Setz -> https://github.com/dharmapurikar/Arduino/tree/master/libraries/multiCameraIrControl
  * "stepper with your arduino" code snippet from bildr.org -> more information below
  * The piezo sounds are based on Mike Greb's code snippet -> http://michael.thegrebs.com/2009/03/23/playing-a-tone-through-an-arduino-connected-piezo/
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


// Stepper Motor Pins
#define DIR_PIN 2
#define STEP_PIN 3

// Piezo Sound Pin
#define PIEZOPIN 9

// Button Pin
#define BUTTONPIN 4

// Status LED Pin
#define PIN_STATUS_LED 13 

//Camera
#define PIN_IR_LED 12 // IR LED for Nikon Camera Shutter



// Turntable controller variables
int shootingsteps = 20; // number of steps, should be 10, 20 or 40
float shootingspeed = 0.01; // rotation speed
int shootingdelay = 1000; // break in milliseconds before and after each rotation
int shootingstatus = 0; // Photoshooting status for stop button (don't change!)
   


void setup() {

  // Stepper Motor Pins
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 

  // Button Pin
  pinMode(BUTTONPIN, INPUT); 

  // Piezo Sound Pin
  pinMode(PIEZOPIN, OUTPUT);
  
  // Status LED Pin
  pinMode(PIN_STATUS_LED, OUTPUT);

  // Infrared LED Pin
  pinMode(PIN_IR_LED, OUTPUT);

}
 

void loop() {
  // turn status LED on
  digitalWrite(PIN_STATUS_LED,  1);
 
  if (digitalRead(BUTTONPIN)==HIGH){
  shootingstatus=1;
  photoshooting();    // start photoshooting
  }
  delay(200); // delay for button debouncing
}


// FUNCTIONS //


// take a picture
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

    // turn status LED off
  digitalWrite(PIN_STATUS_LED,  0);
  // Play start sound
  startsound();
     
  //rotate a specific number of microsteps (8 microsteps per step)
  //a 200 step stepper would take 1600 micro steps for one full revolution    
  for(int a=0;a<shootingsteps;a++){
    if (shootingstatus==1) {
    snap();
    delay(shootingdelay);
    // Rotate turntable
    rotateDeg(360/shootingsteps, shootingspeed);
    delay(shootingdelay);
    stopbutton(); // check if button is pressed and shootingstatus has changed
    }
    else {break;}
  }

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
  if (digitalRead(BUTTONPIN)==HIGH){
  shootingstatus=0;
  interruptsound();
  }
}