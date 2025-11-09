// #include <lvgl.h>
// #include <ui/ui.h>
// #include <stdio.h>

// void SwitchLightClicked(lv_event_t * e) {

// }

// void SliderBrightnessChanged(lv_event_t * e) {
//     // スライダーの値を取得し，明るさラベルを変更する
//     lv_obj_t * slider = lv_event_get_target(e);
//     int value = lv_slider_get_value(slider);

//     // ラベルオブジェクトを取得
//     lv_obj_t * label = ui_LabelBrightness; // Label1 を SquareLine Studio で設定した名前に置き換え
//     char buf[8];
//     snprintf(buf, sizeof(buf), "%d", value); // スライダーの値を文字列に変換

//     lv_label_set_text(label, buf); // ラベルのテキストを更新
// }

// void SliderColorChanged(lv_event_t * e)
// {
//     // スライダーの値を取得し，明るさラベルを変更する
//     lv_obj_t * slider = lv_event_get_target(e);
//     int value = lv_slider_get_value(slider);

//     // ラベルオブジェクトを取得
//     lv_obj_t * label = ui_LabelColor; // Label1 を SquareLine Studio で設定した名前に置き換え
//     char buf[8];
//     snprintf(buf, sizeof(buf), "%d", value); // スライダーの値を文字列に変換

//     lv_label_set_text(label, buf); // ラベルのテキストを更新
// }

// void ButtonTempUpClicked(lv_event_t * e)
// {
// 	// ui_LabelTemp の値を取得し，1増やして設定する
//     lv_obj_t * lavel = ui_LabelTemp;
//     int value = atoi(lv_label_get_text(lavel));
//     value++;
//     char buf[8];
//     snprintf(buf, sizeof(buf), "%d", value);
//     lv_label_set_text(lavel, buf);
// }

// void ButtonTempDownClicked(lv_event_t * e)
// {
// 	// ui_LabelTemp の値を取得し，1減らして設定する
//     lv_obj_t * lavel = ui_LabelTemp;
//     int value = atoi(lv_label_get_text(lavel));
//     value--;
//     char buf[8];
//     snprintf(buf, sizeof(buf), "%d", value);
//     lv_label_set_text(lavel, buf);
// }

// void SliderOpenChanged(lv_event_t * e)
// {
//     // スライダーの値を取得し，明るさラベルを変更する
//     lv_obj_t * slider = lv_event_get_target(e);
//     int value = lv_slider_get_value(slider);

//     // ラベルオブジェクトを取得
//     lv_obj_t * label = ui_LabelOpenSlider; // Label1 を SquareLine Studio で設定した名前に置き換え
//     char buf[8];
//     snprintf(buf, sizeof(buf), "%d", value); // スライダーの値を文字列に変換

//     lv_label_set_text(label, buf); // ラベルのテキストを更新
// }

// void ButtonOpenClicked(lv_event_t * e)
// {
// 	// Your code here
// }

// void ButtonStopClicked(lv_event_t * e)
// {
// 	// Your code here
// }

// void ButtonCloseClicked(lv_event_t * e)
// {
// 	// Your code here
// }
