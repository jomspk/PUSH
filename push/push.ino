#include <WiFi.h>
#include <FirebaseESP32.h>
//#include "Bluetooth.ino"

#define FIREBASE_HOST "https://lockkey-8986f-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "yeRNACrzwymo1a3pEEJJRTSoS2MspfjIQ835yFHG"
#define WIFI_SSID "UBIC-3D-G"
#define WIFI_PASSWORD "UbiC$mar29-"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define INPUT_PIN 32

FirebaseData firebaseData;

void setup(){
  
  Initialization();
  WiFiConnection();
  send_uuid();
  
 }

int counter = 0;
float counter2 = 0.5;

//send UUID to firebase
void send_uuid(){
  Firebase.setString(firebaseData, "/Service_UUID",SERVICE_UUID);
  Firebase.setString(firebaseData, "/Characteristic_UUID",CHARACTERISTIC_UUID);
  }
  
//Initialize buttonand Serials  
void Initialization(){
  
  Serial.begin(115200); 
  Serial.println("Start pushing button!");
  pinMode(INPUT_PIN, INPUT_PULLDOWN);
 }
 
//connect wifi
void WiFiConnection(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop(){
  int val = digitalRead(32);
  if(Firebase.setInt(firebaseData, "/ButtonStatus",val)){
    }

  if(Firebase.setString(firebaseData, "/FirebaseStatus","ACTIVE")){
    
    }
  if(Firebase.getInt(firebaseData, "/ButtonStatus")){
    if(firebaseData.dataType() == "int"){
      Serial.print("data = ");
      Serial.println(firebaseData.intData());
    }
  }

    if(Firebase.getString(firebaseData, "/FirebaseStatus")){
    if(firebaseData.dataType() == "string"){
      Serial.print("data = ");
      Serial.println(firebaseData.stringData());
    }
  }
  
}
