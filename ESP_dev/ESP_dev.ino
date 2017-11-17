#include <ESP8266WiFi.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <MQTT.h>
#include <PubSubClient.h>
#include <PubSubClient_JSON.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <EEPROM.h>

#include "MQTT_ESP.h"
#include "switch_initialize.h"

const char *ssidcon =  "wifi_ssid";
const char *passcon =  "wifi_pass";

#define BUFFER_SIZE 100
#define relay_pin     13
#define wifi_status   16
#define mqtt_status   14

IPAddress mqtt_server(0, 0, 0, 0);   //mqtt server ip
WiFiClient espClient;
PubSubClient client(espClient);

String subscribe_topic = "Light";
String publish_topic = "Light";


long lastDebounceTime = 0;  //the last time the output pin was toggled
long debounceDelay = 50;    //the debounce time: increase if the output flickersint ledState = HIGH;        //current state of the output pin
int ledState = LOW;        //current state of the output pin
int lastButtonState = LOW;  //previous reading from the input pin
int buttonState;            //current reading from the input pin
String on_off_stat;
bool change_flag = false;


void setup() {
  delay(250);
  
  pinMode(relay_pin, OUTPUT);
  pinMode(wifi_status,OUTPUT);
  pinMode(mqtt_status,OUTPUT);  
  digitalWrite(relay_pin, HIGH);    
  digitalWrite(wifi_status,HIGH);
  digitalWrite(mqtt_status,HIGH);
  
  Serial.begin(115200); 
  wifi_init();
  client.set_server(mqtt_server, 1883);
  client.set_callback(callback);
  client.subscribe(subscribe_topic);

  delay(500);
}

void loop() {
  MQTT_connect();
  touch_check();
}

void touch_check() {
  int reading;
  if (digitalRead(5)) {
    reading = 1;
  } else {
    reading = 0;
  }

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 30) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == HIGH) {
        ledState = !ledState;
        digitalWrite(relay_pin, !ledState);        
        Serial.println(ledState);
        if (client.connected()) {
          pubMQTT();
          change_flag = false;
        }
        if (!client.connected()) {
          change_flag = true;
        }
        delay(200);
      }
    }
  }
  lastButtonState = reading;
  delay(5);
}

