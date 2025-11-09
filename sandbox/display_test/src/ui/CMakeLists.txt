SET(SOURCES ui_HomeScreen.c
    ui_RawScreen.c
    ui_AnalyticsScreen.c
    ui_GraphScreen.c
    ui_SettingScreen.c
    ui_DebugScreen.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_events.c)

add_library(ui ${SOURCES})
