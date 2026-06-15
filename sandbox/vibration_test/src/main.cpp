#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_DRV2605.h"

Adafruit_DRV2605 drv;

// ピンアサイン
#define SDA_PIN 15
#define SCL_PIN 16
#define I2C_PORT Wire1

void setup() {
    Serial0.begin(115200);

    I2C_PORT.begin(SDA_PIN, SCL_PIN);

    Serial0.println("Adafruit DRV2605 Basic test");

    if (! drv.begin(&I2C_PORT)) {
        Serial0.println("Could not find DRV2605");
        while (1) delay(10);
    }

    drv.useLRA();
    drv.selectLibrary(1);

    drv.init();

    // I2C trigger by sending 'go' command 
    // default, internal trigger when sending GO command
    drv.setMode(DRV2605_MODE_INTTRIG); 
}

void loop() {
    static uint8_t effect = 1;
    Serial0.print("Effect #");
    Serial0.println(effect);

    // Play effect
    /*
    if (effect == 1) {
        Serial0.println("11.2 Waveform Library Effects List");
    }
    if (effect == 1) {
        Serial0.println(F("1 − Strong Click - 100%"));
    }
    if (effect == 2) {
        Serial0.println(F("2 − Strong Click - 60%"));
    }
    if (effect == 3) {
        Serial0.println(F("3 − Strong Click - 30%"));
    }
    if (effect == 4) {
        Serial0.println(F("4 − Sharp Click - 100%"));
    }
    if (effect == 5) {
        Serial0.println(F("5 − Sharp Click - 60%"));
    }
    if (effect == 6) {
        Serial0.println(F("6 − Sharp Click - 30%"));
    }
    if (effect == 7) {
        Serial0.println(F("7 − Soft Bump - 100%"));
    }
    if (effect == 8) {
        Serial0.println(F("8 − Soft Bump - 60%"));
    }
    if (effect == 9) {
        Serial0.println(F("9 − Soft Bump - 30%"));
    }
    if (effect == 10) {
        Serial0.println(F("10 − Double Click - 100%"));
    }
    if (effect == 11) {
        Serial0.println(F("11 − Double Click - 60%"));
    }
    if (effect == 12) {
        Serial0.println(F("12 − Triple Click - 100%"));
    }
    if (effect == 13) {
        Serial0.println(F("13 − Soft Fuzz - 60%"));
    }
    if (effect == 14) {
        Serial0.println(F("14 − Strong Buzz - 100%"));
    }
    if (effect == 15) {
        Serial0.println(F("15 − 750 ms Alert 100%"));
    }
    if (effect == 16) {
        Serial0.println(F("16 − 1000 ms Alert 100%"));
    }
    if (effect == 17) {
        Serial0.println(F("17 − Strong Click 1 - 100%"));
    }
    if (effect == 18) {
        Serial0.println(F("18 − Strong Click 2 - 80%"));
    }
    if (effect == 19) {
        Serial0.println(F("19 − Strong Click 3 - 60%"));
    }
    if (effect == 20) {
        Serial0.println(F("20 − Strong Click 4 - 30%"));
    }
    if (effect == 21) {
        Serial0.println(F("21 − Medium Click 1 - 100%"));
    }
    if (effect == 22) {
        Serial0.println(F("22 − Medium Click 2 - 80%"));
    }
    if (effect == 23) {
        Serial0.println(F("23 − Medium Click 3 - 60%"));
    }
    if (effect == 24) {
        Serial0.println(F("24 − Sharp Tick 1 - 100%"));
    }
    if (effect == 25) {
        Serial0.println(F("25 − Sharp Tick 2 - 80%"));
    }
    if (effect == 26) {
        Serial0.println(F("26 − Sharp Tick 3 – 60%"));
    }
    if (effect == 27) {
        Serial0.println(F("27 − Short Double Click Strong 1 – 100%"));
    }
    if (effect == 28) {
        Serial0.println(F("28 − Short Double Click Strong 2 – 80%"));
    }
    if (effect == 29) {
        Serial0.println(F("29 − Short Double Click Strong 3 – 60%"));
    }
    if (effect == 30) {
        Serial0.println(F("30 − Short Double Click Strong 4 – 30%"));
    }
    if (effect == 31) {
        Serial0.println(F("31 − Short Double Click Medium 1 – 100%"));
    }
    if (effect == 32) {
        Serial0.println(F("32 − Short Double Click Medium 2 – 80%"));
    }
    if (effect == 33) {
        Serial0.println(F("33 − Short Double Click Medium 3 – 60%"));
    }
    if (effect == 34) {
        Serial0.println(F("34 − Short Double Sharp Tick 1 – 100%"));
    }
    if (effect == 35) {
        Serial0.println(F("35 − Short Double Sharp Tick 2 – 80%"));
    }
    if (effect == 36) {
        Serial0.println(F("36 − Short Double Sharp Tick 3 – 60%"));
    }
    if (effect == 37) {
        Serial0.println(F("37 − Long Double Sharp Click Strong 1 – 100%"));
    }
    if (effect == 38) {
        Serial0.println(F("38 − Long Double Sharp Click Strong 2 – 80%"));
    }
    if (effect == 39) {
        Serial0.println(F("39 − Long Double Sharp Click Strong 3 – 60%"));
    }
    if (effect == 40) {
        Serial0.println(F("40 − Long Double Sharp Click Strong 4 – 30%"));
    }
    if (effect == 41) {
        Serial0.println(F("41 − Long Double Sharp Click Medium 1 – 100%"));
    }
    if (effect == 42) {
        Serial0.println(F("42 − Long Double Sharp Click Medium 2 – 80%"));
    }
    if (effect == 43) {
        Serial0.println(F("43 − Long Double Sharp Click Medium 3 – 60%"));
    }
    if (effect == 44) {
        Serial0.println(F("44 − Long Double Sharp Tick 1 – 100%"));
    }
    if (effect == 45) {
        Serial0.println(F("45 − Long Double Sharp Tick 2 – 80%"));
    }
    if (effect == 46) {
        Serial0.println(F("46 − Long Double Sharp Tick 3 – 60%"));
    }
    if (effect == 47) {
        Serial0.println(F("47 − Buzz 1 – 100%"));
    }
    if (effect == 48) {
        Serial0.println(F("48 − Buzz 2 – 80%"));
    }
    if (effect == 49) {
        Serial0.println(F("49 − Buzz 3 – 60%"));
    }
    if (effect == 50) {
        Serial0.println(F("50 − Buzz 4 – 40%"));
    }
    if (effect == 51) {
        Serial0.println(F("51 − Buzz 5 – 20%"));
    }
    if (effect == 52) {
        Serial0.println(F("52 − Pulsing Strong 1 – 100%"));
    }
    if (effect == 53) {
        Serial0.println(F("53 − Pulsing Strong 2 – 60%"));
    }
    if (effect == 54) {
        Serial0.println(F("54 − Pulsing Medium 1 – 100%"));
    }
    if (effect == 55) {
        Serial0.println(F("55 − Pulsing Medium 2 – 60%"));
    }
    if (effect == 56) {
        Serial0.println(F("56 − Pulsing Sharp 1 – 100%"));
    }
    if (effect == 57) {
        Serial0.println(F("57 − Pulsing Sharp 2 – 60%"));
    }
    if (effect == 58) {
        Serial0.println(F("58 − Transition Click 1 – 100%"));
    }
    if (effect == 59) {
        Serial0.println(F("59 − Transition Click 2 – 80%"));
    }
    if (effect == 60) {
        Serial0.println(F("60 − Transition Click 3 – 60%"));
    }
    if (effect == 61) {
        Serial0.println(F("61 − Transition Click 4 – 40%"));
    }
    if (effect == 62) {
        Serial0.println(F("62 − Transition Click 5 – 20%"));
    }
    if (effect == 63) {
        Serial0.println(F("63 − Transition Click 6 – 10%"));
    }
    if (effect == 64) {
        Serial0.println(F("64 − Transition Hum 1 – 100%"));
    }
    if (effect == 65) {
        Serial0.println(F("65 − Transition Hum 2 – 80%"));
    }
    if (effect == 66) {
        Serial0.println(F("66 − Transition Hum 3 – 60%"));
    }
    if (effect == 67) {
        Serial0.println(F("67 − Transition Hum 4 – 40%"));
    }
    if (effect == 68) {
        Serial0.println(F("68 − Transition Hum 5 – 20%"));
    }
    if (effect == 69) {
        Serial0.println(F("69 − Transition Hum 6 – 10%"));
    }
    if (effect == 70) {
        Serial0.println(F("70 − Transition Ramp Down Long Smooth 1 – 100 to 0%"));
    }
    if (effect == 71) {
        Serial0.println(F("71 − Transition Ramp Down Long Smooth 2 – 100 to 0%"));
    }
    if (effect == 72) {
        Serial0.println(F("72 − Transition Ramp Down Medium Smooth 1 – 100 to 0%"));
    }
    if (effect == 73) {
        Serial0.println(F("73 − Transition Ramp Down Medium Smooth 2 – 100 to 0%"));
    }
    if (effect == 74) {
        Serial0.println(F("74 − Transition Ramp Down Short Smooth 1 – 100 to 0%"));
    }
    if (effect == 75) {
        Serial0.println(F("75 − Transition Ramp Down Short Smooth 2 – 100 to 0%"));
    }
    if (effect == 76) {
        Serial0.println(F("76 − Transition Ramp Down Long Sharp 1 – 100 to 0%"));
    }
    if (effect == 77) {
        Serial0.println(F("77 − Transition Ramp Down Long Sharp 2 – 100 to 0%"));
    }
    if (effect == 78) {
        Serial0.println(F("78 − Transition Ramp Down Medium Sharp 1 – 100 to 0%"));
    }
    if (effect == 79) {
        Serial0.println(F("79 − Transition Ramp Down Medium Sharp 2 – 100 to 0%"));
    }
    if (effect == 80) {
        Serial0.println(F("80 − Transition Ramp Down Short Sharp 1 – 100 to 0%"));
    }
    if (effect == 81) {
        Serial0.println(F("81 − Transition Ramp Down Short Sharp 2 – 100 to 0%"));
    }
    if (effect == 82) {
        Serial0.println(F("82 − Transition Ramp Up Long Smooth 1 – 0 to 100%"));
    }
    if (effect == 83) {
        Serial0.println(F("83 − Transition Ramp Up Long Smooth 2 – 0 to 100%"));
    }
    if (effect == 84) {
        Serial0.println(F("84 − Transition Ramp Up Medium Smooth 1 – 0 to 100%"));
    }
    if (effect == 85) {
        Serial0.println(F("85 − Transition Ramp Up Medium Smooth 2 – 0 to 100%"));
    }
    if (effect == 86) {
        Serial0.println(F("86 − Transition Ramp Up Short Smooth 1 – 0 to 100%"));
    }
    if (effect == 87) {
        Serial0.println(F("87 − Transition Ramp Up Short Smooth 2 – 0 to 100%"));
    }
    if (effect == 88) {
        Serial0.println(F("88 − Transition Ramp Up Long Sharp 1 – 0 to 100%"));
    }
    if (effect == 89) {
        Serial0.println(F("89 − Transition Ramp Up Long Sharp 2 – 0 to 100%"));
    }
    if (effect == 90) {
        Serial0.println(F("90 − Transition Ramp Up Medium Sharp 1 – 0 to 100%"));
    }
    if (effect == 91) {
        Serial0.println(F("91 − Transition Ramp Up Medium Sharp 2 – 0 to 100%"));
    }
    if (effect == 92) {
        Serial0.println(F("92 − Transition Ramp Up Short Sharp 1 – 0 to 100%"));
    }
    if (effect == 93) {
        Serial0.println(F("93 − Transition Ramp Up Short Sharp 2 – 0 to 100%"));
    }
    if (effect == 94) {
        Serial0.println(F("94 − Transition Ramp Down Long Smooth 1 – 50 to 0%"));
    }
    if (effect == 95) {
        Serial0.println(F("95 − Transition Ramp Down Long Smooth 2 – 50 to 0%"));
    }
    if (effect == 96) {
        Serial0.println(F("96 − Transition Ramp Down Medium Smooth 1 – 50 to 0%"));
    }
    if (effect == 97) {
        Serial0.println(F("97 − Transition Ramp Down Medium Smooth 2 – 50 to 0%"));
    }
    if (effect == 98) {
        Serial0.println(F("98 − Transition Ramp Down Short Smooth 1 – 50 to 0%"));
    }
    if (effect == 99) {
        Serial0.println(F("99 − Transition Ramp Down Short Smooth 2 – 50 to 0%"));
    }
    if (effect == 100) {
        Serial0.println(F("100 − Transition Ramp Down Long Sharp 1 – 50 to 0%"));
    }
    if (effect == 101) {
        Serial0.println(F("101 − Transition Ramp Down Long Sharp 2 – 50 to 0%"));
    }
    if (effect == 102) {
        Serial0.println(F("102 − Transition Ramp Down Medium Sharp 1 – 50 to 0%"));
    }
    if (effect == 103) {
        Serial0.println(F("103 − Transition Ramp Down Medium Sharp 2 – 50 to 0%"));
    }
    if (effect == 104) {
        Serial0.println(F("104 − Transition Ramp Down Short Sharp 1 – 50 to 0%"));
    }
    if (effect == 105) {
        Serial0.println(F("105 − Transition Ramp Down Short Sharp 2 – 50 to 0%"));
    }
    if (effect == 106) {
        Serial0.println(F("106 − Transition Ramp Up Long Smooth 1 – 0 to 50%"));
    }
    if (effect == 107) {
        Serial0.println(F("107 − Transition Ramp Up Long Smooth 2 – 0 to 50%"));
    }
    if (effect == 108) {
        Serial0.println(F("108 − Transition Ramp Up Medium Smooth 1 – 0 to 50%"));
    }
    if (effect == 109) {
        Serial0.println(F("109 − Transition Ramp Up Medium Smooth 2 – 0 to 50%"));
    }
    if (effect == 110) {
        Serial0.println(F("110 − Transition Ramp Up Short Smooth 1 – 0 to 50%"));
    }
    if (effect == 111) {
        Serial0.println(F("111 − Transition Ramp Up Short Smooth 2 – 0 to 50%"));
    }
    if (effect == 112) {
        Serial0.println(F("112 − Transition Ramp Up Long Sharp 1 – 0 to 50%"));
    }
    if (effect == 113) {
        Serial0.println(F("113 − Transition Ramp Up Long Sharp 2 – 0 to 50%"));
    }
    if (effect == 114) {
        Serial0.println(F("114 − Transition Ramp Up Medium Sharp 1 – 0 to 50%"));
    }
    if (effect == 115) {
        Serial0.println(F("115 − Transition Ramp Up Medium Sharp 2 – 0 to 50%"));
    }
    if (effect == 116) {
        Serial0.println(F("116 − Transition Ramp Up Short Sharp 1 – 0 to 50%"));
    }
    if (effect == 117) {
        Serial0.println(F("117 − Transition Ramp Up Short Sharp 2 – 0 to 50%"));
    }
    if (effect == 118) {
        Serial0.println(F("118 − Long buzz for programmatic stopping – 100%"));
    }
    if (effect == 119) {
        Serial0.println(F("119 − Smooth Hum 1 (No kick or brake pulse) – 50%"));
    }
    if (effect == 120) {
        Serial0.println(F("120 − Smooth Hum 2 (No kick or brake pulse) – 40%"));
    }
    if (effect == 121) {
        Serial0.println(F("121 − Smooth Hum 3 (No kick or brake pulse) – 30%"));
    }
    if (effect == 122) {
        Serial0.println(F("122 − Smooth Hum 4 (No kick or brake pulse) – 20%"));
    }
    if (effect == 123) {
        Serial0.println(F("123 − Smooth Hum 5 (No kick or brake pulse) – 10%"));
    }

    // set the effect to play
    drv.setWaveform(0, effect);  // play effect 
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();

    // wait a bit
    delay(2000);

    effect++;
    if (effect > 117) effect = 1;

    //*/

    switch (effect) {
        case 1:
            Serial0.println(F("1 − Strong Click - 100%"));
            effect = 4;
            break;
        case 4:
            Serial0.println(F("4 − Sharp Click - 100%"));
            effect = 7;
            break;
        case 7:
            Serial0.println(F("7 − Soft Bump - 100%"));
            effect = 10;
            break;
        case 10:
            Serial0.println(F("10 − Double Click - 100%"));
            effect = 12;
            break;
        case 12:
            Serial0.println(F("12 − Triple Click - 100%"));
            effect = 14;
            break;
        case 14:
            Serial0.println(F("14 − Strong Buzz - 100%"));
            effect = 15;
            break;
        case 15:
            Serial0.println(F("15 − 750 ms Alert 100%"));
            effect = 16;
            break;
        case 16:
            Serial0.println(F("16 − 1000 ms Alert 100%"));
            effect = 17;
            break;
        case 17:
            Serial0.println(F("17 − Strong Click 1 - 100%"));
            effect = 21;
            break;
        case 21:
            Serial0.println(F("21 − Medium Click 1 - 100%"));
            effect = 24;
            break;
        case 24:
            Serial0.println(F("24 − Sharp Tick 1 - 100%"));
            effect = 27;
            break;
        case 27:
            Serial0.println(F("27 − Short Double Click Strong 1 – 100%"));
            effect = 31;
            break;
        case 31:
            Serial0.println(F("31 − Short Double Click Medium 1 – 100%"));
            effect = 34;
            break;
        case 34:
            Serial0.println(F("34 − Short Double Sharp Tick 1 – 100%"));
            effect = 37;
            break;
        case 37:
            Serial0.println(F("37 − Long Double Sharp Click Strong 1 – 100%"));
            effect = 41;
            break;
        case 41:
            Serial0.println(F("41 − Long Double Sharp Click Medium 1 – 100%"));
            effect = 44;
            break;
        case 44:
            Serial0.println(F("44 − Long Double Sharp Tick 1 – 100%"));
            effect = 47;
            break;
        case 47:
            Serial0.println(F("47 − Buzz 1 – 100%"));
            effect = 52;
            break;
        case 52:
            Serial0.println(F("52 − Pulsing Strong 1 – 100%"));
            effect = 54;
            break;
        case 54:
            Serial0.println(F("54 − Pulsing Medium 1 – 100%"));
            effect = 56;
            break;
        case 56:
            Serial0.println(F("56 − Pulsing Sharp 1 – 100%"));
            effect = 58;
            break;
        case 58:
            Serial0.println(F("58 − Transition Click 1 – 100%"));
            effect = 64;
            break;
        case 64:
            Serial0.println(F("64 − Transition Hum 1 – 100%"));
            effect = 70;
            break;
        case 70:
            Serial0.println(F("70 − Transition Ramp Down Long Smooth 1 – 100 to 0%"));
            effect = 71;
            break;
        case 71:
            Serial0.println(F("71 − Transition Ramp Down Long Smooth 2 – 100 to 0%"));
            effect = 72;
            break;
        case 72:
            Serial0.println(F("72 − Transition Ramp Down Medium Smooth 1 – 100 to 0%"));
            effect = 73;
            break;
        case 73:
            Serial0.println(F("73 − Transition Ramp Down Medium Smooth 2 – 100 to 0%"));
            effect = 74;
            break;
        case 74:
            Serial0.println(F("74 − Transition Ramp Down Short Smooth 1 – 100 to 0%"));
            effect = 75;
            break;
        case 75:
            Serial0.println(F("75 − Transition Ramp Down Short Smooth 2 – 100 to 0%"));
            effect = 76;
            break;
        case 76:
            Serial0.println(F("76 − Transition Ramp Down Long Sharp 1 – 100 to 0%"));
            effect = 77;
            break;
        case 77:
            Serial0.println(F("77 − Transition Ramp Down Long Sharp 2 – 100 to 0%"));
            effect = 78;
            break;
        case 78:
            Serial0.println(F("78 − Transition Ramp Down Medium Sharp 1 – 100 to 0%"));
            effect = 79;
            break;
        case 79:
            Serial0.println(F("79 − Transition Ramp Down Medium Sharp 2 – 100 to 0%"));
            effect = 80;
            break;
        case 80:
            Serial0.println(F("80 − Transition Ramp Down Short Sharp 1 – 100 to 0%"));
            effect = 81;
            break;
        case 81:
            Serial0.println(F("81 − Transition Ramp Down Short Sharp 2 – 100 to 0%"));
            effect = 82;
            break;
        case 82:
            Serial0.println(F("82 − Transition Ramp Up Long Smooth 1 – 0 to 100%"));
            effect = 83;
            break;
        case 83:
            Serial0.println(F("83 − Transition Ramp Up Long Smooth 2 – 0 to 100%"));
            effect = 84;
            break;
        case 84:
            Serial0.println(F("84 − Transition Ramp Up Medium Smooth 1 – 0 to 100%"));
            effect = 85;
            break;
        case 85:
            Serial0.println(F("85 − Transition Ramp Up Medium Smooth 2 – 0 to 100%"));
            effect = 86;
            break;
        case 86:
            Serial0.println(F("86 − Transition Ramp Up Short Smooth 1 – 0 to 100%"));
            effect = 87;
            break;
        case 87:
            Serial0.println(F("87 − Transition Ramp Up Short Smooth 2 – 0 to 100%"));
            effect = 88;
            break;
        case 88:
            Serial0.println(F("88 − Transition Ramp Up Long Sharp 1 – 0 to 100%"));
            effect = 89;
            break;
        case 89:
            Serial0.println(F("89 − Transition Ramp Up Long Sharp 2 – 0 to 100%"));
            effect = 90;
            break;
        case 90:
            Serial0.println(F("90 − Transition Ramp Up Medium Sharp 1 – 0 to 100%"));
            effect = 91;
            break;
        case 91:
            Serial0.println(F("91 − Transition Ramp Up Medium Sharp 2 – 0 to 100%"));
            effect = 92;
            break;
        case 92:
            Serial0.println(F("92 − Transition Ramp Up Short Sharp 1 – 0 to 100%"));
            effect = 93;
            // effect = 118;
            break;
        case 93:
            Serial0.println(F("93 − Transition Ramp Up Short Sharp 2 – 0 to 100%"));
            // effect = 118;
            effect = 1; // Loop back to start
            break;
        case 118:
            Serial0.println(F("118 − Long buzz for programmatic stopping – 100%"));
            effect = 1; // Loop back to start
            break;
        default:
            effect = 1; // Safety initialization
            break;
    }

    // set the effect to play
    drv.setWaveform(0, effect);  // play effect 
    drv.setWaveform(1, 0);       // end waveform

    // play the effect!
    drv.go();

    // wait a bit
    delay(1000);
}