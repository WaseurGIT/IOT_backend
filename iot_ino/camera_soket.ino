#include "esp_camera.h"
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// ===== DEPLOYMENT CONFIGURATION ===== //
#define USE_RENDER false
// ==================================== //

// ===== WIFI CONFIGURATION ===== //
const char* ssid = "TP-Link";
const char* password = "asdfghjkl";
// ============================== //

// Server Configuration - CAMERA ENDPOINT
#if USE_RENDER
  const char* serverHost = "iot-backend-uy96.onrender.com";
  const uint16_t serverPort = 443;
  const char* serverPath = "/ws/camera";  // Changed to camera endpoint
#else
  const char* serverHost = "192.168.0.115";
  const uint16_t serverPort = 3000;
  const char* serverPath = "/ws/camera";  // Changed to camera endpoint
#endif

WebSocketsClient webSocket;
WiFiClientSecure secureClient;
bool wsConnected = false;
unsigned long lastFrameTime = 0;
const int frameInterval = 100; // ~10 FPS

// Camera configuration
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("❌ Camera WebSocket DISCONNECTED");
      wsConnected = false;
      break;
      
    case WStype_CONNECTED:
      Serial.printf("✅ Camera WebSocket CONNECTED to: %s\n", payload);
      wsConnected = true;
      sendIdentification();
      break;
      
    case WStype_TEXT:
      if (length < 100) {
        Serial.printf("📨 Camera message: %s\n", payload);
      }
      break;
      
    case WStype_BIN:
      Serial.printf("📦 Camera binary data: %d bytes\n", length);
      break;
      
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

void sendIdentification() {
  DynamicJsonDocument doc(200);
  doc["type"] = "esp32_camera";
  doc["device"] = "ESP32-CAM";
  doc["frame_size"] = "CIF";
  doc["quality"] = 15;
  
  String message;
  serializeJson(doc, message);
  webSocket.sendTXT(message);
}

void sendFrame() {
  if (!wsConnected || !webSocket.isConnected()) {
    return;
  }
  
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("❌ Camera capture failed");
    return;
  }
  
  // Send as binary data
  bool success = webSocket.sendBIN(fb->buf, fb->len);
  
  if (!success) {
    Serial.println("❌ Failed to send frame");
    wsConnected = false;
  }
  
  // Print status every 30 frames
  static unsigned long frameCount = 0;
  if (++frameCount % 30 == 0) {
    Serial.printf("✅ Frames sent: %lu (size: %d bytes)\n", frameCount, fb->len);
  }
  
  esp_camera_fb_return(fb);
}

bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_CIF;
  config.jpeg_quality = 15;
  config.fb_count = 1;

  return esp_camera_init(&config) == ESP_OK;
}

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
  Serial.println("\n❌ WiFi Failed");
  return false;
}

void connectWebSocket() {
  Serial.println("\n🔌 Connecting to Camera WebSocket...");
  
  #if USE_RENDER
    Serial.println("   Mode: Production (Render.com)");
    Serial.println("   Protocol: WSS (Secure)");
    Serial.printf("   Server: %s:%d%s\n", serverHost, serverPort, serverPath);
    secureClient.setInsecure();
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

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n🚀 ESP32-CAM WebSocket Streamer Starting...");
  
  #if USE_RENDER
    Serial.println("📡 Target: Render.com (Production)");
  #else
    Serial.println("📡 Target: Local Server");
  #endif
  
  if (!initCamera()) {
    Serial.println("❌ Camera initialization failed!");
    return;
  }
  Serial.println("✅ Camera initialized");
  
  if (!connectWiFi()) {
    return;
  }
  
  connectWebSocket();
}

void loop() {
  webSocket.loop();
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️ WiFi disconnected");
    wsConnected = false;
    delay(5000);
    connectWiFi();
    if (WiFi.status() == WL_CONNECTED) {
      connectWebSocket();
    }
    return;
  }
  
  if (wsConnected && webSocket.isConnected()) {
    unsigned long currentTime = millis();
    if (currentTime - lastFrameTime >= frameInterval) {
      sendFrame();
      lastFrameTime = currentTime;
    }
  } else {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 5000) {
      Serial.println("🔄 Attempting to reconnect camera...");
      connectWebSocket();
      lastReconnectAttempt = millis();
    }
  }
  
  delay(10);
}