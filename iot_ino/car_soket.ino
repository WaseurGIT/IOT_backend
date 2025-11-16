#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// ===== DEPLOYMENT CONFIGURATION ===== //
#define USE_RENDER false  // Set to true for Render.com, false for local development
// ==================================== //

// ===== WIFI CONFIGURATION ===== //
const char* ssid = "TP-Link";
const char* password = "asdfghjkl";
// ============================== //

// Server Configuration
#if USE_RENDER
  // Render.com configuration (Production)
  const char* serverHost = "iot-backend-uy96.onrender.com";
  const uint16_t serverPort = 443;
  const char* serverPath = "/ws";
#else
  // Local development configuration
  const char* serverHost = "192.168.0.115";
  const uint16_t serverPort = 3000;
  const char* serverPath = "/ws";
#endif

// Motor A (Left wheels) pins
#define ENA 33
#define IN1 25
#define IN2 26

// Motor B (Right wheels) pins
#define ENB 32
#define IN3 14
#define IN4 12

// PWM settings
const int freq = 30000;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int resolution = 8;

// Default motor speed (0-255)
uint8_t motorSpeed = 200;

// WebSocket client
WebSocketsClient webSocket;
bool wsConnected = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n🚗 ESP32 Car Controller Starting...");
  
  // Initialize motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Setup PWM
  ledcSetup(pwmChannelA, freq, resolution);
  ledcAttachPin(ENA, pwmChannelA);
  ledcSetup(pwmChannelB, freq, resolution);
  ledcAttachPin(ENB, pwmChannelB);

  // Stop motors initially
  stopCar();

  // Connect to WiFi
  if (!connectWiFi()) {
    Serial.println("❌ WiFi connection failed. Restarting...");
    delay(5000);
    ESP.restart();
  }

  // Connect to WebSocket
  connectWebSocket();
}

// ===== MOTOR CONTROL FUNCTIONS ===== //

void motorAForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorABackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void motorAStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}

void motorBForward() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void motorBBackward() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motorBStop() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setMotorASpeed(uint8_t speed) {
  ledcWrite(pwmChannelA, speed);
}

void setMotorBSpeed(uint8_t speed) {
  ledcWrite(pwmChannelB, speed);
}

// ===== CAR CONTROL FUNCTIONS ===== //

void moveForward() {
  motorAForward();
  motorBForward();
  setMotorASpeed(motorSpeed);
  setMotorBSpeed(motorSpeed);
  Serial.println("⬆️  Moving FORWARD");
}

void moveBackward() {
  motorABackward();
  motorBBackward();
  setMotorASpeed(motorSpeed);
  setMotorBSpeed(motorSpeed);
  Serial.println("⬇️  Moving BACKWARD");
}

void turnLeft() {
  // Left motor backward, right motor forward
  motorABackward();
  motorBForward();
  setMotorASpeed(motorSpeed);
  setMotorBSpeed(motorSpeed);
  Serial.println("⬅️  Turning LEFT");
}

void turnRight() {
  // Left motor forward, right motor backward
  motorAForward();
  motorBBackward();
  setMotorASpeed(motorSpeed);
  setMotorBSpeed(motorSpeed);
  Serial.println("➡️  Turning RIGHT");
}

void stopCar() {
  motorAStop();
  motorBStop();
  setMotorASpeed(0);
  setMotorBSpeed(0);
  Serial.println("🛑 Car STOPPED");
}

// ===== WIFI FUNCTIONS ===== //

bool connectWiFi() {
  Serial.println("\n📡 Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("✅ WiFi Connected! IP: %s\n", WiFi.localIP().toString().c_str());
      return true;
    }
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n❌ WiFi Connection Failed");
  return false;
}

// ===== WEBSOCKET FUNCTIONS ===== //

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("❌ WebSocket DISCONNECTED");
      wsConnected = false;
      break;
      
    case WStype_CONNECTED:
      Serial.printf("✅ WebSocket CONNECTED to: %s\n", payload);
      wsConnected = true;
      sendIdentification();
      break;
      
    case WStype_TEXT:
      handleCommand((char*)payload);
      break;
      
    case WStype_ERROR:
      Serial.printf("❌ WebSocket ERROR: %s\n", payload);
      break;
      
    default:
      break;
  }
}

void sendIdentification() {
  DynamicJsonDocument doc(200);
  doc["type"] = "esp32_car";
  doc["device"] = "ESP32-Car";
  doc["status"] = "ready";
  
  String message;
  serializeJson(doc, message);
  webSocket.sendTXT(message);
  Serial.println("📤 Sent identification: ESP32-Car");
}

void handleCommand(char* payload) {
  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, payload);
  
  if (error) {
    Serial.printf("❌ JSON parse error: %s\n", error.c_str());
    return;
  }
  
  String command = doc["command"] | "";
  int speed = doc["speed"] | motorSpeed;
  
  // Update motor speed if provided
  if (doc.containsKey("speed")) {
    motorSpeed = constrain(speed, 0, 255);
    Serial.printf("⚙️  Motor speed set to: %d\n", motorSpeed);
  }
  
  // Execute command
  if (command == "forward") {
    moveForward();
  } else if (command == "backward") {
    moveBackward();
  } else if (command == "left") {
    turnLeft();
  } else if (command == "right") {
    turnRight();
  } else if (command == "stop") {
    stopCar();
  } else {
    Serial.printf("⚠️  Unknown command: %s\n", command.c_str());
  }
  
  // Send acknowledgment
  DynamicJsonDocument ack(100);
  ack["type"] = "ack";
  ack["command"] = command;
  ack["status"] = "executed";
  
  String ackMessage;
  serializeJson(ack, ackMessage);
  webSocket.sendTXT(ackMessage);
}

void connectWebSocket() {
  Serial.println("\n🔌 Connecting to WebSocket...");
  
  #if USE_RENDER
    Serial.println("   Mode: Production (Render.com)");
    Serial.println("   Protocol: WSS (Secure)");
    Serial.printf("   Server: %s:%d%s\n", serverHost, serverPort, serverPath);
    webSocket.beginSSL(serverHost, serverPort, serverPath);
  #else
    Serial.println("   Mode: Local Development");
    Serial.println("   Protocol: WS (Plain)");
    Serial.printf("   Server: %s:%d%s\n", serverHost, serverPort, serverPath);
    webSocket.begin(serverHost, serverPort, serverPath);
  #endif
  
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(2000);
}

// ===== MAIN LOOP ===== //

void loop() {
  webSocket.loop();
  
  // Handle WiFi disconnection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi disconnected");
    wsConnected = false;
    delay(5000);
    if (connectWiFi()) {
      connectWebSocket();
    }
    return;
  }
  
  // Try to reconnect WebSocket if disconnected
  if (!wsConnected) {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 5000) {
      Serial.println("🔄 Attempting to reconnect WebSocket...");
      connectWebSocket();
      lastReconnectAttempt = millis();
    }
  }
  
  delay(10);
}