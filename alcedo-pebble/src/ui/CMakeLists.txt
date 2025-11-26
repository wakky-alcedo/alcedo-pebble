SET(SOURCES ui_comp_image1.c
    ui_comp.c
    ui_MainScreen.c
    ui_MenuScreen.c
    ui_QrScreen.c
    ui_SettingScreen.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    ui_img_river_png.c
    ui_img_wether_png.c
    ui_img_notify_png.c
    ui_img_lucid_origin_a_cute_chibi_kingfisher_creature_as_a_digital_pet_0_png.c
    ui_img_qr3_png.c
    ui_img_setting2_png.c
    ui_img_twitter_qr_png.c)

add_library(ui ${SOURCES})
