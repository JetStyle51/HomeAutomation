/*
  ArduinoMqttClient - WiFi Simple Sender

  This example connects to a MQTT broker and publishes a message to
  a topic once a second.

  The circuit:
  - Arduino MKR 1000, MKR 1010 or Uno WiFi Rev.2 board

  This example code is in the public domain.
*/

/*

 *************************************************************
  This example runs directly on ESP32 chip.

  Note: This requires ESP32 support package:
    https://github.com/espressif/arduino-esp32

  Please be sure to select the right ESP32 module
  in the Tools -> Board menu!

  Change WiFi ssid, pass, and Blynk auth token to run :)
  Feel free to apply it to any other example. It's simple!
 *************************************************************/

#include <DYIRDaikin.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoMqttClient.h>
#include "arduino_secrets.h"

#define DYIRDAIKIN_SOFT_IR
#define POWER_LED 27


///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

// To connect with SSL/TLS:
// 1) Change WiFiClient to WiFiSSLClient.
// 2) Change port value from 1883 to 8883.
// 3) Change broker value to a server with a known SSL/TLS root certificate 
//    flashed in the WiFi module.

DYIRDaikin irdaikin;  //Create an DYIRDaikin object
int power = 0;  //Power off
int swing = 1;  //Swing On
int fanSpeed = 1; //2/5
int nbMode = 1; //FAN mode (1-1=0 in the tab)
int temp = 25;  //Temp at 25degree

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "192.168.1.17";
int        port     = 1883;
const char topic[]  = "clim_salon";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(500);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  // You can provide a unique client ID, if not set the library uses Arduino-millis()
  // Each client must have a unique client ID
  // mqttClient.setId("clientId");

  // You can provide a username and password for authentication
  // mqttClient.setUsernamePassword("username", "password");

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  #ifdef DYIRDAIKIN_SOFT_IR
	irdaikin.begin(4);
	#else
	irdaikin.begin();
	#endif
  power = 0;
}

void powerON()
{
irdaikin.on();
irdaikin.setSwing_off();
irdaikin.setMode(nbMode);
irdaikin.setFan(fanSpeed);//FAN speed to MAX = 4; 5= AUTO; 6= SILENT
irdaikin.setTemp(temp);
//----everything is ok and to execute send command-----
irdaikin.sendCommand();
}

//Power ON or OFF
void daikinPower(int power=0) {
    if (power == 0){
        power = 1;
        powerON();
				Serial.println("Turn On");
			}else{
				irdaikin.off();
				Serial.println("Turn Off");
			}
			irdaikin.sendCommand();
			Serial.println("Execute Command!");
      delay(50); // Wait a bit between retransmissions
  }

  //SWING
  void daikinSwing(int swing = 0) {
    if (swing == 1) 
    {
      irdaikin.setSwing_on();
      Serial.println("Swing on");
    } else
      {
        irdaikin.setSwing_off();
        Serial.println("Swing off");
      }
      irdaikin.sendCommand();
      delay(50); // Wait a bit between retransmissions
  }

  //TEMPERATURE
  void change_temp(int temp)  {
    irdaikin.setTemp(temp);
    Serial.println("Temp changed");
    irdaikin.sendCommand();
    delay(50); // Wait a bit between retransmissions
  }

  //FAN SPEED
  void daikinFanSpeed (int speed)  {
    fanSpeed = speed;
    irdaikin.setFan(fanSpeed);
    Serial.println("FAN speed changed");
    irdaikin.sendCommand();
    delay(50); // Wait a bit between retransmissions
  }

//MODE
void daikinMode(int mode)  {
    switch (nbMode) {
      case 1: { //FAN
        nbMode = 0;
        Serial.println("FAN Mode");
        break;
      }
      case 2: { //COOL
        nbMode = 1;
        Serial.println("COOL Mode");
        break;
      }
      case 3: { //DRY
        nbMode = 2;
        Serial.println("DRY Mode");
        break;
      }
      case 4: { //HEAT
        nbMode = 3;
        Serial.println("HEAT Mode");
        break;
      }
      default:
      Serial.println("Selected mode out of the range");
    }
    irdaikin.setMode(nbMode);
    irdaikin.sendCommand();
    delay(50); // Wait a bit between retransmissions
  }

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  mqttClient.poll();

  // avoid having delays in loop, we'll use the strategy from BlinkWithoutDelay
  // see: File -> Examples -> 02.Digital -> BlinkWithoutDelay for more info
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;
  /*
    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.print("hello ");
    Serial.println(count);
    
    // send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print("hello ");
    mqttClient.print(count);
    mqttClient.endMessage();

    Serial.println();

    count++;


  */

  mqttClient.subscribe(topic);

  Serial.print("Waiting for messages on topic: ");
  Serial.println(topic);
  Serial.println();
  }


}
