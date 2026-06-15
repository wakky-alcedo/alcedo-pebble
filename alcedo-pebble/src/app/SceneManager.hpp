#pragma once

#include "main.h"
#include "app/Scene.hpp"
#include <memory>
#include <type_traits>

// --- SceneManagerクラス ---
class SceneManager {
public:
    SceneManager();
    ~SceneManager();

    template<typename T>
    void changeScene() {
		Serial0.println("SceneManager: Changing scene...");
        static_assert(std::is_base_of<Scene, T>::value, "T must be a descendant of Scene");
        if (currentScene) {
            currentScene->onExit();
        }
        currentScene = std::unique_ptr<T>(new T());
        currentScene->onEnter();
    }

    void update() {
        if (currentScene) {
            currentScene->update();
        }
    }

    Scene* getCurrentScene() { return currentScene.get(); }

private:
    std::unique_ptr<Scene> currentScene;
};