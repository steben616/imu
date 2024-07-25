#include <stdio.h>
#include <iomanip>
#include <iostream>

#include <imu/bno055.h>
#include <imu/bno085.h>
#include <mag/hmc5883l.h>

void run_bno055();
void run_bno085();
void run_hmc58883l();

int main() {
    stdio_init_all();
    sleep_ms(5000);
    run_bno085();
    //run_bno055();
    //run_hmc58883l();
}

void run_bno085() {
    imu::bno85 imu(16, 17);
    auto rc = imu.init_i2c_hal();
    if (!rc) return;
    rc = imu.enableReports();
    if (!rc) return;
    rc = imu.enableCalibration();
    if (!rc) return;
    while (true) {
        sleep_ms(100);
        sh2_service();
        if (imu.hasReset()) {
            imu.enableCalibration();
            imu.enableReports();
        }
    }
}

void run_bno055() {
    imu::bno55 imu(16, 17);
    while (true) {
        // calibration
        auto [gyro, accl, mag, sys] = imu.getCalibrationStatus();
        std::cout << "{\"cal_gyro\":" << unsigned(gyro) << ", \"cal_acc\":" << unsigned(accl)
            << ", \"cal_mag\":" << unsigned(mag) << ", \"cal_sys\":" << unsigned(sys) << "}\n";
        // accl
        //bno055_accel_float_t accel;
        //bno055_convert_float_accel_xyz_msq(&accel);
        //std::cout << std::fixed << std::setprecision(2);
        //std::cout << "{\"acc_x\":" << accel.x << ", \"acc_y\":" << accel.y << ", \"acc_z\":" << accel.z << "}\n";
        //euler
        auto [h, p, r] = imu.getEulerAngles();
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "{\"y\":" << h << ", \"p\":" << p << ", \"r\":" << r << "}\n";
        sleep_ms(100);
    }
}

void run_hmc58883l() {
    mag::HMC5883L compass(20, 21);
    compass.setDeclination(4);
    //compass.calibrate(2000);
    while (true) {
        auto heading = compass.getHeading();
        printf("heading: %f\n", heading);
        sleep_ms(700);
    }
}
