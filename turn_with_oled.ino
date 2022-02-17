#include <Ultrasonic.h>
#include <Servo.h>
#include <Wire.h>
#include "SSD1306.h"

#define motor1pin D0   
#define motor2pin D2        

Servo motor1;  // Creates a servo object called "motor1"
Servo motor2;  // Creates a servo object called "motor2"

Ultrasonic ultrasonic_driver(D8, D5);  // An ultrasonic sensor HC-04
Ultrasonic ultrasonic_center(D9, D6);  // An ultrasonic sensor HC-04
Ultrasonic ultrasonic_pass(D10, D7);  // An ultrasonic sensor HC-04

SSD1306 display(0x3C, D14, D15);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  motor1.attach(motor1pin);  // D0 will be a servo motor pin – Driver Side
  motor2.attach(motor2pin);  // D2 will be a servo motor pin – Passenger Side
  motor1.write(90); // Turns motor 1 off
  motor2.write(90); // Turns motor 2 off

  // Display Setup
  display.init();
  display.flipScreenVertically();
  display.drawString(0, 0, "Stevens Smart Robot");
  display.display();
  
  int ultrasonic_value;
  int motor_conrol;
}

void loop() {
  // put your main code here, to run repeatedly:
    motor_control = -1;
    ultrasonic_value = -1;
    // OLED controls
    char temp1[50];
    char temp2[50];
    sprintf(temp1, "%d", motor_control);
    sprintf(temp2, "%d", ultrasonic_value);
    const char *c = temp1;
    display.drawString(0, 0, "Motor Control");
    display.drawString(0, 15, "Value: ");
    display.drawString(80, 16, motor_control);
    display.drawString(0, 30, "Distance: ");
    display.drawString(80, 31, ultrasonic_value);
    display.drawString(0, 45, "inches");

    
  
     Serial.print("Distance Driver(D8,D5) Center(D9,D6) Pass(D10,D7):   ");  // Print a caption
     ultrasonic_value = ultrasonic_driver.read(INC);
     Serial.print(ultrasonic_value);
     Serial.print(" in. ");
     Serial.print(ultrasonic_center.read(INC));
     Serial.print(" in. ");
     Serial.print(ultrasonic_pass.read(INC));
     Serial.println(" in. ");

    
    
    motor1.write(0);
    motor2.write(0);
    motor_control = 0;
   
  if (ultrasonic_driver.read(INC) < 2.75 || ultrasonic_center.read(INC) < 2.75 || ultrasonic_pass.read(INC) < 2.75) {
    motor1.write(90);
    motor2.write(90);
    motor_control = 90;
    delay(500);
    motor1.write(90);
    motor2.write(0);
    delay(1100);
    motor1.write(90);
    motor2.write(90);
    delay(250);
    motor1.write(90);
    motor2.write(90);
  }
}
