SET(SOURCES ui_comp_image1.c
    ui_comp.c
    ui_QrScreen.c
    ui_SettingsScreen.c
    ui_HomeScreen.c
    ui_SkipScreen.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_img_twitter_qr_png.c
    ui_img_275677306.c
    ui_img_1321182874.c
    ui_img_qr3_png.c
    ui_img_larger_stone_icon_png.c
    ui_img_setting2_png.c)

add_library(ui ${SOURCES})
