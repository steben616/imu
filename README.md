### HMC5883L 3D magnetometer header-only library for pico c/c++ sdk;

- Suported magnetometers HMC5883L(mag), LSM303DLHC(mag+accl). Same code works for both; see chip images below.
- Clone this repository at the same level as other examples, i.e., under the pico-examples directory.
- Edit the root CMakeLists.txt of the pico-examples repository and include this repository in the build using add_subdirectory(hmc5883l).
- Build the examples as usual using cmake --build ..
- Change the magnetic declination using setDeclination to match the value for your geographic location.
- Calibrate the magnetometer first using the calibrate function.
- Copy the resulting UF2 file onto the Pico and use PuTTY to check the heading values.
- Reported heading can be cross-verified using a floating magnetized needle or something equivalent.
- The heading should vary from 0 degrees (true geographic north) all the way to 360 degrees (one full x-y plane rotation), and then wrap back to 0.
