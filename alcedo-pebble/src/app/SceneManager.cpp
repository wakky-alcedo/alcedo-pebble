#include "SceneManager.hpp"

// SceneManagerのコンストラクタとデストラクタ
SceneManager::SceneManager() {
    // currentScene は最初は nullptr
}

SceneManager::~SceneManager() {
    // currentScene のデストラクタが自動で呼ばれる
}

// ※ changeScene, update, getCurrentScene は
//   高速化とテンプレートのために SceneManager.hpp にインライン実装されています．