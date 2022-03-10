/*------------------------------------------------------------------------------------------
 * Ver. 1 - 02/24/2022
 * This program is the sample code used for the 7th week lecture in the 2022 Spring semester. 
 * Week7: "Using LiDAR sensor information using MQTT protocol."
 * 
 * Based on this sample code, students should write additional commands to output robot position 
 * information using OLED Display and Serial Monitor.                      -SES IDEAs Program-
 -------------------------------------------------------------------------------------------*/
 
#include <ESP8266WiFi.h>  
#include "PubSubClient.h"
#include "WiFiManager.h" 
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
  
//MQTT Communication associated variables
char payload_global[100];                     
boolean flag_payload;                         

//MQTT Setting variables
const char* mqtt_server = "155.246.62.110";   //MQTT Broker(Server) Address
const char* MQusername = "jojo";              //MQTT username
const char* MQpassword = "hereboy";           //MQTT password
const char* MQtopic = "louis_lidar1";         //MQTT Topic for Arena_1 (EAS011 - South)
const int mqtt_port = 1883;                   //MQTT port#

//Stevens WiFi Setting variables
const char* ssid = "Stevens-IoT";             //Stevens Wi-Fi SSID (Service Set IDentifier)   
const char* password = "nMN882cmg7";          //Stevens Wi-Fi Password

//WiFi Define
WiFiClient espClient;                         
PubSubClient client(espClient);               
       
void setup_wifi() { 
  delay(10);
  // We start by connecting to a Stevens WiFi network
  WiFi.begin(ssid, password);           
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");                        
  }
  randomSeed(micros());                       
}

void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    payload_global[i] = (char)payload[i];
  }
  payload_global[length] = '\0';              
  flag_payload = true;                        
}

void reconnect() {                                                                
  // Loop until we're reconnected
  while (!client.connected()) {
    // Create a random client ID
    String clientId = "ESP8266Client-";       
    clientId += String(random(0xffff), HEX);  
    // Attempt to connect                     
    if (client.connect(clientId.c_str(),MQusername,MQpassword)) {
      client.subscribe(MQtopic);              //EAS011 South - "louis_lidar1"       EAS011 North - "louis_lidar2"
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();                               
  delay(3000);
  Serial.println("Wemos POWERING UP ......... ");
  client.setServer(mqtt_server, mqtt_port);   //This 1883 is a TCP/IP port number for MQTT 
  client.setCallback(callback); 

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

double pair_of_points_to_angle_value(int x_one, int y_one, int x_two, int y_two)  return atan2(y_two - y_one, x_two - x_one);

int angle_value_to_motor_values(double angle) {

  // Add code for converting an angle into appropriate motor values.

}

bool coords_overlap(int x_one, int y_one, int x_two, int y_two, int tolerance)
{
  if (abs(x_one - x_two) > tolerance) return false;
  if (abs(y_one - y_two) > tolerance) return false;
  return true;
}

// Define logical variables.

int [10] target_coords = {};
int current_target = 0;
int tracing = 0;

int arrival_tolerance = 0;
int proximity_tolerance = 4;

void loop() {
  //subscribe the data from MQTT server
  if (!client.connected()) {
    Serial.print("...");
    reconnect();
  }
  client.loop();                              
  
  String payload(payload_global);              
  int testCollector[10];                      
  int count = 0;
  int prevIndex, delimIndex;
    
  prevIndex = payload.indexOf('[');           
  while( (delimIndex = payload.indexOf(',', prevIndex +1) ) != -1){
    testCollector[count++] = payload.substring(prevIndex+1, delimIndex).toInt();
    prevIndex = delimIndex;
  }
  delimIndex = payload.indexOf(']');
  testCollector[count++] = payload.substring(prevIndex+1, delimIndex).toInt();
   
  int x, y; 
  //Robot location x,y from MQTT subscription variable testCollector 
  x = testCollector[0];
  y = testCollector[1];

  // OLED controls
  char temp1[50];
  char temp2[50];
  sprintf(temp1, "%d", x);
  sprintf(temp2, "%d", y);
  const char *c = temp1;
  const char *d = temp2;
  display.clear();
  display.drawString(0, 0, "x-coordinate");
  display.drawString(80, 0, c);
  display.drawString(0, 16, "y-coordinate");
  display.drawString(80, 16, d);
  display.drawString(0, 45, ":)");
  display.display();
  
  // Serial monitor controls
  Serial.print("x-coordinate: ");
  Serial.println(c);
  Serial.print("y-coordinate: ");
  Serial.println(d);
  Serial.println("\n");
  

  // PSEUDOCODE:
    // If on_current_target:
      // set target to the next target in the array.
      // Calculate and point robot towards next target.
      // tracing = 0;    <-- This indicates the robot is not "tracing" an obstacle.
    // If tracing == 0:
      // If the center sensor detects an obstacle:
        // Check which peripheral sensor reports a further distance.
        // Turn in that direction until the center sensor no longer detects an obstacle.
        // turned_left ? tracing = -1 : tracing = 1;    <-- Left corresponds to -1, right to 1;
      // Else:
        // Move forward.
    // Else:
      // Calculate the direction towards the next target.
      // If currently pointing in that direction:
        // tracing = 0;
      // Else:
        // continue to trace the obstacle depending on the value of tracing.

  // Real Code:

  int tar_x = target_coords[current_target * 2];
  int tar_y = target_coords[current_target * 2 + 1];

  int motor_control = 0; // 0 means no movement. -1 and 1 mean turn left and turn right respectively. -2 means turn around, 2 means forward.

  if (coords_overlap(x, y, tar_x, tar_y))
  {
    current_target++;
    // Calculate and point robot towards next target.
    tracing = 0;
  }
  if (tracing == 0)
  {
    double center = ultrasonic_center.read(INC);
    double pass = ultrasonic_pass.read(INC);
    double driver = ultrasonic_driver.read(INC);
    if (center < proximity_tolerance)
    {
      if (driver < proximity_tolerance)
      {
        if (pass < proximity_tolerance)
        {
          // Turn to the direction with the further distance.
          pass < driver ? motor_control = -1 : motor_control = 1;
          // Set tracing to appropriate value.
        } else
        {
          // Turn to the driver's side.
          motor_control = -1;
          tracing = -1;
        }
      } else if (pass < proximity_tolerance)
      {
        // Turn to the passenger's side.
        motor_control = 1;
        tracing = 1;
      } else
      {
        // Turn around.
        motor_control = -2;
      }
    } else  // No central obstacle detected.
    {
      // Move forward a little bit.
      motor_control = 2;
    }
  } else  // tracing != 0
  {
    
  }

  // Motor control
  switch (motor_control)
  {
    case -2:
      break;
    case -1:
      break;
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
  }

}
