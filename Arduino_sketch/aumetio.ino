#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
#include <GP2Y1010AU0F.h>

#define DHTPIN 4
#define DHTTYPE DHT11   // DHT 11

int measurePin = 35;   // Connect dust sensor analog measure pin to Arduino A0 pin
int ledPin     = 27;    // Connect dust sensor LED pin to Arduino pin 2


float dustDensity = 0;
GP2Y1010AU0F dustSensor(ledPin, measurePin); // Construct dust sensor global object
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "DIR-300A-bf05";
const char* password = "11411253";
const char* serverName = "https://aumetio-default-rtdb.firebaseio.com/data/.json";


unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(9600);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
   
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  dht.begin();
  dustSensor.begin();
}

void loop() {
   StaticJsonDocument<100> jsonDocument;
   
  if ((millis() - lastTime) > timerDelay) {
    dustDensity = dustSensor.read();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    jsonDocument["Temperature"] = t;
    jsonDocument["Humidity"] = h;
    jsonDocument["AirQuality"]=dustDensity;
    char buffer[100];
    serializeJsonPretty(jsonDocument, buffer);
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
 
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(buffer);
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
