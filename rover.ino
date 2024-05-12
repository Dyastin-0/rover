#include <ArduinoJson.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <WebSocketsServer_Generic.h>
#include <Hash.h>

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

const byte sunlightAO = 17;

int wheelSpeed = 255;
int currentSpeed = 127;
byte shift = 0;

const byte maxDistance = 50;
byte mode = 0;

byte leftControlVal = 0;
byte rightControlVal = 0;
byte forwardControlVal = 0;
byte backwardControlVal = 0;

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup(void) {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

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

  pinMode(sunlightAO, INPUT);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  addMessage("Connected to " + String(ssid));
  addMessage("IP address: " + WiFi.localIP().toString());

  if (MDNS.begin("esp8266-rover")) { addMessage("MDNS started."); }

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
  server.on("/refreshLogs", handleRefreshLogs);

  server.begin();
  addMessage("HTTP server started.");
  webSocket.begin();
  addMessage("Web socket server started.");
  digitalWrite(wheelSpeedSignal, currentSpeed);
}

void loop() {
  server.handleClient();
  webSocket.loop();
  mode ? handleAutomaticMode() : handleControls();
}

int avoidDistance = 30;
int actionDelay = 400;
void handleAutomaticMode() {
  float midDistance = measureMidDistance();
  float leftDistance = measureLeftDistance();
  float rightDistance = measureRightDistance();
  int lightIntensity = measureLightIntensity();

  addSensorData("Left distance: " + String(leftDistance) + "cm");
  addSensorData("Mid distance: " + String(midDistance) + "cm");
  addSensorData("Right distance: " + String(rightDistance) + "cm");
  addSensorData("Light intensity: " + String(lightIntensity));
  broadcastSensorData();
  
  if (lightIntensity < 1023) {
    determineDirection(leftDistance, rightDistance);
  } else {
    if (midDistance < avoidDistance) {
    determineDirection(leftDistance, rightDistance);
  }
  forward(1, 1);
  }
}

void determineDirection(int leftDistance, int rightDistance) {
  forward(0, 0);
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

int measureLightIntensity() {
  int lightIntensity = analogRead(sunlightAO);
  return lightIntensity;
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
  if (!mode) leftControlVal = signal;
  server.send(200, "text/plain", "Success");
}
void handleRight() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) rightControlVal = signal;
  server.send(200, "text/plain", "Success");
}
void handleForward() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) forwardControlVal = signal;
  server.send(200, "text/plain", "Success");
}
void handleBackward() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) backwardControlVal = signal;
  server.send(200, "text/plain", "Success");
}

void handleSpeed() {
  float speedPercent = (float)server.arg("plain").toInt() / 10.0;
  currentSpeed = speedPercent * wheelSpeed;
  analogWrite(wheelSpeedSignal, currentSpeed);
  addMessage("Current speed: " + String(currentSpeed) + ".");
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "Success");
}

void handleMode() {
  byte modeControl = (byte)server.arg("plain").toInt();
  mode = modeControl;
  forwardControlVal = 0;
  backwardControlVal = 0;
  leftControlVal = 0;
  rightControlVal = 0;
  String text = mode ? "Automatic mode." : "Manual mode.";
  addMessage(text);
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "success");
}

void handleDistance() {
  int distance = server.arg("plain").toInt();
  avoidDistance = distance;
  addMessage("Avoidance set to: " + String(distance) + "cm.");
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "success");
}

void handleDelay() {
  int delay = server.arg("plain").toInt();
  actionDelay = delay;
  addMessage("Action delay set to: " + String(delay) + "ms.");
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "success");
}

void handleRefreshLogs() {
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "success");
}

void getDistance() {
  server.send(200, "text/plain", String(avoidDistance));
}

void getDelay() {
  server.send(200, "text/plain", String(actionDelay));
}

const int maxMessages = 5;
String messages[maxMessages];
int messageCount = 0;

void addMessage(const String& message) {
  if (messageCount < maxMessages) {
    messages[messageCount++] = message;
  } else {
    for (int i = 0; i < maxMessages - 1; i++) {
      messages[i] = messages[i + 1];
    }
    messages[maxMessages - 1] = message;
  }
}

void broadcastLogs() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.createNestedArray("logs");
  for (int i = 0; i < messageCount; i++) {
    array.add(messages[i]);
  }
  String response;
  serializeJson(doc, response);
  webSocket.broadcastTXT(response);
}

const int maxSensorData = 4;
String sensorData[maxSensorData];
int dataCount = 0;

void addSensorData(const String& data) {
  if (dataCount < maxSensorData) {
    sensorData[dataCount++] = data;
  } else {
    for (int i = 0; i < maxSensorData - 1; i++) {
      sensorData[i] = sensorData[i + 1];
    }
    sensorData[maxSensorData - 1] = data;
  }
}

void broadcastSensorData() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.createNestedArray("sensors");
  for (int i = 0; i < messageCount; i++) {
    array.add(sensorData[i]);
  }
  String response;
  serializeJson(doc, response);
  webSocket.broadcastTXT(response);
}

void broadcastVariables() {
  DynamicJsonDocument doc(1024);
  JsonArray array = doc.createNestedArray("variables");
  array.add(avoidDistance);
  array.add(actionDelay);
  float speed = currentSpeed / 255.0;
  array.add(speed);
  array.add(mode);

  String response;
  serializeJson(doc, response);
  webSocket.broadcastTXT(response);
}