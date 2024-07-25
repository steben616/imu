#ifndef BNO85_H
#define BNO85_H

#include <iostream>

extern "C" {
    #include <imu/sh2/sh2_err.h>
    #include <imu/sh2/sh2_SensorValue.h>
}

namespace imu {

bool reset_occurred = false;
constexpr uint8_t BNO085_I2C_ADDR = 0x4A;
sh2_SensorValue_t sensor_value;
sh2_SensorEvent_t sensor_event;
u8 accStatus, gyroStatus, magStatus;

bool hal_reset();
uint32_t getTimeUs(sh2_Hal_t *self);
void hal_callback(void *cookie, sh2_AsyncEvent_t *pEvent);
void sensorHandler(void *cookie, sh2_SensorEvent_t *event);

int i2c_open(sh2_Hal_t *self);
void i2c_close(sh2_Hal_t *self);
int i2c_write(sh2_Hal_t *self, uint8_t *buffer, unsigned len);
int i2c_read(sh2_Hal_t *self, uint8_t *buffer, unsigned len, uint32_t *t_us);

struct bno85{

    sh2_Hal_t _sh2_hal;
    sh2_ProductIds_t _prodIds;

    bno85(int sda, int scl, uint freq = 400*1000) {
        i2c_init(i2c0, freq);
        gpio_set_function(sda, GPIO_FUNC_I2C);
        gpio_set_function(scl, GPIO_FUNC_I2C);
        gpio_pull_up(sda);
        gpio_pull_up(scl);
    }

    bool init_i2c_hal() {
        uint8_t dummy;
        auto rc = i2c_read_blocking(i2c0, BNO085_I2C_ADDR, &dummy, 1, false);
        if(rc < 1){
            std::cout << "i2c_read_blocking dummy test failed" << std::endl;
            return false;
        }
        std::cout << "i2c_read_blocking dummy test successful: " << rc << std::endl;
        if (!hal_reset()) return false;
        _sh2_hal.open = i2c_open;
        _sh2_hal.close = i2c_close;
        _sh2_hal.read = i2c_read;
        _sh2_hal.write = i2c_write;
        _sh2_hal.getTimeUs = getTimeUs;
        auto status = sh2_open(&_sh2_hal, hal_callback, NULL);
        if (status != SH2_OK) {
            std::cout << "sh2_open failed" << std::endl;
            return false;
        }
        memset(&_prodIds, 0, sizeof(_prodIds));
        status = sh2_getProdIds(&_prodIds);
        if (status != SH2_OK) {
            std::cout << "sh2_getProdIds failed" << std::endl;
            return false;
        }
        for (int n = 0; n < _prodIds.numEntries; n++) { // sh2.h line 60
            std::cout << "Part: " << _prodIds.entry[n].swPartNumber << std::endl;
            std::cout << "Version: " << _prodIds.entry[n].swVersionMajor << "." <<
                _prodIds.entry[n].swVersionMinor << "." << _prodIds.entry[n].swVersionPatch << std::endl;
            std::cout << "Build: " << _prodIds.entry[n].swBuildNumber << std::endl;
        }
        status = sh2_setSensorCallback(sensorHandler, NULL);
        if (status != SH2_OK) {
            std::cout << "sh2_setSensorCallback failed" << std::endl;
            return false;
        }
        return true;
    }

    bool enableCalibration() {
        auto status = sh2_setCalConfig(SH2_CAL_ACCEL|SH2_CAL_GYRO|SH2_CAL_MAG);
        std::cout << "sh2_setCalConfig " << status << std::endl;
        return (status == SH2_OK);
    }

    bool enableReports() {
        if (!enableReport(SH2_ACCELEROMETER, 5)) {
            return false;
        }
        if (!enableReport(SH2_ROTATION_VECTOR, 10)) {
            return false;
        }
        if (!enableReport(SH2_GYROSCOPE_CALIBRATED, 5)) {
            return false;
        }
        if (!enableReport(SH2_MAGNETIC_FIELD_CALIBRATED, 5)) {
            return false;
        }
        return true;
    }

    bool enableReport(sh2_SensorId_t sensorId, float frequency) {
        uint32_t interval_us = (uint32_t)(1000000 / frequency);
        static sh2_SensorConfig_t config;
        config.sensorSpecific = 0;
        config.batchInterval_us = 0;
        config.wakeupEnabled = false;
        config.changeSensitivity = 0;
        config.alwaysOnEnabled = false;
        config.reportInterval_us = interval_us;
        config.changeSensitivityEnabled = false;
        config.changeSensitivityRelative = false;
        auto status = sh2_setSensorConfig(sensorId, &config);
        if (status != SH2_OK) {
            std::cout << "sh2_setSensorConfig failed " << sensorId << std::endl;
        }
        return (status == SH2_OK);
    }

    bool hasReset(void) {
        bool x = reset_occurred;
        reset_occurred = false;
        if (x) std::cout << "imu was reset" << std::endl;
        return x;
    }
};

inline bool hal_reset() {
    //Figure 1-27: 1 – reset
    uint8_t soft_reset_pkt[5] = { 5, 0, 1, 0, 1};
    for (int i = 0; i < 5; i++) {
        auto rc = i2c_write_blocking(i2c0, BNO085_I2C_ADDR, soft_reset_pkt, 5, false);
        if (rc == 5) {
            return true;
        } else {
            std::cout << "hal software reset failed " << rc << std::endl;
            sleep_ms(50);
        }
    }
    sleep_ms(300);
    return false;
}

// callback for reset events and other non-sensor events received from SH-2 sensor hub
inline void hal_callback(void *cookie, sh2_AsyncEvent_t *pEvent) {
    if (pEvent->eventId == SH2_RESET) {
        imu::reset_occurred = true;
        std::cout << "hal_callback SH2_RESET" << std::endl;
    }
}

inline void sensorHandler(void *cookie, sh2_SensorEvent_t *event) {
    sensor_event = *event;
    auto status = sh2_decodeSensorEvent(&sensor_value, &sensor_event);
    if (status != SH2_OK) {
        std::cout << "sh2_decodeSensorEvent failed" << std::endl;
        return;
    }
    bool cal_event = false;
    switch (sensor_event.reportId) {
        // The absolute rotation vector provides an orientation output that is
        // expressed as a quaternion referenced to magnetic north and gravity
        // game rotation vector output aligns the quaternion output to an arbitrary orientation
        case SH2_ROTATION_VECTOR:
            std::cout << "{" <<
                "\"i\":" << sensor_value.un.rotationVector.i << ", " <<
                "\"j\":" << sensor_value.un.rotationVector.j << ", " <<
                "\"k\":" << sensor_value.un.rotationVector.k << ", " <<
                "\"r\":" << sensor_value.un.rotationVector.real << "}\n";
            break;
        case SH2_ACCELEROMETER:
            cal_event = true;
            accStatus = (sensor_event.report[2] & 0x03);
            break;
        case SH2_GYROSCOPE_CALIBRATED:
            cal_event = true;
            gyroStatus = (sensor_event.report[2] & 0x03);
            break;
        case SH2_MAGNETIC_FIELD_CALIBRATED:
            cal_event = true;
            magStatus = (sensor_event.report[2] & 0x03);
            break;
    }
    if (cal_event) {
        // std::cout << "{" <<
        //     "\"cal_gyro\":" << unsigned(gyroStatus) << ", " <<
        //     "\"cal_acc\":" << unsigned(accStatus) << ", " <<
        //     "\"cal_mag\":" << unsigned(magStatus) << "}\n";
    }
}

inline int i2c_open(sh2_Hal_t *self) {
    std::cout << "i2c_open" << std::endl;
    if (!hal_reset()) return 1;
    return 0;
}

inline void i2c_close(sh2_Hal_t *self) {
    std::cout << "i2c_close" << std::endl;
}

inline int i2c_read(sh2_Hal_t *self, uint8_t *buffer, unsigned len, uint32_t *t_us) {
    *t_us = to_us_since_boot(get_absolute_time());
    uint8_t shtp_header[4]; // DS: 1.3.1 SHTP
    auto rc = i2c_read_blocking(i2c0, BNO085_I2C_ADDR, shtp_header, 4, false);
    if(rc != 4){
        std::cout << "i2c_read shtp_header: " << rc << std::endl;
        return 0;
    }
    uint16_t length = (shtp_header[1] << 8) |  shtp_header[0];
    length &= 0x7FFF;
    // DEBUG_PRINT("shtp_header 0 : 0x%02x, 0b%08b\r\n", shtp_header[0], shtp_header[0]);
    // DEBUG_PRINT("shtp_header 1 : 0x%02x, 0b%08b\r\n", shtp_header[1], shtp_header[1]);
    // DEBUG_PRINT("shtp_header 2 : 0x%02x, 0b%08b\r\n", shtp_header[2], shtp_header[2]);
    // DEBUG_PRINT("shtp_header 3 : 0x%02x, 0b%08b\r\n", shtp_header[3], shtp_header[3]);
    // DEBUG_PRINT("length %d\r\n", length);
    if(length > len){
        std::cout << "i2c_read shtp_header length " << length << ", len " << len << std::endl;
        return 0;
    }
    rc = i2c_read_blocking(i2c0, BNO085_I2C_ADDR, buffer, length, false);
    if(rc != length) {
        std::cout << "i2c_read_blocking buffer rc " << rc << ", length " << length << std::endl;
        return 0;
    }
    return length;
}

inline int i2c_write(sh2_Hal_t *self, uint8_t *buffer, unsigned len) {
    uint16_t length = (len > SH2_HAL_MAX_TRANSFER_OUT) ? SH2_HAL_MAX_TRANSFER_OUT : len;
    auto rc = i2c_write_blocking(i2c0, BNO085_I2C_ADDR, buffer, length, false);
    //std::cout << "i2c_write " << rc << std::endl;
    if(rc != length) return 0;
    return rc;
}

inline uint32_t getTimeUs(sh2_Hal_t *self) {
    return to_us_since_boot(get_absolute_time());
}

}

#endif