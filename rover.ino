#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "secrets.h"
#include "website.h"

const byte forwardRightWheel = 16;
const byte backwardRightWheel = 5;
const byte backwardLeftWheel = 4;
const byte forwardLeftWheel = 0;
const byte wheelSpeedSignal = 2;

const byte udsEchoMid = 14;
const byte udsTriggerMid = 12;

const byte udsEchoLeft = 13;
const byte udsTriggerLeft = 15;

const byte udsEchoRight = 3;
const byte udsTriggerRight = 1;

int wheelSpeed = 255;
byte shift = 0;

const byte maxDistance = 50;
byte mode = 0;

byte leftControlVal = 0;
byte rightControlVal = 0;
byte forwardControlVal = 0;
byte backwardControlVal = 0;

ESP8266WebServer server(80);

void setup(void) {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  pinMode(forwardLeftWheel, OUTPUT);
  pinMode(forwardRightWheel, OUTPUT);
  pinMode(backwardLeftWheel, OUTPUT);
  pinMode(backwardRightWheel, OUTPUT);
  pinMode(wheelSpeedSignal, OUTPUT);

  pinMode(udsEchoMid, INPUT);
  pinMode(udsTriggerMid, OUTPUT);
  pinMode(udsEchoLeft, INPUT);
  pinMode(udsTriggerLeft, OUTPUT);
  pinMode(udsEchoRight, INPUT);
  pinMode(udsTriggerRight, OUTPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("[Rover] Connected to ");
  Serial.println(ssid);
  Serial.print("[Rover] IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("[Rover] MDNS is on."); }

  server.on("/", handleRoot);
  server.on("/controlLeft", handleLeft);
  server.on("/controlRight", handleRight);
  server.on("/controlForward", handleForward);
  server.on("/controlBackward", handleBackward);
  server.on("/controlSpeed", handleSpeed);
  server.on("/controlMode", handleMode);
  server.on("/setDistance", handleDistance);
  server.on("/setDelay", handleDelay);
  server.on("/get/distance", getDistance);
  server.on("/get/delay", getDelay);

  server.begin();
  Serial.println("[Rover] HTTP sever is on.");
}

void loop(void) {
  server.handleClient();
  mode ? handleAutomaticMode() : handleControls();
}

int avoidDistance = 30;
int actionDelay = 400;
void handleAutomaticMode() {
  float midDistance = measureMidDistance();
  float leftDistance = measureLeftDistance();
  float rightDistance = measureRightDistance();
  Serial.printf("[Rover] Left: %f\n", leftDistance);
  Serial.printf("[Rover] Right: %f\n", rightDistance);
  Serial.printf("[Rover] Mid: %f\n", midDistance);
  if (midDistance < avoidDistance) {
    Serial.printf("[Rover] Distance: %d\n", avoidDistance);
    forward(0, 0);
    delay(actionDelay);
    backward(1, 1);
    delay(actionDelay);
    backward(0, 0);
    delay(actionDelay);
    if (leftDistance > rightDistance) {
      forward(0, 1);
      backward(1, 0);
    } else {
      forward(1, 0);
      backward(0, 1);
    }
    delay(actionDelay);
    backward(0, 0);
    forward(0, 0);
    delay(actionDelay);
  }
  forward(1, 1);
}

float measureMidDistance() {
  digitalWrite(udsTriggerMid, LOW);
  delayMicroseconds(2);
  digitalWrite(udsTriggerMid, HIGH);
  delayMicroseconds(10);
  digitalWrite(udsTriggerMid, LOW);
  long duration = pulseIn(udsEchoMid, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

float measureLeftDistance() {
  digitalWrite(udsTriggerLeft, LOW);
  delayMicroseconds(2);
  digitalWrite(udsTriggerLeft, HIGH);
  delayMicroseconds(10);
  digitalWrite(udsTriggerLeft, LOW);
  long duration = pulseIn(udsEchoLeft, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

float measureRightDistance() {
  digitalWrite(udsTriggerRight, LOW);
  delayMicroseconds(2);
  digitalWrite(udsTriggerRight, HIGH);
  delayMicroseconds(10);
  digitalWrite(udsTriggerRight, LOW);
  long duration = pulseIn(udsEchoRight, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void handleControls() {
  if (leftControlVal) {
    forward(0 , 1);
    backward(1, 0);
  } else {
    if (!forwardControlVal && !rightControlVal) {
      forward(0, 0);
      backward(0, 0);
    }
  }

  if (rightControlVal) {
    forward(1, 0);
    backward(0, 1);
  } else {
    if (!forwardControlVal && !leftControlVal) {
      forward(0, 0);
      backward(0, 0);
    }
  }

  if (forwardControlVal) {
    forward(1, 1);
  } else {
    if (!leftControlVal && !rightControlVal) {
      forward(0, 0);
    }
  }
  
  if (backwardControlVal) {
    backward(1, 1);
  } else {
    if (!leftControlVal && !rightControlVal) {
      backward(0, 0);
    }
  }
}

void backward(byte backwardLeftWheelState, byte backwardRightWheelState) {
  digitalWrite(backwardLeftWheel, backwardLeftWheelState);
  digitalWrite(backwardRightWheel, backwardRightWheelState);
}

void forward(byte forwardLeftWheelState, byte forwardRightWheelState) {
  digitalWrite(forwardLeftWheel, forwardLeftWheelState);
  digitalWrite(forwardRightWheel, forwardRightWheelState);
}

void handleRoot() {
  server.send(200, "text/html", controlPage);
}

void handleLeft() {
  byte signal = (byte)server.arg("plain").toInt();
  Serial.printf("[Rover] Left set to: %d\n", signal);
  if (!mode) leftControlVal = signal;
  server.send(200, "text/plain", "Success");
}
void handleRight() {
  byte signal = (byte)server.arg("plain").toInt();
  Serial.printf("[Rover] Right set to: %d\n", signal);
  if (!mode) rightControlVal = signal;
  server.send(200, "text/plain", "Success");
}
void handleForward() {
  byte signal = (byte)server.arg("plain").toInt();
  Serial.printf("[Rover] Forward set to: %d\n", signal);
  if (!mode) forwardControlVal = signal;
  server.send(200, "text/plain", "Success");
}
void handleBackward() {
  byte signal = (byte)server.arg("plain").toInt();
  Serial.printf("[Rover] Backward set to: %d\n", signal);
  if (!mode) backwardControlVal = signal;
  server.send(200, "text/plain", "Success");
}

void handleSpeed() {
  float speedPercent = (float)server.arg("plain").toInt() / 10.0;
  int currentSpeed = speedPercent * wheelSpeed;
  Serial.printf("[Rover] Speed set to: %d\n", currentSpeed);
  analogWrite(wheelSpeedSignal, currentSpeed);
  server.send(200, "text/plain", "Success");
}

void handleMode() {
  byte modeControl = (byte)server.arg("plain").toInt();
  mode = modeControl;
  forwardControlVal = 0;
  backwardControlVal = 0;
  leftControlVal = 0;
  rightControlVal = 0;
  String text = mode ? "[Rover] Set to automatic mode." : "[Rover] set to manual mode.";
  Serial.println(text);
  server.send(200, "text/plain", "success");
}

void handleDistance() {
  int distance = server.arg("plain").toInt();
  avoidDistance = distance;
  Serial.printf("[Rover] Obstacle distance set to: %d\n", distance);
  server.send(200, "text/plain", "success");
}

void handleDelay() {
  int delay = server.arg("plain").toInt();
  actionDelay = delay;
  Serial.printf("[Rover] Action delay set to: %d\n", delay);
  server.send(200, "text/plain", "success");
}

void getDistance() {
  server.send(200, "text/plain", String(avoidDistance));
}

void getDelay() {
  server.send(200, "text/plain", String(actionDelay));
}