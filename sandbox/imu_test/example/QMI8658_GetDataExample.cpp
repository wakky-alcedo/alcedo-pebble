/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      QMI8658_GetDataExample.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2022-10-16
 *
 */
/**
 * QMI8658から加速度とジャイロデータを取得するサンプルコード
 * I2CまたはSPIインターフェースを使用してQMI8658センサーから加速度とジャイロデータを取得します。
 * esp32-s3ボード向けにピン設定を変更済み
 */
#include <Arduino.h>
#include "SensorQMI8658.hpp"

SensorQMI8658 qmi;

#define SENSOR_SDA 6
#define SENSOR_SCL 7
#define SENSOR_INT1 4
#define SENSOR_INT2 3

IMUdata acc;
IMUdata gyr;

void setup() {
    // put your setup code here, to run once:
    Serial0.begin(115200);
    // while (!Serial0) {
    //   ; // wait for Serial0 port to connect. Needed for native USB
    // }
    Serial0.println("IMU Test Starting");

    bool ret = qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);
    if (!ret) {
        Serial0.println("Failed to find QMI8658 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }

    Serial0.print("Device ID:");
    Serial0.println(qmi.getChipID(), HEX);

    if (qmi.selfTestAccel()) {
        Serial0.println("Accelerometer self-test successful");
    } else {
        Serial0.println("Accelerometer self-test failed!");
    }

    if (qmi.selfTestGyro()) {
        Serial0.println("Gyroscope self-test successful");
    } else {
        Serial0.println("Gyroscope self-test failed!");
    }

    qmi.configAccelerometer(
        /*
         * ACC_RANGE_2G
         * ACC_RANGE_4G
         * ACC_RANGE_8G
         * ACC_RANGE_16G
         * */
        SensorQMI8658::ACC_RANGE_4G,
        /*
         * ACC_ODR_1000H
         * ACC_ODR_500Hz
         * ACC_ODR_250Hz
         * ACC_ODR_125Hz
         * ACC_ODR_62_5Hz
         * ACC_ODR_31_25Hz
         * ACC_ODR_LOWPOWER_128Hz
         * ACC_ODR_LOWPOWER_21Hz
         * ACC_ODR_LOWPOWER_11Hz
         * ACC_ODR_LOWPOWER_3H
        * */
        SensorQMI8658::ACC_ODR_1000Hz,
        /*
        *  LPF_MODE_0     //2.66% of ODR
        *  LPF_MODE_1     //3.63% of ODR
        *  LPF_MODE_2     //5.39% of ODR
        *  LPF_MODE_3     //13.37% of ODR
        *  LPF_OFF        // OFF Low-Pass Fitter
        * */
        SensorQMI8658::LPF_MODE_0);




    qmi.configGyroscope(
        /*
        * GYR_RANGE_16DPS
        * GYR_RANGE_32DPS
        * GYR_RANGE_64DPS
        * GYR_RANGE_128DPS
        * GYR_RANGE_256DPS
        * GYR_RANGE_512DPS
        * GYR_RANGE_1024DPS
        * */
        SensorQMI8658::GYR_RANGE_64DPS,
        /*
         * GYR_ODR_7174_4Hz
         * GYR_ODR_3587_2Hz
         * GYR_ODR_1793_6Hz
         * GYR_ODR_896_8Hz
         * GYR_ODR_448_4Hz
         * GYR_ODR_224_2Hz
         * GYR_ODR_112_1Hz
         * GYR_ODR_56_05Hz
         * GYR_ODR_28_025H
         * */
        SensorQMI8658::GYR_ODR_896_8Hz,
        /*
        *  LPF_MODE_0     //2.66% of ODR
        *  LPF_MODE_1     //3.63% of ODR
        *  LPF_MODE_2     //5.39% of ODR
        *  LPF_MODE_3     //13.37% of ODR
        *  LPF_OFF        // OFF Low-Pass Fitter
        * */
        SensorQMI8658::LPF_MODE_3);

    /*
    * If both the accelerometer and gyroscope sensors are turned on at the same time,
    * the output frequency will be based on the gyroscope output frequency.
    * The example configuration is 896.8HZ output frequency,
    * so the acceleration output frequency is also limited to 896.8HZ
    * */
    qmi.enableGyroscope();
    qmi.enableAccelerometer();

    // Print register configuration information
    qmi.dumpCtrlRegister();

#if IMU_IRQ > 0
// If you want to enable interrupts, then turn on the interrupt enable
    qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_1, true);
    qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_2, false);
#endif

    Serial0.println("Read data now...");

    delay(1000);
    
    Serial0.print("\x1B[2J"); // 画面全体をクリア

}

void loop() {
    // When the interrupt pin is passed in through setPin,
    // the GPIO will be read to see if the data is ready.
    if (qmi.getDataReady()) {

        // Serial0.print("Timestamp:");
        // Serial0.print(qmi.getTimestamp());

        Serial0.print("\x1B[H");

        if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {

            // Print to Serial0 plotter
            Serial0.print("ACCEL.x:"); Serial0.print(acc.x); Serial0.print(",");
            Serial0.print("ACCEL.y:"); Serial0.print(acc.y); Serial0.print(",");
            Serial0.print("ACCEL.z:"); Serial0.print(acc.z); Serial0.println();

            /*
            m2/s to mg
            Serial0.print(" ACCEL.x:"); Serial0.print(acc.x * 1000); Serial0.println(" mg");
            Serial0.print(",ACCEL.y:"); Serial0.print(acc.y * 1000); Serial0.println(" mg");
            Serial0.print(",ACCEL.z:"); Serial0.print(acc.z * 1000); Serial0.println(" mg");
            */
        } else {
            // データを取得できなかった場合のプレースホルダー
            Serial0.print("ACCEL: (Waiting for data...)"); 
        }

        // 3. 1行目の行末までをクリアし，改行する
        Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア
        Serial0.print("\n");     // 2行目へ改行

        if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
            // Print to Serial0 plotter
            Serial0.print("GYRO.x:"); Serial0.print(gyr.x); Serial0.print(",");
            Serial0.print("GYRO.y:"); Serial0.print(gyr.y); Serial0.print(",");
            Serial0.print("GYRO.z:"); Serial0.print(gyr.z); Serial0.println();

            // Serial0.print(" GYRO.x:"); Serial0.print(gyr.x); Serial0.println(" degrees/sec");
            // Serial0.print(",GYRO.y:"); Serial0.print(gyr.y); Serial0.println(" degrees/sec");
            // Serial0.print(",GYRO.z:"); Serial0.print(gyr.z); Serial0.println(" degrees/sec");
        } else {
            // データを取得できなかった場合のプレースホルダー
            Serial0.print("GYRO: (Waiting for data...)");
        }

        Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア

        // Serial0.print("Temperature:");
        // Serial0.print(qmi.getTemperature_C());
        // Serial0.println(" degrees C");

    }
    delay(100);
}

