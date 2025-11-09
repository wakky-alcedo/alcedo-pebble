/**
 * @file display_manager.h
 * @brief ディスプレイ(LovyanGFX)とGUIライブラリ(LVGL)の初期化と管理を行います。
 */
#pragma once

#include <LovyanGFX.hpp>

// static LGFX lcd;

/**
 * @brief ディスプレイとLVGLのドライバを初期化します。
 * @details LCDの初期化、LVGLの基本設定、描画バッファの確保、
 * ディスプレイおよびタッチドライバのLVGLへの登録を行います。
 */
void display_init();

/**
 * @brief LVGLのメインループ処理。
 * @details この関数をメインループ内で定期的に呼び出すことで、
 * UIの描画やイベント処理が実行されます。
 */
void display_handler();
