#include <iostream>
#include "Device.h"
#include <atomic>
#include <condition_variable>
#include <csignal>
using namespace std;
using namespace cbl;


int main(int argc, char const *argv[])
{

    Device device("192.168.55.189", 6379);
    device.start();
    device.run();
    std::thread reportThread(&Device::report, &device);
    reportThread.join();
    return 0;
}
