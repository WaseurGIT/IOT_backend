#include "esp_camera.h"
#include <WiFi.h>
#include <SocketIOclient.h>
#include <ArduinoJson.h>
#include <mbedtls/base64.h>

// ===== WIFI CONFIGURATION ===== //
const char* ssid = "TP-Link";
const char* password = "asdfghjkl";
// ============================== //

// Backend Configuration
const char* backendHost = "iot-backend-uy96.onrender.com";  // Render backend host
const int backendPort = 443;                                // Render uses HTTPS/WSS
const int frameInterval = 100; // milliseconds (10 FPS)

SocketIOclient socketIO;
bool socketConnected = false;

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

// Forward declarations
void identifyWithServer();

// Socket.IO event handler
void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case sIOtype_DISCONNECT:
      Serial.println("❌ Socket.IO disconnected");
      socketConnected = false;
      break;

    case sIOtype_CONNECT:
      Serial.println("✅ Socket.IO connected");
      Serial.printf("📡 Connected to: %s:%d\n", backendHost, backendPort);
      socketIO.send(sIOtype_CONNECT, "/");
      identifyWithServer();
      socketConnected = true;
      break;

    case sIOtype_EVENT: {
      Serial.printf("📨 Event received: %s\n", payload);
      DynamicJsonDocument doc(256);
      DeserializationError err = deserializeJson(doc, payload, length);
      if (err) {
        Serial.printf("⚠️  Failed to parse event payload: %s\n", err.c_str());
        return;
      }

      JsonArray array = doc.as<JsonArray>();
      if (array.size() == 0) {
        return;
      }

      String eventName = array[0].as<String>();
      if (eventName == "capture") {
        Serial.println("📸 Capture command received from server");
        // Optional: implement capture handling
      } else if (eventName == "message" && array.size() > 1) {
        Serial.printf("💬 Message payload: %s\n", array[1].as<const char*>());
      }
      break;
    }

    case sIOtype_ERROR:
      Serial.printf("❌ Socket.IO error: %s\n", payload ? (char*)payload : "unknown");
      break;

    default:
      Serial.println("ℹ️  Unhandled Socket.IO message type");
      break;
  }
}

void identifyWithServer() {
  DynamicJsonDocument doc(256);
  JsonArray array = doc.to<JsonArray>();
  array.add("message");

  DynamicJsonDocument payloadDoc(128);
  payloadDoc["type"] = "esp32_camera";
  payloadDoc["device"] = "ESP32-CAM";

  String payloadString;
  serializeJson(payloadDoc, payloadString);
  array.add(payloadString);

  String message;
  serializeJson(doc, message);
  socketIO.sendEVENT(message);
  Serial.println("📤 Identification event sent to server");
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n🚀 ESP32-CAM Starting (WebSocket Mode)...");
  
  // Check PSRAM availability
  if (psramFound()) {
    Serial.println("✅ PSRAM: Available");
  } else {
    Serial.println("⚠️  PSRAM: Not available (will use DRAM)");
  }
  
  if (initCamera()) {
    Serial.println("✅ Camera: READY");
  } else {
    Serial.println("❌ Camera: FAILED");
    Serial.println("⚠️  Continuing without camera...");
    // Don't return - allow WiFi and Socket.IO to work even without camera
  }
  
  if (connectWiFi()) {
    Serial.println("✅ WiFi: CONNECTED");
    
    // Initialize Socket.IO connection (secure WSS)
    Serial.printf("🔌 Connecting to Socket.IO: wss://%s:%d/socket.io/?EIO=4&transport=websocket\n", 
                  backendHost, backendPort);
    
    socketIO.beginSSL(backendHost, backendPort, "/socket.io/?EIO=4&transport=websocket");
    socketIO.onEvent(socketIOEvent);
    socketIO.setReconnectInterval(5000);
    
    Serial.println("🎥 Waiting for Socket.IO connection...");
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
  config.xclk_freq_hz = 20000000;  // Increased from 10MHz to 20MHz for better compatibility
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Try smaller frame size first if PSRAM is not available
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA;  // 640x480
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_PSRAM;
  } else {
    config.frame_size = FRAMESIZE_QVGA;  // 320x240 (smaller, uses less memory)
    config.fb_count = 1;
    config.fb_location = CAMERA_FB_IN_DRAM;
  }
  
  config.jpeg_quality = 12;  // Lower = better quality (0-63)
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  // Initialize camera
  Serial.println("📷 Initializing camera...");
  esp_err_t err = esp_camera_init(&config);
  
  if (err != ESP_OK) {
    Serial.printf("❌ Camera init failed with error 0x%x\n", err);
    
    // Try with alternative configuration (without PSRAM)
    Serial.println("🔄 Trying alternative configuration (no PSRAM)...");
    config.fb_location = CAMERA_FB_IN_DRAM;
    config.fb_count = 1;
    err = esp_camera_init(&config);
    
    if (err != ESP_OK) {
      Serial.printf("❌ Camera init failed again with error 0x%x\n", err);
      
      // Try with even smaller frame size
      Serial.println("🔄 Trying with QVGA frame size...");
      config.frame_size = FRAMESIZE_QVGA;  // 320x240
      config.fb_location = CAMERA_FB_IN_DRAM;
      config.fb_count = 1;
      err = esp_camera_init(&config);
      
      if (err != ESP_OK) {
        Serial.printf("❌ Camera init failed with QVGA, error 0x%x\n", err);
        Serial.println("💡 Troubleshooting tips:");
        Serial.println("   1. Check camera module connections (all pins properly soldered)");
        Serial.println("   2. Verify camera module is OV2640 (most common for ESP32-CAM)");
        Serial.println("   3. Ensure stable 5V power supply (use external power if needed)");
        Serial.println("   4. Check if camera module is properly seated");
        Serial.println("   5. Verify pin configuration matches your board");
        Serial.println("   6. Try power cycling the ESP32-CAM");
        return false;
      }
    }
  }

  // Get camera sensor
  sensor_t *s = esp_camera_sensor_get();
  if (s == NULL) {
    Serial.println("❌ Failed to get camera sensor");
    return false;
  }

  // Print camera info
  Serial.println("✅ Camera initialized successfully!");
  Serial.println("   Camera sensor detected and ready");
  
  // Set initial sensor settings
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);         // -2 to 2
  s->set_saturation(s, 0);       // -2 to 2
  s->set_special_effect(s, 0);  // 0 to 6
  s->set_whitebal(s, 1);        // 0 = disable, 1 = enable
  s->set_awb_gain(s, 1);        // 0 = disable, 1 = enable
  s->set_wb_mode(s, 0);         // 0 to 4
  s->set_exposure_ctrl(s, 1);   // 0 = disable, 1 = enable
  s->set_aec2(s, 0);            // 0 = disable, 1 = enable
  s->set_ae_level(s, 0);        // -2 to 2
  s->set_aec_value(s, 300);     // 0 to 1200
  s->set_gain_ctrl(s, 1);       // 0 = disable, 1 = enable
  s->set_agc_gain(s, 0);        // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);             // 0 = disable, 1 = enable
  s->set_wpc(s, 1);             // 0 = disable, 1 = enable
  s->set_raw_gma(s, 1);         // 0 = disable, 1 = enable
  s->set_lenc(s, 1);            // 0 = disable, 1 = enable
  s->set_hmirror(s, 0);         // 0 = disable, 1 = enable
  s->set_vflip(s, 0);           // 0 = disable, 1 = enable
  s->set_dcw(s, 1);             // 0 = disable, 1 = enable
  s->set_colorbar(s, 0);        // 0 = disable, 1 = enable

  return true;
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

bool encodeFrameToBase64(const uint8_t *input, size_t length, String &output) {
  size_t expectedLength = ((length + 2) / 3) * 4;
  unsigned char *buffer = (unsigned char*)malloc(expectedLength + 1);
  if (!buffer) {
    Serial.println("❌ Failed to allocate memory for Base64 encoding");
    return false;
  }

  size_t encodedLength = 0;
  int ret = mbedtls_base64_encode(buffer, expectedLength + 1, &encodedLength, input, length);
  if (ret != 0) {
    Serial.printf("❌ Base64 encoding failed: %d\n", ret);
    free(buffer);
    return false;
  }

  buffer[encodedLength] = '\0';
  output = (char*)buffer;
  free(buffer);
  return true;
}

void sendFrameViaWebSocket() {
  if (!socketConnected || !socketIO.isConnected()) {
    // Don't spam logs
    if (millis() % 5000 < frameInterval) {
      Serial.println("⚠️  Waiting for Socket.IO connection...");
    }
    return;
  }
  
  // Check if camera is available
  sensor_t *s = esp_camera_sensor_get();
  if (s == NULL) {
    // Camera not initialized, skip frame sending
    return;
  }
  
  camera_fb_t *fb = esp_camera_fb_get();
  
  if (!fb) {
    // Don't spam logs for camera failures
    if (millis() % 5000 < frameInterval) {
      Serial.println("⚠️  Camera capture failed");
    }
    return;
  }

  String base64Frame;
  if (!encodeFrameToBase64(fb->buf, fb->len, base64Frame)) {
    esp_camera_fb_return(fb);
    return;
  }

  DynamicJsonDocument doc(256 + base64Frame.length());
  JsonArray array = doc.to<JsonArray>();
  array.add("frame");
  array.add(base64Frame);

  String message;
  serializeJson(doc, message);
  if (!socketIO.sendEVENT(message)) {
    Serial.println("❌ Failed to send frame event");
  }
  
  // Print status every 5 seconds to avoid spam
  if (millis() % 5000 < frameInterval) {
    Serial.printf("✅ Frame sent via Socket.IO (size: %d bytes, base64 length: %d)\n", fb->len, base64Frame.length());
  }
  
  esp_camera_fb_return(fb);
}

void loop() {
  // Socket.IO loop - MUST be called frequently
  socketIO.loop();
  
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("⚠️  WiFi disconnected, reconnecting...");
    socketConnected = false;
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