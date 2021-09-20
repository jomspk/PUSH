#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Ticker.h>//タイマー割り込み用

/*基本属性定義*/
#define DEVICE_NAME "ESP32" //デバイス名
#define SPI_SPEED 115200　//SPI通信速度
#define GREEN_BUTTON_PIN 32
#define RED_BUTTON_PIN 33
#define BLUE_BUTTON_PIN 27

/*UUID定義*/
#define SERVICE_UUID           "28b0883b-7ec3-4b46-8f64-8559ae036e4e"  // サービスのUUID
#define CHARACTERISTIC_UUID_TX "2049779d-88a9-403a-9c59-c7df79e1dd7c" 

/*通信制御用*/
BLECharacteristic *pCharacteristicTX; //送信用キャラクタリスティック
bool deviceDisconnected = false; //デバイスの切断状態
bool deviceConnected = false; //デバイスの接続状態
int send_value = 0; //iOSに送る値

/*プッシュボタン*/
int buttonState = LOW; //状態
int lastButtonState = LOW; //直前の状態
unsigned long lastDebounceTime = 0; //直前の切り替え時刻
unsigned long debounceDelay = 50; //デバウンスタイム(mSec.)

/*タイマー制御用*/
Ticker ticker;
bool bReadyTicker = false;
const float iIntervalTime = 0.1; //計測間隔(1秒)

//接続・切断時コールバック
class funcServerCallbacks: public BLEServerCallbacks{
  void onConnect(BLEServer* pServer){
    Serial.println("Connected");
    deviceConnected = true;
    deviceDisconnected = false;
    }
  void onDisconnect(BLEServer* pServer){
    deviceConnected = false;
    deviceDisconnected = true;
    }
};

//タイマー割り込み関数
static void kickRoutine(){
  bReadyTicker = true; //割り込んでいいのかのフラグ、コールバック関数内でブロッキングI/O処理できないから
}

/*初期化処理*/
void doInitialize(){
  Serial.begin(115200);
  pinMode(GREEN_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(RED_BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(BLUE_BUTTON_PIN, INPUT_PULLDOWN);
}

/*準備処理*/
void doPrepare(BLEService *pService){
  //Notify用のキャラクタリスティックを作成する
  pCharacteristicTX = pService->createCharacteristic(
    CHARACTERISTIC_UUID_TX,
    BLECharacteristic::PROPERTY_NOTIFY
    );
  pCharacteristicTX->addDescriptor(new BLE2902());  
}

/*主処理ロジック*/
void doMainProcess(){
  //緑のボタンのステータスを読む
  char str[30];
  int green_state = digitalRead(GREEN_BUTTON_PIN);
  int red_state = digitalRead(RED_BUTTON_PIN);
  int blue_state = digitalRead(BLUE_BUTTON_PIN);
  if(green_state){
    send_value = 1;  
  }
  if(red_state){
    send_value = 2;  
  }
  if(blue_state){
    send_value = 3;  
  }
  sprintf(str,"%d",send_value);
  Serial.println(str);
  pCharacteristicTX->setValue(str);
  pCharacteristicTX->notify();  
}

void setup() {
  // 初期化処理を行ってBLEデバイスを初期化する
  doInitialize();
  BLEDevice::init(DEVICE_NAME);

  //Serverオブジェクトを作成してコールバックを設定する
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new funcServerCallbacks());

  //Serviceオブジェクトを作成して準備処理を実行する
  BLEService *pService = pServer->createService(SERVICE_UUID);
  doPrepare(pService);

  //サービスを開始して、SERVICE_UUIDでアドバタイジングを開始する
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  //タイマー割り込みを開始する
  ticker.attach(iIntervalTime, kickRoutine);
  Serial.println("Waiting to connect...");
}

void loop() {
  // 接続が確立されていれば
  if(deviceConnected){
    //タイマー割り込みによって主処理を実行する
    if(bReadyTicker){
      doMainProcess();
      bReadyTicker = false;
    }
  }
  if(deviceDisconnected){
    ESP.restart();
    delay(10000);   
  } 
}
