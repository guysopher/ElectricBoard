// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#define MIN_SIGNAL 900
#define MAX_SIGNAL 2000
#define INT_SPEED 10
#define MIN_SPEED 0
#define MAX_SPEED 10000
#define LED_PIN 8
#define MOTOR_PIN 9
#define SERVO_PIN 6
#define POT_PIN A0

#include <Servo.h> 
#include <Wire.h>
#include "WiiChuck.h"

Servo motor; 
Servo srv; 
WiiChuck chuck = WiiChuck();
 
int acc; //accelerator
int spd = 0; //speed 
int motor_signal; //the signal to pass to the motor
int servo_signal; //the signal to pass to the motor

void setup() 
{ 
  Serial.begin(9600);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("Program begin...");
  Serial.println("This program will calibrate the ESC.");

  srv.attach(SERVO_PIN);

  motor.attach(MOTOR_PIN);
  motor.writeMicroseconds(MAX_SIGNAL);    
  Serial.println("Now writing maximum output.");

  printWait(3000, 500);

  Serial.println("Sending minimum output");
  motor.writeMicroseconds(MIN_SIGNAL);

  printWait(3000, 500);
  
} 
 
void loop() 
{ 

  //wait for the remote control to connect
  while (!chuck.getStatus()){
    chuck.begin();
    chuck.update();
    printWait(1000, 200);
    yo("Waiting for remote...", chuck.getStatus(), true);
  }
  
  //READY!!!  
  digitalWrite(LED_PIN, LOW);

  chuck.update();
  
  if (chuck.zActive()){
    acc = (chuck.readJoyY());
    acc = map(acc, -128, 128, -1*INT_SPEED, INT_SPEED);
  
    spd += acc;
  }else{
    spd -= INT_SPEED;
  }    
  spd = max(MIN_SPEED, spd);
  spd = min(spd, MAX_SPEED);
  
  if (chuck.cActive()) spd -= (10 * INT_SPEED); //BRAKE!

  yo("spd", spd, false);

  servo_signal = map(spd, MIN_SPEED, MAX_SPEED, 0, 179);
  srv.write(servo_signal);

  motor_signal = map(spd, MIN_SPEED, MAX_SPEED, MIN_SIGNAL, MAX_SIGNAL);
  motor.writeMicroseconds(motor_signal);    

  yo("motor_signal", motor_signal, false);
  Serial.println();
  delay(10);                           
}



