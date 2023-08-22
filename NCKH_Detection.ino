#include <WiFi.h>
#include <FirebaseESP32.h>
//#include "BluetoothSerial.h"

// BluetoothSerial SerialBT;

#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

const char* ssid = "Tien Thanh";
const char* password = "tienthanh2003";

#define API_KEY "AIzaSyAMvBCTbMwNGP6kl74H2-HkbxJ_oRCRaXo"

#define USER_EMAIL1 "test@gmail.com"
#define USER_PASSWORD1 "112233"

#define DATABASE_URL "https://data11-415a2-default-rtdb.asia-southeast1.firebasedatabase.app/" 

#define sensor1 32  // quang trở
#define sensor2 19  //sr505mini - cảm biến chuyển động nhiệt
#define LEDR 35      // đèn báo động đỏ - có trộm
#define LEDG 12        // đèn báo động xanh - an toàn
#define Loa 34        // loa báo động
#define Light 13  // đèn ngày - đêm ( dùng relay1 )

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseData firebaseData;

// bool toggleUser = false;

//----------------------------hàm get tk và mk trên firebase--------------------------------------
void signIn(const char *email, const char *password)
{

    auth.user.email = email;
    auth.user.password = password;

    Firebase.reset(&config);
    Firebase.begin(&config, &auth);
}
//---------------------------------------------------------------------------------------------
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}
void setup()
{
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  pinMode(LEDR,OUTPUT);
  pinMode(LEDG,OUTPUT);
  pinMode(Loa,OUTPUT);
  pinMode(Light,OUTPUT);
  Serial.begin(115200);
  
  initWiFi();
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    Firebase.reconnectWiFi(true);
    fbdo.setResponseSize(4096);
    config.token_status_callback = tokenStatusCallback; 
    signIn(USER_EMAIL1, USER_PASSWORD1);
}

//-------------------------------"hệ thống cảnh báo"----------------------------------------------
void detection(float value1, float value2){
  
    //"chế độ trời tối"
    
    if(value1==1){
      Serial.print(F("\n trời tối"));
      Firebase.setString(firebaseData, "/FirebaseIOT/admin/01/sttled","Ban đêm, trời tối.");
      if (value2==0){
        Serial.print(F("\n Không có trộm"));
          digitalWrite(LEDR, LOW);
          digitalWrite(LEDG, HIGH);
          digitalWrite(Loa, LOW);
          digitalWrite(Light, HIGH);

      Firebase.setString(firebaseData, "/FirebaseIOT/admin/01/Detection","Không có khả nghi.");
        }else if(value2 ==1){
          Serial.print(F("\n CÓ TRỘM!!!"));
          digitalWrite(LEDR, HIGH);
          digitalWrite(LEDG, LOW);
          digitalWrite(Loa, HIGH);
          digitalWrite(Light, HIGH);
          
      Firebase.setString(firebaseData, "/FirebaseIOT/admin/01/Detection","Có trộm!!!!!!!");
        } 
      }
      
      //"chế độ trời sáng"
      
      if(value1==0){
        Serial.print(F("\n trời sáng"));
      Firebase.setString(firebaseData, "/FirebaseIOT/admin/01/sttled","Ban ngày, trời sáng.");
        if (value2==0){
          Serial.print(F("\n không có trộm"));
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, HIGH);
            digitalWrite(Loa, LOW);
            digitalWrite(Light, LOW);

      Firebase.setString(firebaseData, "/FirebaseIOT/admin/01/Detection","Không có khả nghi.");
        }else if(value2 ==1){
          Serial.print(F("\n có trộmmmmmm"));
            digitalWrite(LEDR, HIGH);
            digitalWrite(LEDG, LOW);
            digitalWrite(Loa, HIGH);
            digitalWrite(Light, LOW);
          
      Firebase.setString(firebaseData, "/FirebaseIOT/admin/01/Detection","Có trộm!!!!!!!");

        } 
      }
}

//-----------------------------------------------------------------------------

int control;
void loop() {
  
     if (Firebase.ready())// khi firebase đã sẵn sàng
        {
    // thực thi chương trình
      Firebase.getFloat(firebaseData, "/FirebaseIOT/admin/01/DetectionValue");
      control = firebaseData.intData();
      {
        Serial.println(control);
        }
     if (control == 1){
  float value1= digitalRead(sensor1);
  float value2= digitalRead(sensor2);
    detection(value1,value2);
    }     
    else if (control==0)
    {      
   float value1= digitalRead(sensor1);
   detection(value1,0);
   delay(10000);
  Firebase.setFloat(firebaseData, "/FirebaseIOT/admin/01/DetectionValue",1);
      } 
  // kết thúc thực thi chương trình     
        }   
}
