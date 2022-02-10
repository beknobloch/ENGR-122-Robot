#include <Ultrasonic.h>
#include <Dhcp.h>
#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

//#include <WemosInit.h>
#include <Servo.h>

//Change the pin definitions below-My 2019 & 2020 motor is D1 R1 -- Use pins D0 and D2 for motor for D1R1
#define motor1pin D0   //                       Driver        Side Motor  D0   
#define motor2pin D2   //                       Passenger Side Motor D2         

Servo motor1;  // Creates a servo object called "motor1"
Servo motor2;  // Creates a servo object called "motor2"

Ultrasonic ultrasonic_driver(D8, D5);  // An ultrasonic sensor HC-04
Ultrasonic ultrasonic_center(D9, D6);  // An ultrasonic sensor HC-04
Ultrasonic ultrasonic_pass(D10, D7);  // An ultrasonic sensor HC-04

//Prototype the function names
void forward();   //Subroutine to have the robot move forward
void reverse();   //Subroutine to have the robot move backwards
void right();     //Subroutine to have the robot turn right
void left();      //Subroutine to have the robot turn left
void halt();      //Subroutine to have the robot stop



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  motor1.attach(motor1pin);  // D0 will be a servo motor pin – Driver Side
  motor2.attach(motor2pin);  // D2 will be a servo motor pin – Passenger Side
  motor1.write(90); // Turns motor 1 off
  motor2.write(90); // Turns motor 2 off

}

void loop() {
  // put your main code here, to run repeatedly:

  forward();
  while (ultrasonic1.read(INC) > 3) {
    Serial.println("Distance from wall: " + ultrasonic_center.read(INC) + " inches");
  }
  halt();
  left();
  delay(2000);
  forward();
  while (ultrasonic1.read(INC) > 3) {
    Serial.println("Distance from wall: " + ultrasonic_center.read(INC) + " inches");
  }
  halt();
  left();
  delay(2000);

}


// Subroutine listings
void forward()  // Has the motors go forward- even for 2020
  {
   motor1.write(70); // Driver motor D0 - furthest from 90 goes faster. 60 is faster than 80
   motor2.write(63); // Passenger motor D2 
   }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void reverse()  // Has the motors go in reverse - even for 2020
  {
    motor1.write(110);  //left  motor D0 - furthest from 90 goes faster. 120 is faster than 100
    motor2.write(116);  //right motor D2 - 100 is slow but good for testing; 100 is much slower than 80 is fast
  }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void halt() // Has the motors stop
  {
    motor1.write(90); //Turns motor1 off
    motor2.write(90); //Turns motor2 off
  }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void left() // Has the motors turn gently left ---- furthest from 90 and a harder left
{
  motor1.write(110);
  motor2.write(70);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void right() // Has the motors turn left
{
  motor1.write(70);
  motor2.write(110);
}
