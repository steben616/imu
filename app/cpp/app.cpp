#include <wss.h>
#include <serialib.h>

#include <thread>
#include <iostream>

int main(int argc, char *argv[]) {
    wss_server ws;
    auto t = std::thread([&](){ws.run_server();});
    auto port = argv[1];
    serialib serial;
    char errorOpening = serial.openDevice(port, 115200);
    if (errorOpening!=1) return errorOpening;
    while (true) {
        char buffer[256] = {0};
        serial.readString(buffer, '\n', 256, 5000);
        printf("%s", buffer);
        ws.send_data(buffer);
    }
    serial.closeDevice();
    t.join();
    return 0 ;
}