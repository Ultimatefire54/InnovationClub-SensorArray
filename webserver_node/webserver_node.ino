/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif


#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "Adafruit_SI1145.h"
#include <Digital_Light_TSL2561.h>

Adafruit_BMP280 bmp;
Adafruit_SI1145 sunlight = Adafruit_SI1145();

float lightIntensity = 0.0;

const char* ssid = "Innovation-TestHS";
const char* password = "networkPasswordEsp";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


String bmpTemperature(){
  float reading = bmp.readTemperature()*9/5 + 32;
  if(isnan(reading)){
    Serial.print("failed to take reading");
    return "";
  }
  Serial.println("took temperature reading");
  return String(bmp.readTemperature()*9/5 + 32);
}

String bmpPressure(){
  float reading = bmp.readPressure()/3386.39;
  if(isnan(reading)){
    Serial.print("failed to take reading");
    return "";
  }
  Serial.println("took pressure reading");
  return String(bmp.readPressure()/3386.39);
}

String bmpAltitude(){
  float reading = bmp.readAltitude(1025.5) * 3.28084;
  if(isnan(reading)){
    Serial.print("failed to take reading");
    return "";
  }
  Serial.println("took altitude reading");
  return String(bmp.readAltitude(1025.5) * 3.28084);
}

String readInfrared() {
  float reading = sunlight.readIR();
  if(isnan(reading)){
    Serial.print("failed to take reading");
    return "";
  }
  Serial.println("took IR reading");
  return String(sunlight.readIR());
}

String readVisible() {
  float reading = sunlight.readVisible();
  if(isnan(reading)){
    Serial.print("failed to take reading");
    return "";
  }
  Serial.println("took optical reading");
  return String(sunlight.readVisible());
}

String readUV() {
  float reading = sunlight.readUV()/100.0;
  if(isnan(reading)){
    Serial.print("failed to take reading");
    return "";
  }
  Serial.println("took uv reading");
  return String(sunlight.readUV()/100.0);
}

String readDigital(){
  float reading = TSL2561.readVisibleLux();
  Serial.println(reading);
  if(isnan(reading)){
    return "";
  }
  return String(TSL2561.readVisibleLux());
}

bool initSensors(){

  return (bmp.begin() * sunlight.begin()); 
}

void setup(){
  Wire.begin();
  Serial.begin(115200);

  //bmp.begin();
  //TSL2561.init();
  if (!initSensors()) {
    Serial.println("Check I2C wiring!");
    while (1);
  }

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  if(WiFi.status() == WL_CONNECTED)
    Serial.print("yes");
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", bmpTemperature().c_str());
  });
  server.on("/altitude", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", bmpAltitude().c_str());
  });
  server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", bmpPressure().c_str());
  });
  server.on("/digitalLight", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(lightIntensity).c_str());
  });

  server.on("/IR", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readInfrared().c_str());
  });

  server.on("/visible", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readVisible().c_str());
  });

  server.on("/UV", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readUV().c_str());
  });

  // Start server
  server.begin();

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}
 
void loop(){
//lightIntensity = TSL2561.readVisibleLux();
}
