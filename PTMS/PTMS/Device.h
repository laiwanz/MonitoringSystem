#if !defined(DEVICE_H__20231011)
#define DEVICE_H__20231011
#include "RedisMaster.h"
#include "secure/secure.h"
#include "utility/utility.h"
#include <thread>
#include <chrono>
#include "netT/netT.h"

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
    int CallBack_NetInfo(netT::NETINFO &stNetInfo);

private:

    int m_nInterval;
    std::atomic_bool stopThread;  
    std::condition_variable cv;  
    std::mutex mtx;  
    std::string m_sCurrentNetworkCardDeviceName;
    bool m_bConnected;
};




#endif //DEVICE_H__20231011
