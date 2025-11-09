#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// サービスUUID (適当なUUIDに変更可能)
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// キャラクタリスティックUUID (時刻書き込み用)
#define TIME_CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// 書き込みイベントを処理するコールバッククラス
class TimeCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        // データ（バイト配列）を取得
        std::string value = pCharacteristic->getValue();
        
        if (value.length() >= 4) {
            // 受信したバイトデータをuint32_t型のUNIX時間として解釈
            // (Android側からリトルエンディアンで4バイト送信される想定)
            uint32_t unixTime = 0;
            memcpy(&unixTime, value.c_str(), 4);

            Serial0.print("受信したUNIX時間: ");
            Serial0.println(unixTime);

            // TODO: ここでESP32の内部時刻(RTC)を設定する処理などを追加
            // 例: timeval tv = { (long)unixTime, 0 };
            // settimeofday(&tv, NULL);
        }
    }
};

void setup() {
    Serial0.begin(115200);
    Serial0.println("BLE サーバーを起動します...");

    // BLEデバイスの初期化
    BLEDevice::init("ESP32-S3 TimeReceiver"); // BLEアドバタイズ名

    // BLEサーバーの作成
    BLEServer *pServer = BLEDevice::createServer();
    // サービスの作成
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // キャラクタリスティックの作成（クライアントからの書き込み(WRITE)を許可）
    BLECharacteristic *pTimeCharacteristic = pService->createCharacteristic(
        TIME_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );

    // キャラクタリスティックにコールバックを設定
    pTimeCharacteristic->setCallbacks(new TimeCharacteristicCallbacks());

    // サービスを開始
    pService->start();

    // アドバタイジング（スマホから見えるように）を開始
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    Serial0.println("アドバタイジング開始．スマートフォンからの接続を待機中...");
}

void loop() {
    // BLEは別タスクで動作するため，loopは空でも良い
    delay(2000);
}