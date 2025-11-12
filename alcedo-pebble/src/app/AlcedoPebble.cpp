#include "scenes/MainScene.hpp" // 最初のシーンとして指定
#include "AlcedoPebble.hpp"

// --- AlcedoPebble ---

AlcedoPebble::AlcedoPebble() {
    // コンストラクタでマネージャクラスを生成
    displayManager = std::make_unique<DisplayManager>();
    hardwareManager = std::make_unique<HardwareManager>();
    powerManager = std::make_unique<PowerManager>();
    bleManager = std::make_unique<BleManager>();
    sceneManager = std::make_unique<SceneManager>();
}

AlcedoPebble::~AlcedoPebble() {
    // デストラクタ（通常は呼ばれない）
}

void AlcedoPebble::init() {
	Serial0.println("AlcedoPebble init...");

    // 初期化は順序が重要
    
    // 1. ハードウェア（IMU,vibration）
    hardwareManager->init();
    
    // 2. ディスプレイ（LVGL, SLS UI）
    // (LGFXの初期化もここで行われる)
    displayManager->init();
    
    // 3. 電源管理（バッテリーADCにHWManagerが必要）
    powerManager->init(hardwareManager.get());

    // 4. BLE
    bleManager->init();

    // 5. 最初のシーンを設定
    // (※注: この時点で MainScene が最初のシーンとして起動する)
    sceneManager->changeScene<MainScene>();

	Serial0.println("AlcedoPebble init complete.");
}

void AlcedoPebble::loop() {
    // 毎フレームの更新処理
    // 順序が重要
    
    // 1. ハードウェア入力（センサー）を更新
    hardwareManager->update();

    // 2. BLEイベント処理
    bleManager->update();

    // 3. 電源管理（アイドル検知，電圧監視）
    powerManager->update();

    // 4. 現在のシーンのロジックを実行
    sceneManager->update();

    // 5. ディスプレイ（LVGL）の描画処理
    // (※必ず全てのロジックの最後に呼ぶ)
    displayManager->update();
}

// --- アクセサ（Getter）の実装 ---

DisplayManager& AlcedoPebble::getDisplayManager() {
    return *displayManager;
}
HardwareManager& AlcedoPebble::getHardwareManager() {
    return *hardwareManager;
}
PowerManager& AlcedoPebble::getPowerManager() {
    return *powerManager;
}
BleManager& AlcedoPebble::getBleManager() {
    return *bleManager;
}
SceneManager& AlcedoPebble::getSceneManager() {
    return *sceneManager;
}