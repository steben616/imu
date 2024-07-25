#ifndef BNO55_H
#define BNO55_H

#include <cstring>
#include <iostream>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

extern "C" {
    #include <imu/bno055/bno055.h>
}

namespace imu {

void pico_delay_ms(u32 msec);
int8_t pico_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t* reg_data, uint8_t len);
int8_t pico_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t* reg_data, uint8_t len);

struct bno55 {

    struct bno055_t bno;

    bno55(int sda, int scl, uint freq = 400*1000) {
        i2c_init(i2c0, freq);
        gpio_set_function(sda, GPIO_FUNC_I2C);
        gpio_set_function(scl, GPIO_FUNC_I2C);
        gpio_pull_up(sda);
        gpio_pull_up(scl);
        bno.bus_read = pico_i2c_read;
        bno.bus_write = pico_i2c_write;
        bno.delay_msec = pico_delay_ms;
        bno.dev_addr = BNO055_I2C_ADDR1; // check jumper
        bno055_init(&bno);
        sleep_ms(100);
        auto failed = bno055_set_power_mode(BNO055_POWER_MODE_NORMAL);
        if (!failed) {
            sleep_ms(100);
            failed = bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
            if (failed) {
                std::cout << "bno055_set_operation_mode failed" << std::endl;
            }
            sleep_ms(100);
        } else {
            std::cout << "bno055_set_power_mode failed" << std::endl;
        }
    }

    auto getEulerAngles() {
        bno055_euler_float_t euler;
        bno055_convert_float_euler_hpr_deg(&euler);
        return std::make_tuple(euler.h, euler.p, euler.r);
    }

    auto getQuaternion() {
        // quaternion
        // bno055_read_quaternion_wxyz();
        return 0.0;
    }

    auto getCalibrationStatus() {
        u8 sys;
        bno055_get_sys_calib_stat(&sys);
        u8 gyro;
        bno055_get_gyro_calib_stat(&gyro);
        u8 accl;
        bno055_get_accel_calib_stat(&accl);
        u8 mag;
        bno055_get_mag_calib_stat(&mag);
        return std::make_tuple(gyro, accl, mag, sys);
    }
};

inline int8_t pico_i2c_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t* reg_data, uint8_t len) {
    uint8_t buf[16] = {0};
    buf[0] = reg_addr;
    memcpy(buf + 1, reg_data, len);
    auto rc = i2c_write_blocking(i2c0, dev_addr, buf, len + 1, false);
    if (rc == PICO_ERROR_GENERIC) {
        return BNO055_ERROR;
    }
    return BNO055_SUCCESS;
}

inline int8_t pico_i2c_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t* reg_data, uint8_t len) {
    auto rc = i2c_write_blocking(i2c0, dev_addr, &reg_addr, 1, true);
    if (rc == PICO_ERROR_GENERIC) {
        return 1;
    }
    rc = i2c_read_blocking(i2c0, dev_addr, reg_data, len, false);
    if (rc == PICO_ERROR_GENERIC) {
        return BNO055_ERROR;
    }
    return BNO055_SUCCESS;
}

inline void pico_delay_ms(u32 msec) {
    sleep_ms(msec);
}

}

#endif