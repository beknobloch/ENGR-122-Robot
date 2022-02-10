// TS: 08/15/2018    
// UP: 01/27/2021
// UP: 01/07/2022 - Speeds made faster
// AU: Joseph S. Miles
// DS: Basic Robot Motor Dance
// DS: Sketch saved under /Documents/Arduino on Office Laptop
// DS: Open a basic Arduino Sketch and copy and Paste in and compile and then upload

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

//Prototype the function names
void forward();   //Subroutine to have the robot move forward
void reverse();   //Subroutine to have the robot move backwards
void right();     //Subroutine to have the robot turn right
void left();      //Subroutine to have the robot turn left
void halt();      //Subroutine to have the robot stop


void setup() 
{
  // put your setup code here, to run once:
  // tells the ESP8266 to set up the pins to act as servos
motor1.attach(motor1pin);  // D0 will be a servo motor pin – Driver Side
motor2.attach(motor2pin);  // D2 will be a servo motor pin – Passenger Side
motor1.write(90); // Turns motor 1 off
motor2.write(90); // Turns motor 2 off

}


void loop() // All code in loop runs continuously after setup
{
  // put your main code here, to run repeatedly:
halt();         // have robot in stop condition for 5 seconds until you get it positioned
delay(3000);


forward();      // move forward for 2 seconds
delay(3000);


halt();         // stop for 5 seconds
delay(1000);

reverse();      // move backward for 2 seconds
delay(3000);


left();         // spin left for 2 seconds
delay(2000);


right();          // spin right for 2 seconds
delay (2000);
}


// Subroutine listings
void forward()  // Has the motors go forward- even for 2020
  {
   motor1.write(70); // Driver motor D0 - furthest from 90 goes faster. 60 is faster than 80
   motor2.write(65); // Passenger motor D2 
   }
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void reverse()  // Has the motors go in reverse - even for 2020
  {
    motor1.write(110);  //left  motor D0 - furthest from 90 goes faster. 120 is faster than 100
    motor2.write(115);  //right motor D2 - 100 is slow but good for testing; 100 is much slower than 80 is fast
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
