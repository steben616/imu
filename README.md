### hmc5883L 3D magnetometer header only library for raspberry pi pico c/c++ sdk;

- clone this repo at the same level as other examples i.e. under the pico-examples directory.
- edit the root CMakeLists.txt of the pico-examples repo and include this repo in the build using "add_subdirectory(hmc5883L)"
- build the examples as usual using "cmake --build ."
- change the magnetic declination using setDeclination to match the value for your geographic location.
- calibrate the magnetometer first using calibrate function.
- copy the resulting uf2 file on pico and use putty to check the values.
- reported heading values can be cross checked using a floating magnetized needed or something equivalent.
- heading should vary from 0 deg due true geographic north all the way 360 degrees on one full x-y plane rotation and then wrap back to 0.
