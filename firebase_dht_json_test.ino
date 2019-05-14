// https://github.com/FirebaseExtended/firebase-arduino
// ArduinoJson by Benoit Blanchon Version 5.13.5
// DHT sensor library by Adafruit Version 1.3.4
// Adafruit Unified Sensor by Adafruit Version 1.0.3
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include "secrets.h"
#include "DHT.h"

#define DHTPIN D5
#define DHTTYPE DHT22

#define RATE 5*60
#define PATH "/sensor1"

DHT dht(DHTPIN, DHTTYPE);
DynamicJsonBuffer jsonBuffer(200);
JsonObject& obj = jsonBuffer.createObject();
JsonObject& timestamp = obj.createNestedObject("timestamp");

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT22 test!"));

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("connecting"));
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print(F("connected: "));
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C  Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));

  // get value 
  //Serial.print("last values: ");
  //Serial.println(Firebase.get(PATH));
  
  obj["temperature"] = t;
  obj["humidity"] = h;
  timestamp[".sv"] = "timestamp";
  obj.printTo(Serial); Serial.println();
  Firebase.push(PATH, obj);
  if (Firebase.failed()) {
      Serial.print(F("Pushing data failed: "));
      Serial.println(Firebase.error());
      Serial.println(F("Trying again"));
      Firebase.push(PATH, obj);
      if (Firebase.failed()) {
        Serial.print(F("Pushing data failed again: "));
        Serial.println(Firebase.error());
      } else Serial.println(F("Pushing data second time: OK"));
  } else Serial.println(F("Pushing data: OK"));

  delay(RATE*1000);
 
}
