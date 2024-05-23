#include<ArduinoQueue.h>
#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include <WebSocketsServer_Generic.h>

#include "secrets.h"
#include "website.h"
#include "waitingPage.h"

const byte forwardFrontLeftWheel = 25;
const byte backwardFrontLeftWheel = 26;

const byte forwardBackLeftWheel = 32;
const byte backwardBackLeftWheel = 33;

const byte forwardBackRightWheel = 27;
const byte backwardBackRightWheel = 14;

const byte forwardFrontRightWheel = 12;
const byte backwardFrontRightWheel = 13;

const byte leftWheelSpeedSignal = 22;
const byte rightWheelSpeedSignal = 21;

const byte builtInLED = 2;

const byte udsEchoLeft = 5;
const byte udsTriggerLeft = 17;

const byte udsEchoMid = 23;
const byte udsTriggerMid = 19;

const byte udsEchoRight = 16;
const byte udsTriggerRight = 4;

const byte sunlightAO = 34;

int wheelSpeed = 255;
int currentSpeed = 127;
byte shift = 0;

const byte maxDistance = 50;
byte mode = 0;

byte leftControlVal = 0;
byte rightControlVal = 0;
byte forwardControlVal = 0;
byte backwardControlVal = 0;
byte forwardLeftControlVal = 0;
byte forwardRightControlVal = 0;
byte backwardLeftControlVal = 0;
byte backwardRightControlVal = 0;
byte spinLeftControlVal = 0;
byte spinRightControlVal = 0;

int connectedDevices = 0;
int maxConnectedDevices = 1;

ArduinoQueue<uint8_t> clientQueue(20);

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void setup(void) {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  pinMode(forwardFrontLeftWheel, OUTPUT);
  pinMode(backwardFrontLeftWheel, OUTPUT);
  pinMode(forwardFrontRightWheel, OUTPUT);
  pinMode(backwardFrontRightWheel, OUTPUT);
  pinMode(forwardBackLeftWheel, OUTPUT);
  pinMode(backwardBackLeftWheel, OUTPUT);
  pinMode(forwardBackRightWheel, OUTPUT);
  pinMode(backwardBackRightWheel, OUTPUT);

  pinMode(leftWheelSpeedSignal, OUTPUT);
  pinMode(rightWheelSpeedSignal, OUTPUT);

  pinMode(builtInLED, OUTPUT);
  digitalWrite(builtInLED, 0);

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

  digitalWrite(builtInLED, 1);
  addMessage("Connected to " + String(ssid));
  Serial.println(WiFi.localIP());

  MDNS.begin("esp32rover");

  server.on("/", handleRoot);
  server.on("/controlLeft", handleLeft);
  server.on("/controlRight", handleRight);
  server.on("/controlForward", handleForward);
  server.on("/controlBackward", handleBackward);
  server.on("/controlForwardLeft", handleForwardLeft);
  server.on("/controlForwardRight", handleForwardRight);
  server.on("/controlBackwardLeft", handleBackwardLeft);
  server.on("/controlBackwardRight", handleBackwardRight);
  server.on("/controlSpinLeft", handleSpinLeft);
  server.on("/controlSpinRight", handleSpinRight);
  server.on("/controlSpeed", handleSpeed);
  server.on("/controlMode", handleMode);
  server.on("/setDistance", handleDistance);
  server.on("/setDelay", handleDelay);
  server.on("/get/distance", getDistance);
  server.on("/get/delay", getDelay);

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  addMessage("MDNS, web socket, & web server started.");
  digitalWrite(leftWheelSpeedSignal, currentSpeed);
  digitalWrite(rightWheelSpeedSignal, currentSpeed);
}

void loop() {
  server.handleClient();
  webSocket.loop();
  mode ? handleAutomaticMode() : handleControls();
}

int avoidDistance = 30;
int actionDelay = 400;
void handleAutomaticMode() {
  float midDistance = measureDistance(udsTriggerMid, udsEchoMid);
  float leftDistance = measureDistance(udsTriggerLeft, udsEchoLeft);
  float rightDistance = measureDistance(udsTriggerRight, udsEchoRight);
  int lightIntensity = measureLightIntensity(sunlightAO);

  addSensorData("Left: " + String(leftDistance) + "cm");
  addSensorData("Mid: " + String(midDistance) + "cm");
  addSensorData("Right: " + String(rightDistance) + "cm");
  addSensorData("Light: " + String(lightIntensity));
  broadcastSensorData();
  
  if (lightIntensity < 4095) {
    determineDirection(leftDistance, rightDistance);
  } else {
    if (midDistance < avoidDistance) {
    determineDirection(leftDistance, rightDistance);
    } else {
      frontLeftWheels(1, 0);
      backLeftWheels(1, 0);
      frontRightWheels(1, 0);
      backRightWheels(1, 0);
    }
  }
}

void determineDirection(int leftDistance, int rightDistance) {
  frontLeftWheels(0, 1);
  backLeftWheels(0, 1);
  frontRightWheels(0, 1);
  backRightWheels(0, 1);
  delay(actionDelay);
  frontLeftWheels(0, 0);
  backLeftWheels(0, 0);
  frontRightWheels(0, 0);
  backRightWheels(0, 0);
  delay(actionDelay);
  if (leftDistance > rightDistance) {
    frontLeftWheels(0, 1);
    backLeftWheels(0, 1);
    frontRightWheels(1, 0);
    backRightWheels(1, 0);
  } else {
    frontLeftWheels(1, 0);
    backLeftWheels(1, 0);
    frontRightWheels(0, 1);
    backRightWheels(0, 1);
  }
  delay(actionDelay);
  frontLeftWheels(0, 0);
  backLeftWheels(0, 0);
  frontRightWheels(0, 0);
  backRightWheels(0, 0);
  delay(actionDelay);
}

int measureLightIntensity(byte aoPin) {
  int lightIntensity = analogRead(aoPin);
  return lightIntensity;
}

float measureDistance(byte triggerPin, byte echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void handleControls() {
  if (forwardControlVal) {
    frontLeftWheels(1, 0);
    backLeftWheels(1, 0);
    frontRightWheels(1, 0);
    backRightWheels(1, 0);
  } else if (backwardControlVal) {
    frontLeftWheels(0, 1);
    backLeftWheels(0, 1);
    frontRightWheels(0, 1);
    backRightWheels(0, 1);
  } else if (leftControlVal) {
    frontLeftWheels(0, 1);
    backLeftWheels(1, 0);
    frontRightWheels(1, 0);
    backRightWheels(0, 1);
  } else if (rightControlVal) {
    frontLeftWheels(1, 0);
    backLeftWheels(0, 1);
    frontRightWheels(0, 1);
    backRightWheels(1, 0);
  } else if (forwardLeftControlVal) {
    frontLeftWheels(0, 0);
    backLeftWheels(1, 0);
    frontRightWheels(1, 0);
    backRightWheels(0, 0);
  } else if (forwardRightControlVal) {
    frontLeftWheels(1, 0);
    backLeftWheels(0, 0);
    frontRightWheels(0, 0);
    backRightWheels(1, 0);
  } else if (backwardLeftControlVal) {
    frontLeftWheels(0, 1);
    backLeftWheels(0, 0);
    frontRightWheels(0, 0);
    backRightWheels(0, 1);
  } else if (backwardRightControlVal) { 
    frontLeftWheels(0, 0);
    backLeftWheels(0, 1);
    frontRightWheels(0, 1);
    backRightWheels(0, 0);
  } else if (spinLeftControlVal) {
    frontLeftWheels(0, 1);
    backLeftWheels(0, 1);
    frontRightWheels(1, 0);
    backRightWheels(1, 0);
  } else if (spinRightControlVal) {
    frontLeftWheels(1, 0);
    backLeftWheels(1, 0);
    frontRightWheels(0, 1);
    backRightWheels(0, 1);
  } else {
    frontLeftWheels(0, 0);
    backLeftWheels(0, 0);
    frontRightWheels(0, 0);
    backRightWheels(0, 0);
  }
}

void frontLeftWheels(byte forward, byte backward) {
  digitalWrite(forwardFrontLeftWheel, forward);
  digitalWrite(backwardFrontLeftWheel, backward);
}

void backLeftWheels(byte forward, byte backward) {
  digitalWrite(forwardBackLeftWheel, forward);
  digitalWrite(backwardBackLeftWheel, backward);
}

void frontRightWheels(byte forward, byte backward) {
  digitalWrite(forwardFrontRightWheel, forward);
  digitalWrite(backwardFrontRightWheel, backward);
}

void backRightWheels(byte forward, byte backward) {
  digitalWrite(forwardBackRightWheel, forward);
  digitalWrite(backwardBackRightWheel, backward);
}

void handleRoot() {
  Serial.printf("Connected: %d\n", connectedDevices);
  !(connectedDevices == maxConnectedDevices) ? server.send(200, "text/html", controlPage) : server.send(200, "text/html", waitPage);
}

void handleLeft() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) leftControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved leftward.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleRight() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) rightControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved rightward.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleForward() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) forwardControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved forward.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleBackward() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) backwardControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved backward.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleForwardLeft() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) forwardLeftControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved forwardleft.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleForwardRight() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) forwardRightControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved forwardright.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleBackwardLeft() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) backwardLeftControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved backwardleft.");
    broadcastLogs(); 
  }
  server.send(200, "text/plain", "Success");
}

void handleBackwardRight() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) backwardRightControlVal = signal;
  if (!mode && signal) {
    addMessage("Moved backwardright.");
    broadcastLogs();   
  }
  server.send(200, "text/plain", "Success");
}

void handleSpinLeft() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) spinLeftControlVal = signal;
  if (!mode && signal) {
    addMessage("Spinned left.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleSpinRight() {
  byte signal = (byte)server.arg("plain").toInt();
  if (!mode) spinRightControlVal = signal;
  if (!mode && signal) {
    addMessage("Spinned right.");
    broadcastLogs();
  }
  server.send(200, "text/plain", "Success");
}

void handleSpeed() {
  float speedPercent = (float)server.arg("plain").toInt() / 10.0;
  currentSpeed = speedPercent * wheelSpeed;
  digitalWrite(leftWheelSpeedSignal, currentSpeed);
  digitalWrite(rightWheelSpeedSignal, currentSpeed);
  addMessage("Speed: " + String(currentSpeed) + ".");
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
  String text = mode ? "Automatic mode." : "Control mode.";
  addMessage(text);
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "success");
}

void handleDistance() {
  int distance = server.arg("plain").toInt();
  avoidDistance = distance;
  addMessage("Avoidance: " + String(distance) + " cm.");
  broadcastLogs();
  broadcastVariables();
  server.send(200, "text/plain", "success");
}

void handleDelay() {
  int delay = server.arg("plain").toInt();
  actionDelay = delay;
  addMessage("Action delay: " + String(delay) + " ms.");
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
  for (int i = 0; i < dataCount; i++) {
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

void webSocketEvent(const uint8_t& num, const WStype_t& type, uint8_t * payload, const size_t& length) {
  (void) length;
  IPAddress ip = webSocket.remoteIP(num);
  String ipAddress = ip.toString();
  bool isDisconnect = strcmp(reinterpret_cast<const char*>(payload), "disconnect") == 0;
  switch (type) {
    case WStype_CONNECTED: {
      if (connectedDevices != maxConnectedDevices) {
        connectedDevices++;
      } else {
        clientQueue.enqueue(num);
      }
    }
    break;
    case WStype_TEXT:
      if (strcmp(reinterpret_cast<const char*>(payload), "broadcast_request") == 0) {
        broadcastLogs();
        broadcastVariables();
      } 
      if (isDisconnect) {
        addMessage("[" + String(ipAddress) + "] disconnected from the rover.");
        Serial.printf("disc\n");
        connectedDevices--;
        broadcastLogs();
        webSocket.sendTXT(clientQueue.dequeue(), "redirect");
      }
      if (!isDisconnect && !(strcmp(reinterpret_cast<const char*>(payload), "broadcast_request") == 0)) addMessage("[" + String(ipAddress) + "] " + String(reinterpret_cast<const char*>(payload)));
      break;
    default:
      break;
  }
}