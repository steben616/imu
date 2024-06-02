#include <stdio.h>
#include <hmc5883l.h>

int main() {
    stdio_init_all();
    mag::HMC5883L compass(20, 21);
    compass.setDeclination(4); // Stuttgart
    //compass.calibrate(2000);
    while (true) {
        auto heading = compass.getHeading();
        printf("heading: %f\n", heading);
        sleep_ms(700);
    }
}

