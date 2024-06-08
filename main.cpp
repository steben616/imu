#include <stdio.h>
#include <imu/bno055.h>
#include <mag/hmc5883l.h>

void run_imu();
void run_hmc58883l();

int main() {
    stdio_init_all();
    sleep_ms(3000);
    run_imu();
    //run_hmc58883l();
}

void run_imu() {
    imu::bno55 imu(16, 17);
    while (true) {
        u8 sysStatus;
        bno055_get_sys_calib_stat(&sysStatus);
        u8 gyroStatus;
        bno055_get_gyro_calib_stat(&gyroStatus);
        u8 accStatus;
        bno055_get_accel_calib_stat(&accStatus);
        u8 magStatus;
        bno055_get_mag_calib_stat(&magStatus);
        printf("calibration: system %u, gyro %u, acc %u, mag %u\n", sysStatus, gyroStatus, accStatus, magStatus);
        bno055_accel_float_t accel;
        bno055_convert_float_accel_xyz_msq(&accel);
        printf("accel_x: %3.2f,   accel_y: %3.2f,   accel_z: %3.2f\n", accel.x, accel.y, accel.z);
        bno055_euler_float_t euler;
        bno055_convert_float_euler_hpr_deg(&euler);
        printf("heading: %3.2f,   pitch: %3.2f,   roll: %3.2f\n\n", euler.h, euler.p, euler.r);
        sleep_ms(200);
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
