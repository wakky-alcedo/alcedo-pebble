#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>

// Current Time Service (CTS) の標準UUID
static BLEUUID serviceUUID("0x1805");
// Current Time キャラクタリスティックの標準UUID
static BLEUUID charUUID("0x2A2B");

static boolean doConnect = false;       // 接続フラグ
static boolean connected = false;       // 接続状態フラグ
static BLEAdvertisedDevice* myDevice;   // 接続対象のBLEデバイス

// スキャン結果を処理するコールバッククラス
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    // スキャンでデバイスが見つかるたびに呼ばれる
    void onResult(BLEAdvertisedDevice advertisedDevice) {
        // 見つかったデバイスが "Current Time Service (0x1805)" を
        // アドバタイズしているかチェック
        if (advertisedDevice.isAdvertisingService(serviceUUID)) {
            Serial.print("CTSサービスを発見: ");
            Serial.println(advertisedDevice.toString().c_str());

            // スキャンを停止（最初に見つかったデバイスに接続試行）
            BLEDevice::getScan()->stop();

            // 接続対象のデバイス情報をコピー
            // (advertisedDeviceオブジェクトはこの関数の終了後に破棄されるため)
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true; // メインループで接続処理へ
        }
    }
};

/**
 * @brief "Current Time"キャラクタリスティックのデータを解析してシリアル出力
 *
 * CTS (0x2A2B) のデータ構造 (10バイト) に基づいてパース
 * - [0-1] Year (uint16_t, little-endian)
 * - [2]   Month (uint8_t)
 * - [3]   Day (uint8_t)
 * - [4]   Hours (uint8_t)
 * - [5]   Minutes (uint8_t)
 * - [6]   Seconds (uint8_t)
 * - [7]   Day of Week (uint8_t, 1=Monday)
 * - [8]   Fractions256 (uint8_t)
 * - [9]   Adjust Reason (uint8_t)
 */
void parseCurrentTime(std::string data) {
    if (data.length() < 7) { // 少なくとも秒までの7バイトは必要
        Serial.println("受信データが短すぎます");
        return;
    }

    const char* raw = data.c_str();

    // リトルエンディアンで年を読み込む
    uint16_t year = (uint16_t)(raw[0] | (raw[1] << 8));
    uint8_t month = (uint8_t)raw[2];
    uint8_t day = (uint8_t)raw[3];
    uint8_t hours = (uint8_t)raw[4];
    uint8_t minutes = (uint8_t)raw[5];
    uint8_t seconds = (uint8_t)raw[6];

    Serial.println("--- 時刻受信成功 ---");
    Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n",
                  year, month, day, hours, minutes, seconds);
    Serial.println("---------------------");
}

// サーバーに接続し、時刻を読み取る関数
bool connectToServer() {
    Serial.print("接続試行中: ");
    Serial.println(myDevice->getAddress().toString().c_str());

    // BLEクライアントの作成
    BLEClient* pClient = BLEDevice::createClient();

    // サーバー（スマホ）へ接続
    if (!pClient->connect(myDevice)) {
        Serial.println("接続に失敗しました");
        delete pClient; // クライアントオブジェクトを解放
        return false;
    }
    Serial.println("接続完了");

    // サービスの取得 (CTS: 0x1805)
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.print("サービス(CTS)が見つかりません: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        delete pClient;
        return false;
    }

    // キャラクタリスティックの取得 (Current Time: 0x2A2B)
    BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.print("キャラクタリスティック(Current Time)が見つかりません: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
        delete pClient;
        return false;
    }

    // キャラクタリスティックの値を読み取る
    if (pRemoteCharacteristic->canRead()) {
        std::string value = pRemoteCharacteristic->readValue();
        Serial.print("データを読み取りました (");
        Serial.print(value.length());
        Serial.println(" bytes)");

        // 読み取ったデータを解析
        parseCurrentTime(value);
    }

    // 接続を切断
    pClient->disconnect();
    // クライアントオブジェクトを解放
    delete pClient;

    Serial.println("切断しました");
    return true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("BLE クライアント (CTS) を起動します...");

    // BLEデバイスを初期化
    BLEDevice::init(""); // クライアントなので名前は不要

    // スキャンオブジェクトを取得
    BLEScan* pBLEScan = BLEDevice::getScan();
    // スキャンコールバックを設定
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // アクティブスキャンを有効化
    pBLEScan->setInterval(100);    // スキャン間隔 (ms)
    pBLEScan->setWindow(99);       // スキャンウィンドウ (ms)

    Serial.println("スキャンを開始します (CTSサービスを探します)...");
}

void loop() {
    // 接続フラグ(doConnect)が立ったら接続処理を実行
    if (doConnect) {
        doConnect = false; // フラグをリセット
        if (connectToServer()) {
            Serial.println("時刻の読み取りに成功しました．");
        } else {
            Serial.println("時刻の読み取りに失敗しました．");
        }

        // 接続試行が完了したら、接続対象デバイスの情報を破棄
        delete myDevice;
        myDevice = nullptr;

        // 30秒後に再度スキャンを開始
        Serial.println("30秒後にスキャンを再開します...");
        delay(30000);
        Serial.println("スキャンを再開します．");
        BLEDevice::getScan()->start(5, false); // 5秒間スキャン
    }

    // 最初にスキャンを開始 (setup()で開始するとCPUリセット時に問題が起きる場合があるため)
    if (!BLEDevice::getScan()->isScanning() && !doConnect && myDevice == nullptr) {
        Serial.println("スキャンを開始 (5秒間)...");
        BLEDevice::getScan()->start(5, false); // 5秒間スキャン
    }

    delay(1000); // メインループの負荷軽減
}