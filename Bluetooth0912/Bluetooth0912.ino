/*
 sendData2Iphone.ino
 https://101010.fun
*/
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define GREEN_BUTTON_PIN 32
#define RED_BUTTON_PIN 33
#define BLUE_BUTTON_PIN 27

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
BLECharacteristic* pCharacteristic;

bool green_button_flag = true;
unsigned long count_timer = 0.00;


void sendMessage(int v) {
    char str[30];
    sprintf(str, "%d", v);
    Serial.println(str);
    pCharacteristic->setValue(str);
    pCharacteristic->notify();
}

void ble_setup(){
  
    BLEDevice::init("ESP32 Bluetooth Server");//BlE環境の初期化
    BLEServer* pServer = BLEDevice::createServer();//BLEサーバーを作成している
    BLEService* pService = pServer->createService(SERVICE_UUID);//BLEserviceのインスタンス化を行っている
    pCharacteristic = pService->createCharacteristic(//Characteristicのインスタンス化を行っている
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pService->start(); //セントラル（iOS）がペリフェラルにリクエストを投げたら返事をするようにする
    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();//アドバタイズするためのオブジェクトを生成している
    pAdvertising->addServiceUUID(SERVICE_UUID);//ServiceUUIDを入れている
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // iPhone接続の問題に役立つ
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
    
  }
void setup() {
    Serial.begin(115200);

    pinMode(GREEN_BUTTON_PIN, INPUT_PULLDOWN); // 内蔵抵抗でプルアップまたは10kΩの抵抗でプルアップ
    pinMode(RED_BUTTON_PIN, INPUT_PULLDOWN); // 内蔵抵抗でプルアップまたは10kΩの抵抗でプルアップ
    pinMode(BLUE_BUTTON_PIN, INPUT_PULLDOWN); // 内蔵抵抗でプルアップまたは10kΩの抵抗でプルアップ    

    ble_setup();
}

void loop() {
  int green_button_status = digitalRead(GREEN_BUTTON_PIN);
  int red_button_status = digitalRead(RED_BUTTON_PIN);
  int blue_button_status = digitalRead(BLUE_BUTTON_PIN);
  if(green_button_status == HIGH){
    sendMessage(1);
    }
  if(red_button_status == HIGH){
    sendMessage(2);
    }
  if(blue_button_status == HIGH){
    sendMessage(3);
    }
    delay(10);
}
