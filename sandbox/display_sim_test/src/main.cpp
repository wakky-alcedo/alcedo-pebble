#include <lvgl.h>
// #include <win_drv.h> // v8.3ドライバのWindowsヘッダ
#include <SDL.h>
#include <sdl/sdl.h>
// #include <unistd.h> // SDL_Delayを使うので不要

/**
 * @brief 最小限のテストUI
 */
static void create_minimal_ui(void)
{
    // アクティブなスクリーンを取得
    lv_obj_t * scr = lv_scr_act();

    // ラベルを作成
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "Minimal Test OK");

    // ラベルを中央に配置
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

/**
 * @brief メイン関数 (SDL対応)
 */
int main(int argc, char *argv[])
{
    (void)argc; // 未使用引数の警告回避
    (void)argv; // 未使用引数の警告回避

    // 1. LVGLのコアを初期化
    printf("Initializing LVGL...\n");
    lv_init();

    // 2. LVGL用のSDLドライバを初期化
    // (ウィンドウ作成，描画バッファ設定，ドライバ登録)
    printf("Initializing SDL driver...\n");
    sdl_init();

    // 3. 最小限のUIを作成
    printf("Creating minimal UI...\n");
    create_minimal_ui();

    printf("Entering main loop...\n");

    // 4. LVGLのタスクハンドラをループで呼び出す
    while (1) {
        // LVGLの内部タイマーやタスクを処理
        lv_timer_handler();

        // 5ms待機
        SDL_Delay(5);
    }

    return 0;
}