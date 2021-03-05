#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#define ISS_overhead    D5
#define Entry           D6
#define Exit            D7


#define WLAN_SSID       "SSID"             // Your SSID
#define WLAN_PASS       "PASSWORD"        // Your password

//=================================================
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "YOUR ADAFRUIT.IO USERNAME" // Replace it with your Adafruit username
#define AIO_KEY         "YOUR ADAFRUIT.IO KEY"   // Replace with your AdafrProject Auth Key

//==================================================

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


//===========================Feeds=====================


//Setting up fields
Adafruit_MQTT_Subscribe ISS_Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/ISS_overhead"); // FeedNames
Adafruit_MQTT_Subscribe Entry_Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Entry");
Adafruit_MQTT_Subscribe Exit_Light = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Exit");

//=====================================================

void MQTT_connect();

//===================================================
void setup() {
  Serial.begin(115200);

  pinMode(ISS_overhead, OUTPUT);
  pinMode(Entry, OUTPUT);
  pinMode(Exit, OUTPUT);
  
  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
 

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&ISS_Light);
  mqtt.subscribe(&Entry_Light);
  mqtt.subscribe(&Exit_Light);
}

//=========================================================


void loop() {
  mqtt.ping();//This prevents from the NodeMCU to become inactive after some time
  MQTT_connect();
  

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &ISS_Light) {
      Serial.print(F("ISS overhead light: "));
      Serial.println((char *)ISS_Light.lastread);
      int ISS_Light_State = atoi((char *)ISS_Light.lastread);
      digitalWrite(ISS_overhead,HIGH);
      delay(20000);//Light up for 20 seconds
      digitalWrite(ISS_overhead,LOW);
      
    }
    if (subscription == &Entry_Light) {
      Serial.print(F("Astronauts Entry Light: "));
      Serial.println((char *)Entry_Light.lastread);
      //int Entry_Light_State = atoi((char *)Entry_Light.lastread);
      digitalWrite(Entry,HIGH);
      delay(20000);
      digitalWrite(Entry,LOW);
    }
    if (subscription == &Exit_Light) {
      Serial.print(F("Astronauts Exit Light: "));
      Serial.println((char *)Exit_Light.lastread);
      //int Exit_Light_State = atoi((char *)Exit_Light.lastread);
      digitalWrite(Exit,HIGH);
      delay(20000);
      digitalWrite(Exit,LOW);}
  }

  
}




//===========================================================


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  
}
