#include <Arduino.h>
#include <WiFi.h>
#include <iostream>
#include <sstream>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define STOP 0

#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

#define FOEWARD 1
#define BACKWARD -1

const int PWMFreq = 1000; //1KHz
const int PWMResolution =8; //8Bit
const int PWMSpeedChannel =4; //control channel

const char* ssid = "TAEHYUN";
const char* password = "20020205";

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

struct MOTOR_PINS {
  int pinEn;
  int pinIN1;
  int pinIN2;
};

std::vector<MOTOR_PINS> motorPins = {
  {22,16,17}, //Right Motor (EnA, IN1, IN2)
  {23,18,19}, //Left Motor (EnB, IN3, IN4)
};

void setUpPinModes() {
  ledcSetup(PWMSpeedChannel, PWMFreq, PWMResolution);
  for (int i=0; i<motorPins.size(); i++) {
    pinMode(motorPins[i].pinEn, OUTPUT);
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);

    ledcAttachPin(motorPins[i].pinEn, PWMSpeedChannel);
  }
}

void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "File Not Found.");
}

void setup() {
  // put your setup code here, to run once:
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAP();
  Serial.print("AP IP Address:");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);
  server.begin();
  Serial.println("HTTP Server Started.");
}

void loop() {
  // put your main code here, to run repeatedly:
}
