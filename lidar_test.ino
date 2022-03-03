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
//const char* MQtopic = "louis_lidar2";       //MQTT Topic for Arena_2 (EAS011 - North)
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
  
  // Display Setup
  display.init();
  display.flipScreenVertically();
  display.setFont(Arial_MT_Plain_16);
  display.drawString(0, 0, "Initializing");
  display.display();
}

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
}
