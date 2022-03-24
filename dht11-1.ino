//Here we go with the code:
#include <ESP8266WiFi.h>;  
#include<WiFiClient.h>;
#define BLYNK_PRINT Serial // Enables Serial Monitor
#include <BlynkSimpleEsp8266.h> //For Blynk interface
#include <ThingSpeak.h>;   //For thingspeak interface
#include "DHT.h"         

const char* ssid = "wifi name";
const char* pass = "type your wifi password";

unsigned long myChannelNumber = 1430786;   //Thingspeak channel number
const char * myWriteAPIKey = "XOCER4ZMO34ty57v";   //Thingspeak API key

#define DHTPIN 4  // Digital pin connected to the DHT sensor D2
#define DHTTYPE DHT11

char auth[] = "1mKYOsvaY-C3cc0gSZAex_RIs5At_9eC"; //Blynk authorisation token

DHT dht(DHTPIN, DHTTYPE); 

#define FAN_PIN 12 // D6
WidgetLED FAN(V4);       //Sending the status of fan that the fan is on or off

WiFiClient client;

void setup()
{
    Blynk.begin(auth, ssid, pass);
    Serial.begin(9600);
    pinMode(FAN_PIN, OUTPUT);
    Serial.println("Speed Depends on Temperature");
    dht.begin();
    ThingSpeak.begin(client);
}

void loop()
{
  Blynk.run();
 
  float h = dht.readHumidity();       // Read humidity (the default)
  float t = dht.readTemperature();    // Read temperature as Celsius (the default)
  
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  //Thingspeak field number to send data on it
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
  ThingSpeak.writeField(myChannelNumber, 3, hic, myWriteAPIKey);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.println(" ");

  //When temperature greater than 33 fan rotates with its full speed
  if (t > 33) {
    digitalWrite(FAN_PIN,HIGH);
    analogWrite(FAN_PIN, 255);
    FAN.on();
  }
  //When temperature lies between than 31 and 33 fan rotates with its half speed
  else if (t >= 31 && t <= 33) {
    digitalWrite(FAN_PIN,HIGH);
    analogWrite(FAN_PIN, 255 / 2); //speed reduces to 50%
    FAN.on();
  }
  //When temperature less than 31 fan doesnot rotates
  else if (t < 31) 
  {
    digitalWrite(FAN_PIN,LOW);
    FAN.off();
  }
  Blynk.virtualWrite(V1,t);   //sending temperature of environment to Blynk
  Blynk.virtualWrite(V2,h);   //sending humidity of environment to Blynk
  Blynk.virtualWrite(V3,hic); //sending Heat Index of environment to Blynk
  
  delay(500); 
}
