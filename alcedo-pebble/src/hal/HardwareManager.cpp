#include "HardwareManager.hpp"

#include "main.h"

// ピン定義
// バッテリーADCピン
#define BATT_ADC_PIN 1

// IMUセンサーI2Cピン定義
#define SENSOR_I2C Wire
#define SENSOR_SDA 6
#define SENSOR_SCL 7
#define SENSOR_INT1 4
#define SENSOR_INT2 3

// バイブレーションモーターI2Cピン定義
#define VIBE_I2C Wire1
#define VIBRATE_SDA 15
#define VIBRATE_SCL 16

HardwareManager::HardwareManager() {}
HardwareManager::~HardwareManager() {}

void HardwareManager::init() {
    Serial0.println("HardwareManager init...");

    // バイブモーターピン設定
    VIBE_I2C.begin(VIBRATE_SDA, VIBRATE_SCL);
	if (vibeMotor.begin(&VIBE_I2C)) {
		Serial0.println("Found DRV2605");
	} else {
		Serial0.println("Failed to find DRV2605");
		// while (1);
	}
	vibeMotor.useLRA(); // LRAモーターを使用
	vibeMotor.selectLibrary(1); // ライブラリ1を選択
	vibeMotor.init(); // 初期化
	vibeMotor.setMode(DRV2605_MODE_INTTRIG); // 内部トリガーモード

	// --------------------------------------------------------------------------------

    // IMU (QMI8658) の初期化 (スタブ)
    bool ret = imuSensor.begin(SENSOR_I2C, QMI8658_L_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL);
    if (!ret) {
        Serial0.println("Failed to find QMI8658 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }

    Serial0.print("Device ID:");
    Serial0.println(imuSensor.getChipID(), HEX);

    if (imuSensor.selfTestAccel()) {
        Serial0.println("Accelerometer self-test successful");
    } else {
        Serial0.println("Accelerometer self-test failed!");
    }

    if (imuSensor.selfTestGyro()) {
        Serial0.println("Gyroscope self-test successful");
    } else {
        Serial0.println("Gyroscope self-test failed!");
    }

    imuSensor.configAccelerometer(
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

    imuSensor.configGyroscope(
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
    imuSensor.enableGyroscope();
    imuSensor.enableAccelerometer();

    // Print register configuration information
    imuSensor.dumpCtrlRegister();

#if IMU_IRQ > 0
// If you want to enable interrupts, then turn on the interrupt enable
    imuSensor.enableINT(SensorQMI8658::INTERRUPT_PIN_1, true);
    imuSensor.enableINT(SensorQMI8658::INTERRUPT_PIN_2, false);
#endif

	// --------------------------------------------------------------------------------
    
    // バッテリーADCピン設定
    pinMode(BATT_ADC_PIN, INPUT);

    Serial0.println("HardwareManager init complete.");
}

void HardwareManager::update() {
    // 毎フレーム呼ばれ，センサー値をポーリングする

    // 1. IMUデータの読み取り (スタブ)
    imuSensor.getAccelerometer(currentImu.accelX, currentImu.accelY, currentImu.accelZ);
	imuSensor.getGyroscope(currentImu.gyroX, currentImu.gyroY, currentImu.gyroZ);
	switch (SCREEN_ROTATION) {
		case 0:
			{
				float tem = currentImu.accelX;
				currentImu.accelX = currentImu.accelY;
				currentImu.accelY = -tem;
			}
			break;
		case 1:
			{
				currentImu.accelX = -currentImu.accelX;
				currentImu.accelY = -currentImu.accelY;
			}
			break;
		case 2:
			{
				float tem = currentImu.accelX;
				currentImu.accelX = -currentImu.accelY;
				currentImu.accelY = tem;
			}
			break;
		case 3:
			{
				// そのまま
			}
			break;
	}
}

/**
 * @brief バイブレーションを実行する
 * @param effect 効果音の番号 (DRV2605の効果音ライブラリに準拠)
 */
void HardwareManager::vibrate(uint8_t effect) {
	if (!vibrateEnabled) return;
    vibeMotor.setWaveform(0, effect);  // set the effect to play
    vibeMotor.setWaveform(1, 0);       // end waveform sequence

    vibeMotor.go(); // play the effect!
}

ImuData HardwareManager::getImuData() {
    return currentImu;
}

float HardwareManager::getAccelX() {
    return currentImu.accelX;
}

float HardwareManager::getAccelY() {
    return currentImu.accelY;
}

uint16_t HardwareManager::readBatteryAdc() {
    return analogRead(BATT_ADC_PIN);
}