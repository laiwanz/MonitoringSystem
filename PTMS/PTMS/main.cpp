#include <iostream>
#include "Device.h"
#include <atomic>
#include <condition_variable>
#include <csignal>
using namespace std;
using namespace cbl;


int main(int argc, char const *argv[])
{
	if(argc < 3){
		printf("param numbers is need more than 2, eg: 192.168.55.189 6379");
		return -1;
	}
		
    Device device(argv[1], atoi(argv[2]));
    device.start();
    device.run();
    std::thread reportThread(&Device::report, &device);
    reportThread.join();
    return 0;
}
