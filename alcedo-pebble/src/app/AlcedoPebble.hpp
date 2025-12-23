#pragma once

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

    void init();
    void loop();

    DisplayManager& getDisplayManager() { return *displayManager; }
    HardwareManager& getHardwareManager() { return *hardwareManager; }
    PowerManager& getPowerManager() { return *powerManager; }
    BleManager& getBleManager() { return *bleManager; }
    SceneManager& getSceneManager() { return *sceneManager; }

private:
    AlcedoPebble();
    ~AlcedoPebble();
    AlcedoPebble(const AlcedoPebble&) = delete;
    AlcedoPebble& operator=(const AlcedoPebble&) = delete;

    std::unique_ptr<DisplayManager> displayManager;
    std::unique_ptr<HardwareManager> hardwareManager;
    std::unique_ptr<PowerManager> powerManager;
    std::unique_ptr<BleManager> bleManager;
    std::unique_ptr<SceneManager> sceneManager;
};