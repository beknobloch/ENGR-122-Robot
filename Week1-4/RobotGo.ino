#include <Ultrasonic.h>
#include <Servo.h>


#define motor1pin D0   
#define motor2pin D2        

Servo motor1;  // Creates a servo object called "motor1"
Servo motor2;  // Creates a servo object called "motor2"

Ultrasonic ultrasonic_driver(D8, D5);  // An ultrasonic sensor HC-04
Ultrasonic ultrasonic_center(D9, D6);  // An ultrasonic sensor HC-04
Ultrasonic ultrasonic_pass(D10, D7);  // An ultrasonic sensor HC-04

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  motor1.attach(motor1pin);  // D0 will be a servo motor pin – Driver Side
  motor2.attach(motor2pin);  // D2 will be a servo motor pin – Passenger Side
  motor1.write(90); // Turns motor 1 off
  motor2.write(90); // Turns motor 2 off
  

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


    motor1.write(0);
    motor2.write(0);
   
}
