/**
 * CPUに余計な負荷をかけず，センサーのデータが準備できた最適なタイミングで効率よくデータを取得するサンプルコード
 * QMI8658のデータレディ割り込みピンを使用して，データが準備できたことを検出します。
 * データレディ割り込みが発生したときにフラグを設定し，メインループでフラグを監視してデータを取得します。
 */
#include <Arduino.h>
#include <Wire.h>
#include "SensorQMI8658.hpp"

SensorQMI8658 qmi;

#define SENSOR_SDA 6
#define SENSOR_SCL 7
#define SENSOR_INT1 4
#define SENSOR_INT2 3

IMUdata acc;
IMUdata gyr;

IMUdata maxAcc = {0.0f, 0.0f, 0.0f};
IMUdata maxGyr = {0.0f, 0.0f, 0.0f};

bool interruptFlag = false;

void setFlag(void) {
    interruptFlag = true;
}

// Callback when data is locked
void lockingMechanismHandler() {
    if (qmi.getAccelerometer(acc.x, acc.y, acc.z)) {
        Serial0.print("\x1B[H");
        Serial0.print("{ACCEL: ");
        Serial0.print(acc.x);
        Serial0.print(",");
        Serial0.print(acc.y);
        Serial0.print(",");
        Serial0.print(acc.z);
        Serial0.print("}");
    } else {
        Serial0.print("Error reading accelerometer data");
    }
    
    Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア
    Serial0.print("\n");     // 2行目へ改行

    if (acc.x < maxAcc.x) maxAcc.x = acc.x;
    if (acc.y < maxAcc.y) maxAcc.y = acc.y;
    if (acc.z > maxAcc.z) maxAcc.z = acc.z;
    Serial0.print("Max Accel X:");
    Serial0.print(maxAcc.x);
    Serial0.print(" Y:");
    Serial0.print(maxAcc.y);
    Serial0.print(" Z:");
    Serial0.print(maxAcc.z);

    Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア
    Serial0.print("\n");

    if (qmi.getGyroscope(gyr.x, gyr.y, gyr.z)) {
        Serial0.print("{GYRO: ");
        Serial0.print(gyr.x);
        Serial0.print(",");
        Serial0.print(gyr.y );
        Serial0.print(",");
        Serial0.print(gyr.z);
        Serial0.println("}");
    } else {
        Serial0.print("Error reading gyroscope data");
    }

    Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア
    Serial0.print("\n");     // 改行

    if (gyr.x > maxGyr.x) maxGyr.x = gyr.x;
    if (gyr.y > maxGyr.y) maxGyr.y = gyr.y;
    if (gyr.z > maxGyr.z) maxGyr.z = gyr.z;
    Serial0.print("Max Gyro X:");
    Serial0.print(maxGyr.x);
    Serial0.print(" Y:");
    Serial0.print(maxGyr.y);
    Serial0.print(" Z:");
    Serial0.print(maxGyr.z);
    
    Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア
    Serial0.print("\n");     // 改行

    Serial0.print(qmi.getTimestamp());
    Serial0.print("  ");
    Serial0.print(qmi.getTemperature_C());
    Serial0.println("*C");

    Serial0.print("\x1B[K"); // カーソル位置から行末までをクリア
}


void beginPower()
{
    // T_BEAM_S3_SUPREME The PMU voltage needs to be turned on to use the sensor
#if defined(ARDUINO_T_BEAM_S3_SUPREME)
    XPowersAXP2101 power;
    power.begin(Wire1, AXP2101_SLAVE_ADDRESS, 42, 41);
    power.disableALDO1();
    power.disableALDO2();
    delay(250);
    power.setALDO1Voltage(3300); power.enableALDO1();
    power.setALDO2Voltage(3300); power.enableALDO2();
#endif
}

void setup() {
    Serial0.begin(115200);
    while (!Serial0);

    beginPower();

    bool ret = false;
    ret = qmi.begin(Wire, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);

    if (!ret) {
        Serial0.println("Failed to find QMI8658 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }
    /* Get chip id*/
    Serial0.print("Device ID:");
    Serial0.println(qmi.getChipID(), HEX);

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

    //Enable Locking Mechanism
    qmi.enableLockingMechanism();

    // Set locking data event callback
    qmi.setDataLockingEventCallBack(lockingMechanismHandler);


    // Use interrupt .
    // QMI8658 interrupt always outputs low level by default,
    // and the interrupt is triggered when the rising edge
    pinMode(SENSOR_INT2, INPUT_PULLUP);
    attachInterrupt(SENSOR_INT2, setFlag, RISING);

    // qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_1); //no use
    // Enable data ready to interrupt pin2
    qmi.enableINT(SensorQMI8658::INTERRUPT_PIN_2);


    // Print register configuration information
    qmi.dumpCtrlRegister();

    Serial0.println("Read data now...");
}


void loop() {
    if (interruptFlag) {
        interruptFlag = false;
        qmi.update();
    }
    delay(100);
}

