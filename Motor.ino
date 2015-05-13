// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#define MIN_SIGNAL 900
#define MAX_SIGNAL 2000
#define MIN_SPEED 0
#define MAX_SPEED 10000
#define MIN_ACC_STEP 5
#define MAX_ACC_STEP 100
#define MIN_NTRL_STEP 0
#define MAX_NTRL_STEP -100
#define MIN_BRK_STEP -10
#define MAX_BRK_STEP -2000

#define LED_PIN 8
#define MOTOR_PIN 9
#define SERVO_PIN 6
#define ACC_POT_PIN A2
#define NTRL_POT_PIN A1
#define BRK_POT_PIN A0

#include <Servo.h> 
#include <Wire.h>
#include "WiiChuck.h"

Servo motor; 
Servo srv; 
WiiChuck chuck = WiiChuck();
 
int acc_step = 10;
int ntrl_step = -10;
int brk_step = -100;

int acc = 0; //acceleration 
int spd = 0; //speed 
int motor_signal; //the signal to pass to the motor
int servo_signal; //the signal to pass to the motor

boolean rdy = false;

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

  printWait(3000, 400);

  Serial.println("Sending minimum output");
  motor.writeMicroseconds(MIN_SIGNAL);

  printWait(3000, 400);
  
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

  if (!rdy){
    rdy = true;
    chuck.calibrateJoy();
    digitalWrite(LED_PIN, LOW);
    Serial.println("------------------------| READY!!! |---------------------------");  
  }  
  
  //read potentiometers and set steps
  acc_step = map(analogRead(ACC_POT_PIN), 0, 1024, MIN_ACC_STEP, MAX_ACC_STEP);
  ntrl_step = map(analogRead(NTRL_POT_PIN), 0, 1024, MIN_NTRL_STEP, MAX_NTRL_STEP);
  brk_step = map(analogRead(BRK_POT_PIN), 0, 1024, MIN_BRK_STEP, MAX_BRK_STEP);

  chuck.update();
  
  if (chuck.zActive()){
    acc = (chuck.readJoyY());
    acc = map(acc, -128, 128, -1*acc_step, acc_step);
  
    spd += acc;
  }else{
    spd += ntrl_step;
  }    
  spd = max(MIN_SPEED, spd);
  spd = min(spd, MAX_SPEED);
  
  if (chuck.cActive()) spd += brk_step; //BRAKE!

  yo("spd", spd, false);

  servo_signal = map(spd, MIN_SPEED, MAX_SPEED, 0, 179);
  srv.write(servo_signal);

  motor_signal = map(spd, MIN_SPEED, MAX_SPEED, MIN_SIGNAL, MAX_SIGNAL);
  motor.writeMicroseconds(motor_signal);    

  yo("motor_signal", motor_signal, false);
  Serial.println();
  delay(10);                           
}



