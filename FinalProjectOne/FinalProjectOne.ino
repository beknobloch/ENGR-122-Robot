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
const char* MQtopic = "louis_lidar2";         //MQTT Topic for Arena_1 (EAS011 - South)
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

void oled_debug(char * statement, char * statement_one)
{
  // OLED controls
  display.clear();
  display.drawString(0, 0, "DEBUG:");
  display.drawString(0, 16, statement);
  display.drawString(0, 32, statement_one);
  display.display();
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
  
  if ((x_one == x_two && y_one == y_two) || ab == 0)  angle_to_return = 0;
  else angle_to_return = acos(dot / ab);

  // Implement direction
  if ((u_1 * v_1) < (u_2 * v_2))  angle_to_return *= -1;

  return angle_to_return * (180/M_PI);
}

int turn_with_angle(double angle) {

  // Add code for converting an angle into appropriate motor values. If < 0, turn left. If > 0, turn right.
 
  double angle_to_turn_coefficient = 6;

  if (angle == 0)
  {
    oled_debug("ANGLE APPEARS", "TO BE 0");
    motor1.write(70);
    motor2.write(70);
  }
  else if (angle < 0)
  {
    Serial.println("I'm turning left.");
    oled_debug("ANGLE TURN", "LEFT LEFT LEFT");
    motor1.write(120);
    motor2.write(70);
    angle *= -1;
  } else {
    Serial.println("I'm turning right.");
    oled_debug("ANGLE TURN", "RIGHT RIGHT RIGHT");
    motor1.write(70);
    motor2.write(120);
  }

  Serial.println("Turning with calculated delay.");

  delay(angle_to_turn_coefficient * angle);

  motor1.write(90);
  motor2.write(90);

  return int(angle_to_turn_coefficient * angle);

}

bool coords_overlap(int x_one, int y_one, int x_two, int y_two, int tolerance)
{
 return (sqrt(pow(x_two - x_one, 2) + pow(y_two - y_one, 2)) < tolerance);
}

// Define logical variables.

int target_coords [8] = {1400, 150, 650, 150, 150, 150, 2000, 700};
int current_target = 0;

int arrival_tolerance = 80;
int proximity_tolerance = 2;
bool reorient = false;

int motor_control = 0; // 0 means no movement. -1 and 1 mean turn left and turn right respectively. -2 means turn around, 2 means forward.
int recorded_x = 100;
int recorded_y = 650;

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
   
  int x, y, tar_x, tar_y; 
  //Robot location x,y from MQTT subscription variable testCollector 
  x = testCollector[0];
  y = testCollector[1];
  tar_x = target_coords[current_target * 2];
  tar_y = target_coords[current_target * 2 + 1];


  // ALGORITHM

  double angle = points_to_angle_value(recorded_x, recorded_y, x, y, tar_x, tar_y);

  if (coords_overlap(x, y, tar_x, tar_y, arrival_tolerance))
  {
    current_target++;
    if (current_target == 4)
    {
      // END!
      delay(100000000);
      oled_debug("FINISH!", "VICTORY!");
    }
    motor1.write(90);
    motor2.write(90);
    oled_debug("ARRIVAL!", "NEXT TARGET!");
    delay(1500);
    tar_x = target_coords[current_target * 2];
    tar_y = target_coords[current_target * 2 + 1];
    turn_with_angle(angle);
  }
  double center = ultrasonic_center.read(INC);
  double pass = ultrasonic_pass.read(INC);
  double driver = ultrasonic_driver.read(INC);

  if (reorient)
  {
      turn_with_angle(angle);
      reorient = false;
  }
  else
  {
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
  }
  
  if (center < proximity_tolerance)
  {
      if (driver > pass)                                                    // Left.
      {
          motor_control = -1;
      }
      else                                                                  // Right.
      {
          motor_control = 1;
      }
  }
  else if (pass < proximity_tolerance || driver < proximity_tolerance)
  {
      if (!(pass < proximity_tolerance))                                    // Slight right turn.
      {
          motor1.write(70);
          motor2.write(120);
          delay(100);
      }
      else if (!(driver < proximity_tolerance))                             // Slight left turn.
      {
          motor1.write(120);
          motor2.write(70);
          delay(100);
      } else                                                                // U-turn
      {
          motor_control = -2;
      }
  }
  else                                                                      // Forward.
  {
      motor_control = 2;
      
  }

  // Motor control
  switch (motor_control)
  {
    case -2:
      motor1.write(120);
      motor2.write(70);
      delay(800);
      break;
    case -1:
      motor1.write(120);
      motor2.write(70);
      delay(400);
      break;
    case 0:
      motor1.write(90);
      motor2.write(90);
      delay(10);
      break;
    case 1:
      motor1.write(70);
      motor2.write(120);
      delay(400);
      break;
    case 2:
      recorded_x = x;
      recorded_y = y;
      motor1.write(70);
      motor2.write(70);
      delay(100);
      reorient = true;
      break;
  }

}
