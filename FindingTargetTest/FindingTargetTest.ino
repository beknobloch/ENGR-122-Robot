/*------------------------------------------------------------------------------------------
 * Final Target-to-Target Program
 * ENGR 122 - Group 4
 * Professor Miles
 * 
 *  
 * 
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
const char* MQtopic = "louis_lidar2";         //MQTT Topic for Arena_1 (EAS011 - North)
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
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Initializing");
  display.display();

  delay(3000);
}

double points_to_angle_value(int x_one, int y_one, int x_two, int y_two, int x_target, int y_target)
{
  double angle_to_return, u_1, u_2, v_1, v_2;
  u_1 = x_two - x_one;
  u_2 = y_two - y_one;
  v_1 = x_target - x_two;
  v_2 = y_target - y_two;
  
  double dot = (u_1 * v_1) + (u_2 * v_2);
  double ab = sqrt(pow(u_1, 2) + pow(u_2, 2)) * sqrt(pow(v_1, 2) + pow(v_2, 2));
  
  if (ab != 0) angle_to_return = acos(dot / ab);
  else angle_to_return = 0;

  // Implement direction
  if ((u_1 * v_1) > (u_2 * v_2))  angle_to_return *= -1;

  return angle_to_return;
}

void turn_with_angle(double angle) {

  // Add code for converting an angle into appropriate motor values. If < 0, turn left. If > 0, turn right.
 
  double angle_to_turn_coefficient = 0.15;
  
  if (angle < 0)
  {
    Serial.println("I'm turning left.");
    motor1.write(120);
    motor2.write(0);
    angle *= -1;
  } else {
    Serial.println("I'm turning right.");
    motor1.write(0);
    motor2.write(120);
  }

  Serial.println("Turning with calculated delay.");
  delay(angle_to_turn_coefficient * angle);
  motor1.write(90);
  motor2.write(90);

}

bool coords_overlap(int x_one, int y_one, int x_two, int y_two, int tolerance)
{
 return (sqrt(pow(x_two - x_one, 2) + pow(y_two - y_one, 2)) < tolerance);
}

// Define logical variables.

int arrival_tolerance = 10;

int recorded_x = 100;
int recorded_y = 650;

int loop_iteration = 0;

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
  delay(1000);
  delimIndex = payload.indexOf(']');
  testCollector[count++] = payload.substring(prevIndex+1, delimIndex).toInt();
   
  int x, y, tar_x, tar_y; 
  //Robot location x,y from MQTT subscription variable testCollector 
  x = testCollector[0];
  y = testCollector[1];

  Serial.println("Read the location on the screen.");
  Serial.print("Is this right?:  ");
  Serial.print(x);
  Serial.print(", ");
  Serial.println(y);
  
  tar_x = 800;
  tar_y = 600;

  // OLED controls
  char temp1[50];
  char temp2[50];
  char temp3[50];
  char temp4[50];
  sprintf(temp1, "%d", x);
  sprintf(temp2, "%d", y);
  sprintf(temp3, "%d", tar_x);
  sprintf(temp4, "%d", tar_y);
  const char *c = temp1;
  const char *d = temp2;
  const char *e = temp3;
  const char *f = temp4;
  display.clear();
  display.drawString(0, 0, "x: ");
  display.drawString(20, 0, c);
  display.drawString(40, 0, "y: ");
  display.drawString(60, 0, d);
  display.drawString(0, 16, "Heading to:");
  display.drawString(0, 32, e);
  display.drawString(40, 32, f);
  display.display();
  
  // Serial monitor controls
  Serial.print("x-coordinate: ");
  Serial.println(c);
  Serial.print("y-coordinate: ");
  Serial.println(d);
  Serial.println("\n");

  Serial.println("\nANGLE TEST 1: ");
  Serial.println(points_to_angle_value(0, 0, 500, 500, 500, 0));
  Serial.println("\nANGLE TEST 2: ");
  Serial.println(points_to_angle_value(0, 0, 500, 500, 0, 500));

  switch (loop_iteration)
  {
    case 0:
      // Check if arrived at target. If so, stop for a long time.
      if (coords_overlap(x, y, tar_x, tar_y, arrival_tolerance))
      {
        Serial.println("We're here!");
        delay(100000);
      }
    
      // Record location and drive forward briefly.
      recorded_x = x;
      recorded_y = y;

      Serial.print("Recording current position: ");
      Serial.print(x);
      Serial.print(", ");
      Serial.println(y);
      
      motor1.write(0);
      motor2.write(0);

      delay(1000);

      motor1.write(90);
      motor2.write(90);

      Serial.print("Recorded coordinates: ");
      Serial.print(recorded_x);
      Serial.print(", ");
      Serial.println(recorded_y);
      
      break;
    case 1:
      // Use new position to turn to target and drive there.

      Serial.print("\nCurrent coordinates: ");
      Serial.print(x);
      Serial.print(", ");
      Serial.println(y);

      Serial.println("Turning to where I think the target is.");
      
      double angle = points_to_angle_value(recorded_x, recorded_y, x, y, tar_x, tar_y);
      Serial.print("Calculate angle value in radians: ");
      Serial.println(angle);
      turn_with_angle(angle);

      break;
  }

  loop_iteration++;
  loop_iteration = loop_iteration % 2;
}
