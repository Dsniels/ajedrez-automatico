#include "esp_camera.h"
#include <WiFi.h>
#include "base64.h"
#include <HTTPClient.h>


#include "board_config.h"
#define BUTTON_PIN 13  

const char *ssid = "******";
const char *password = "*****";
const char* endpoint= "****";
const char* apiKey = "****";
const char* deploymentName="gpt-4.1";
void startCameraServer();
void setupLedFlash();
std::vector<String> conversationHistory;

void sendPhotoToAzure();
void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
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
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_UXGA;
  config.pixel_format = PIXFORMAT_JPEG;  
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 4;
  config.fb_count = 2;

  if (config.pixel_format == PIXFORMAT_JPEG) {
    if (psramFound()) {
      config.jpeg_quality = 10;
      config.fb_count = 2;
      config.grab_mode = CAMERA_GRAB_LATEST;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
    }
  } else {
    config.frame_size = FRAMESIZE_240X240;
#if CONFIG_IDF_TARGET_ESP32S3
    config.fb_count = 2;
#endif
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t *s = esp_camera_sensor_get();
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);        
    s->set_brightness(s, 1);   
    s->set_saturation(s, -2);  
  }
  if (config.pixel_format == PIXFORMAT_JPEG) {
    s->set_framesize(s, FRAMESIZE_QVGA);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  s->set_vflip(s, 1);
#endif


#if defined(LED_GPIO_NUM)
  setupLedFlash();
#endif

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");
}

void loop() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    
    if (cmd == "TAKE") {
      sendPhotoToAzure();
    }
  }
  delay(100);
}
void uploadPhotoToServer(camera_fb_t* fb) {
  HTTPClient http;
  http.begin("http://192.168.1.21:9000/upload"); 
  http.addHeader("Content-Type", "image/jpeg");
  
  int code = http.POST(fb->buf, fb->len);
  Serial.printf("Upload: %d\n", code);
  http.end();
}


void sendPhotoToAzure() {
  

  camera_fb_t* old = esp_camera_fb_get();
  if (old) esp_camera_fb_return(old);
  
  delay(100);
  camera_fb_t * fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Error al capturar la imagen");
    return;
  }
   
  String imageBase64 = base64::encode(fb->buf, fb->len);
  String payload = R"({"model":"gpt-4.1","messages":[)";
  
    payload += R"({"role":"system","content":"You are a chess engine. Always respond with UCI notation only.  You should always assume that the black's already move, Your goal is to make the white's win, before you select the move look at the image again and determine if that is the best/valid move. Chess board from White's perspective,  Bottom-left is a1, bottom-right is h1, top-left is a8, top-right is h8. White pieces start on rows 1-2, Black on rows 7-8. Always Identify all pieces on the board when you receive the image, then give White's best move in UCI format (e.g., e2e4). Look carefully at the board. in each move try to use different pieces to make the game more dynamic"},)";

  for (size_t i = 0; i < conversationHistory.size(); i++) {
    payload += conversationHistory[i];
    if (i < conversationHistory.size() - 1 || true) payload += ",";
  }
  
  payload += R"({"role":"user","content":[)";
  payload += R"({"type":"text","text":"White's best next move in UCI notation (e.g., e2e4, g1f3) base in the image, YOU SHOULD ONLY RETURN THE UCI NOTATION.  No explanation.  Format: FROM_SQUARE TO_SQUARE"},)";
  payload += R"({"type":"image_url","image_url":{"url":"data:image/jpeg;base64,)";
  payload += imageBase64;
  payload += R"("}})";
  payload += R"(]})";
  
  payload += R"(],"max_tokens":100})";
  
  HTTPClient http;
  http.begin(endpoint);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("api-key", apiKey);
  Serial.println("....");

  int code = http.POST(payload);

  if (code == 200) {
    String resp = http.getString();
    
    int idx = resp.indexOf("\"content\":\"");
    if (idx != -1) {
      idx += 11;
      int end = resp.indexOf("\"", idx);
      String move = resp.substring(idx, end);
      move.trim();
      
      Serial.println(move);
      
      String assistantMsg = R"({"role":"assistant","content":")" + move + R"("})";
      conversationHistory.push_back(assistantMsg);
      if (conversationHistory.size() > 20) {
        conversationHistory.erase(conversationHistory.begin());
      }
    }
  } else {
    Serial.printf("ERR:%d\n", code);
  }

  http.end();
  esp_camera_fb_return(fb);
}
