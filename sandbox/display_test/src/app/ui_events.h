/**
 * @file ui_events.h
 * @brief SquareLine Studioで作成したUIのイベントハンドラを定義します。
 */
#pragma once
#include <vector>
#include <lvgl.h>

/**
 * @brief ホーム画面のログ一覧リストのボタンオブジェクトを保持します。
 */
// extern std::vector<lv_obj_t*> log_list_buttons;

/**
 * @brief UIイベントの初期化を行います。
 * @details 各UIオブジェクトにイベントハンドラ関数を関連付けます。
 * SquareLine Studio側でイベントを設定した場合は、手動での呼び出しは不要な場合があります。
 */
void ui_events_init();



// #ifdef __cplusplus
// extern "C" {
// #endif

// void DropdownStartupScreenChange(lv_event_t * e);

// #ifdef __cplusplus
// } /*extern "C"*/
// #endif