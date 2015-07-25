// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#define MIN_SIGNAL 700
#define MAX_SIGNAL 2000
#define SIGNAL_MARGIN 50
#define MIN_SPEED 0
#define MAX_SPEED 10000
#define MIN_ACC_STEP 5
#define MAX_ACC_STEP 100
#define MIN_NTRL_STEP 0
#define MAX_NTRL_STEP -100
#define MIN_BRK_STEP -10
#define MAX_BRK_STEP -2000

#define LED_PIN 9
#define MOTOR_PIN 3
#define ACC_POT_PIN A2
#define NTRL_POT_PIN A1
#define BRK_POT_PIN A0

#define SERVO_PIN_LOW 8
#define SERVO_PIN_HIGH 7
#define SERVO_PIN_SIGNAL 6

#include <Servo.h> 
#include <Wire.h>
#include "WiiChuck.h"

Servo gauge;
Servo motor; 
WiiChuck chuck = WiiChuck();

int acc_step = 10;
int ntrl_step = -10;
int brk_step = -100;

int acc = 0; //acceleration 
int spd = 0; //speed 
int motor_signal; //the signal to pass to the motor
int led_signal; //the signal to pass to the led

boolean rdy = false;

boolean calibrate = false;
boolean program_mode = true;

void setup() 
{ 
  Serial.begin(9600);
  
//  pinMode(SERVO_PIN_LOW, OUTPUT);
//  digitalWrite(SERVO_PIN_LOW, LOW);
//  pinMode(SERVO_PIN_HIGH, OUTPUT);
//  digitalWrite(SERVO_PIN_HIGH, HIGH);
//  gauge.attach(SERVO_PIN_SIGNAL);
//  gauge.write(90);    

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("Program begin...");

  motor.attach(MOTOR_PIN);
  
  if (calibrate) {
    Serial.println("This program will calibrate the ESC.");

    motor.writeMicroseconds(MAX_SIGNAL);    
    Serial.println("Now writing maximum output.");
  
    printWait(5000, 400);
  
    Serial.println("Sending minimum output");
    motor.writeMicroseconds(MIN_SIGNAL);
  
    printWait(5000, 400);
  } else if (program_mode) {
    Serial.println("This program will enter programing mode.");

    motor.writeMicroseconds(MAX_SIGNAL);    
    Serial.println("Now writing maximum output.");
  
    printWait(7000, 400);
   
  } else {
    motor.writeMicroseconds(MIN_SIGNAL);
  }
  
} 
 
void loop() 
{ 

  //wait for the remote control to connect
  while (!chuck.getStatus()){
    rdy = false;
    motor.writeMicroseconds(MIN_SIGNAL);
    chuck.begin();
    chuck.update();
    printWait(1000, 200);
    yo("Waiting for remote...", chuck.getStatus(), true);
  }

  //after first nunchuck sync - calibrate joystick and led
  if (!rdy){
    rdy = true;
    chuck.calibrateJoy();
    digitalWrite(LED_PIN, LOW);
    Serial.println("------------------------| READY!!! |---------------------------");  
  }  
  
  //read potentiometers and set steps
  acc_step = map(analogRead(ACC_POT_PIN), 1024, 0, MIN_ACC_STEP, MAX_ACC_STEP);
  yo("acc_step", acc_step, false);

  ntrl_step = map(analogRead(NTRL_POT_PIN), 1024, 0, MIN_NTRL_STEP, MAX_NTRL_STEP);
  yo("ntrl_step", ntrl_step, false);

  brk_step = map(analogRead(BRK_POT_PIN), 1024, 0, MIN_BRK_STEP, MAX_BRK_STEP);
  yo("brk_step", brk_step, false);

  //read the nunchuck signal and set the speed
  chuck.update();

  if (program_mode) {
    
    if (chuck.cActive() && chuck.zActive()){
      motor_signal = ((MIN_SIGNAL + MAX_SIGNAL) / 2);
    }else if (chuck.cActive()){
      motor_signal = (MIN_SIGNAL);
    }else if (chuck.zActive()){
      motor_signal = (MAX_SIGNAL);
    }    

    motor.writeMicroseconds(motor_signal);    
    yo("motor_signal", motor_signal, false);

  } else {
  
    if (chuck.cActive()){
      //BRAKE button
      spd += brk_step;
    }else if (chuck.zActive()){
      //ACCELARATION button is pressed - use joystick to accelarate
      acc = (chuck.readJoyY());
      acc = map(acc, -128, 128, -1*acc_step, acc_step);
      spd += acc;
    } else {
      //No button is pressed - decend slowly
      spd += ntrl_step;
    }    
   
    // make sure the speed is within the min/max limits
    spd = max(MIN_SPEED, spd);
    spd = min(spd, MAX_SPEED); 
  
    yo("spd", spd, false);
  
    //signal the speed with the intensity of the led
    led_signal = map(spd, MIN_SPEED, MAX_SPEED, 0, 255);
    yo("led_signal", led_signal, false);
    analogWrite(LED_PIN, led_signal);
  
    //set the signal for the motor
    motor_signal = map(spd, MIN_SPEED, MAX_SPEED, (MIN_SIGNAL+SIGNAL_MARGIN), (MAX_SIGNAL-SIGNAL_MARGIN));
    motor.writeMicroseconds(motor_signal);    
    yo("motor_signal", motor_signal, false);
  
}
  
  Serial.println();
  delay(5);                           
}



