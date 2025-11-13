#include "esp_camera.h"
#include <WiFi.h>
#include <WebSocketsClient.h>

// ===== WIFI CONFIGURATION ===== //
const char* ssid = "TP-Link";
const char* password = "asdfghjkl";
// ============================== //

// Backend Configuration
const char* backendHost = "192.168.0.115";  // Change to your computer's IP
const int backendPort = 3000;
const int frameInterval = 100; // milliseconds (10 FPS)

WebSocketsClient webSocket;
bool wsConnected = false;

// Camera configuration (same as before)
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

unsigned long lastFrameTime = 0;
unsigned long lastPingTime = 0;

// WebSocket event handler
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("❌ WebSocket Disconnected");
      wsConnected = false;
      break;
      
    case WStype_CONNECTED:
      Serial.println("✅ WebSocket Connected!");
      Serial.printf("📡 Connected to: %s:%d\n", backendHost, backendPort);
      wsConnected = true;
      // Send ESP32 identifier
      webSocket.sendTXT("{\"type\":\"esp32_camera\",\"device\":\"ESP32-CAM\"}");
      break;
      
    case WStype_TEXT:
      Serial.printf("📨 Message received: %s\n", payload);
      // Handle commands from server (optional)
      if (strcmp((char*)payload, "capture") == 0) {
        Serial.println("📸 Capture command received");
      }
      break;
      
    case WStype_BIN:
      Serial.println("📦 Binary data received (unexpected)");
      break;
      
    case WStype_ERROR:
      Serial.println("❌ WebSocket Error!");
      break;
      
    case WStype_PING:
      Serial.println("🏓 Ping received");
      break;
      
    case WStype_PONG:
      Serial.println("🏓 Pong received");
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n🚀 ESP32-CAM Starting (WebSocket Mode)...");
  
  if (initCamera()) {
    Serial.println("✅ Camera: READY");
  } else {
    Serial.println("❌ Camera: FAILED");
    return;
  }
  
  if (connectWiFi()) {
    Serial.println("✅ WiFi: CONNECTED");
    
    // Initialize WebSocket
    Serial.printf("🔌 Connecting to WebSocket: ws://%s:%d/socket.io/?EIO=4&transport=websocket\n", 
                  backendHost, backendPort);
    
    // Socket.IO v4 connection string
    webSocket.begin(backendHost, backendPort, "/socket.io/?EIO=4&transport=websocket");
    webSocket.onEvent(webSocketEvent);
    webSocket.setReconnectInterval(5000);
    
    Serial.println("🎥 Waiting for WebSocket connection...");
  } else {
    Serial.println("❌ WiFi: FAILED");
  }
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
  config.frame_size = FRAMESIZE_VGA;  // 640x480
  config.jpeg_quality = 12;  // Lower = better quality (0-63)
  config.fb_count = 2;  // Use 2 frame buffers for smoother streaming

  esp_err_t err = esp_camera_init(&config);
  return err == ESP_OK;
}

bool connectWiFi() {
  Serial.println("\n📡 Connecting to WiFi...");
  Serial.printf("Network: %s\n", ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.print("Connecting");
  for (int i = 0; i < 20; i++) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Connected!");
      Serial.printf("IP Address: %s\n", WiFi.localIP().toString().c_str());
      return true;
    }
    delay(1000);
    Serial.print(".");
  }
  
  Serial.println("\n❌ Failed to connect");
  return false;
}

void sendFrameViaWebSocket() {
  if (!wsConnected) {
    // Don't spam logs
    if (millis() % 5000 < frameInterval) {
      Serial.println("⚠️  Waiting for WebSocket connection...");
    }
    return;
  }
  
  camera_fb_t *fb = esp_camera_fb_get();
  
  if (!fb) {
    Serial.println("❌ Camera capture failed");
    return;
  }

  // Send binary frame via WebSocket
  webSocket.sendBIN(fb->buf, fb->len);
  
  // Print status every 5 seconds to avoid spam
  if (millis() % 5000 < frameInterval) {
    Serial.printf("✅ Frame sent via WebSocket (size: %d bytes)\n", fb->len);
  }
  
  esp_camera_fb_return(fb);
}

void loop() {
  // WebSocket loop - MUST be called frequently
  webSocket.loop();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi disconnected, reconnecting...");
    wsConnected = false;
    connectWiFi();
    delay(5000);
    return;
  }

  // Send frames at specified interval
  unsigned long currentTime = millis();
  if (currentTime - lastFrameTime >= frameInterval) {
    sendFrameViaWebSocket();
    lastFrameTime = currentTime;
  }
  
  delay(10);  // Small delay to prevent watchdog issues
}