#pragma once

// すべてのマネージャヘッダをインクルード
#include "main.h"

#include "hal/DisplayManager.hpp"
#include "hal/HardwareManager.hpp"
#include "hal/PowerManager.hpp"
#include "hal/BleManager.hpp"
#include "app/SceneManager.hpp"

// --- シングルトンクラス AlcedoPebble ---
class AlcedoPebble {
public:
    static AlcedoPebble& getInstance() {
        static AlcedoPebble instance; 
        return instance;
    }
    AlcedoPebble(const AlcedoPebble&) = delete;
    AlcedoPebble& operator=(const AlcedoPebble&) = delete;
    AlcedoPebble(AlcedoPebble&&) = delete;
    AlcedoPebble& operator=(AlcedoPebble&&) = delete;

    void init();
    void loop();

    // 各マネージャへのアクセサ
    DisplayManager& getDisplayManager();
    HardwareManager& getHardwareManager();
    PowerManager& getPowerManager();
    BleManager& getBleManager();
    SceneManager& getSceneManager();

private:
    AlcedoPebble();
    ~AlcedoPebble();

    std::unique_ptr<DisplayManager> displayManager;
    std::unique_ptr<HardwareManager> hardwareManager;
    std::unique_ptr<PowerManager> powerManager;
    std::unique_ptr<BleManager> bleManager;
    std::unique_ptr<SceneManager> sceneManager;
};