#include <esp_now.h>
#include "WiFi.h"
#define CHANNEL 1

// Thiết lập thông tin về ESP32 #2
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  
  // Khởi động ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Đặt địa chỉ MAC của ESP32 #2
  peerInfo.peer_addr[0] = 0x0c;
  peerInfo.peer_addr[1] = 0xb8;
  peerInfo.peer_addr[2] = 0x15;
  peerInfo.peer_addr[3] = 0x76;
  peerInfo.peer_addr[4] = 0x11;
  peerInfo.peer_addr[5] = 0x40;
  
  // Đăng ký ESP32 #2 là thiết bị nhận
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Error adding peer");
    return;
  }
  
  // Đăng ký hàm callback để nhận dữ liệu
  esp_now_register_recv_cb([](const uint8_t *senderMac, const uint8_t *data, int length) 
  {
    Serial.println("Received from ESP32 #2: " + String((char*)data));
  });
}

void loop() {
  // Đọc dữ liệu từ Serial Monitor và gửi đến ESP32 #2
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    if (data.length() > 0) {
      // Chuyển đổi dữ liệu thành mảng uint8_t và gửi đến ESP32 #2
      uint8_t dataToSend[data.length() + 1];
      data.getBytes(dataToSend, data.length() + 1);

      esp_err_t result = esp_now_send(peerInfo.peer_addr, dataToSend, data.length() + 1);
      if (result == ESP_OK) {
        Serial.println("Sent to ESP32 #2: " + data);
      } else {
        Serial.println("Error sending data");
      }
    }
  }

  // You can add other functions in loop() for ESP32 #1
}