#include <Servo.h>

Servo myservo; 

#define ldr1 A1
#define ldr2 A2

int pos = 90; // initial position of the Horizontal movement controlling servo motor
int tolerance = 20; // allowable tolerance setting - so solar servo motor isn't constantly in motion

void setup() {
  myservo.attach(2); // attaches the servo on digital pin 2 to the horizontal movement servo motor
  pinMode(ldr1, INPUT); 
  pinMode(ldr2, INPUT); 
  myservo.write(pos); 

  delay(500); 
}

void loop() {
  int val1 = analogRead(ldr1); 
  int val2 = analogRead(ldr2); 

  if ((abs(val1 - val2) <= tolerance) || (abs(val2 - val1) <= tolerance)) {
    //no servo motor horizontal movement will take place if the ldr value is within the allowable tolerance
  } else {
    if (val1 > val2) 
    {
      pos = pos + 1; 
    }
    if (val1 < val2) 
    {
      pos = pos - 1; 
    }
  }

  if (pos > 180) { 
    pos = 180; 
  }
  if (pos < 0) {
    pos = 0; 
  }
  myservo.write(pos); 
  delay(50);

}
