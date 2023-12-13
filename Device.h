#if !defined(DEVICE_H__20231011)
#define DEVICE_H__20231011
#include "RedisMaster.h"
#include "secure/secure.h"
#include "utility/utility.h"
#include <thread>
#include <chrono>

class Device : public RedisMaster {
public:
    Device(const string& sHost, int nPort, const string& sAuth = string());
    ~Device();

public:

    int setReportInterval(int interval);
    
    int run();

    int start();

    bool isConnected();

    int ping();

    bool pingAndConnected();

    int report();

private:

    int setCliConfig(const string& sFile);

    int grtMachineInfo(string& sReportInfo);
    

private:

    int m_nInterval;
    std::atomic_bool stopThread;  
    std::condition_variable cv;  
    std::mutex mtx;  

    bool m_bConnected;
};




#endif //DEVICE_H__20231011
