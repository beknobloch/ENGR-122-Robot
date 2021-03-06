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

int motor_control;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  motor1.attach(D2);  // D0 will be a servo motor pin – Driver Side
  motor2.attach(D0);  // D2 will be a servo motor pin – Passenger Side
  motor1.write(90); // Turns motor 1 off
  motor2.write(90); // Turns motor 2 off
  
  // Display Setup
  display.init();
  display.flipScreenVertically();
  display.setFont(Arial_MT_Plain_16);
  display.drawString(0, 0, "Initializing");
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:
     Serial.print("Distance Driver(D8,D5) Center(D9,D6) Pass(D10,D7):   ");  // Print a caption
     Serial.print(ultrasonic_driver.read(INC));
     Serial.print(" in. ");
     Serial.print(ultrasonic_center.read(INC));
     Serial.print(" in. ");
     Serial.print(ultrasonic_pass.read(INC));
     Serial.println(" in. ");
  
  // put your main code here, to run repeatedly:
    motor_control = 0;
  
  // OLED controls
    char temp1[50];
    char temp2[50];
    sprintf(temp1, "%d", motor_control);
    sprintf(temp2, "%d", ultrasonic_center.read(INC));
    const char *c = temp1;
    const char *d = temp2;
    display.drawString(0, 0, "Motor Control");
    display.drawString(0, 15, "Value: ");
    display.drawString(80, 16, c);
    display.drawString(0, 30, "Distance: ");
    display.drawString(80, 31, d);
    display.drawString(0, 45, "inches");


    motor1.write(0);
    motor2.write(0);
    
  if (ultrasonic_pass.read(INC) < 5.5) {
    motor1.write(65);
    motor2.write(0);
    
    motor_control = 65;
    
    delay(30);
  }
  if (ultrasonic_pass.read(INC) > 9) {
    motor1.write(0);
    motor2.write(65);
    
    motor_control = -65;
    
    delay(20);
} 
  
  if (ultrasonic_center.read(INC) < 6) {
    motor1.write(90);
    motor2.write(90);
    motor_control = 90;
    delay(10);
    motor1.write(120);
    motor2.write(0);
    motor_control = 120;
    delay(400);
    motor1.write(90);
    motor2.write(90);
    motor_control = 90;
    delay(10);
  }
  

}
