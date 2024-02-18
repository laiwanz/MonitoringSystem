#include "Device.h"
#include "platform/board/board.h"
#include <unistd.h> // for sleep 
#include "cbl/cbl.h"
//using namespace netT;
#define		PACKAGE_VERSION_FILE	"/mnt/wecon/app/usr/package"
 #include "Global.h"
  
struct ProcessStats {  
    long utime; // User mode jiffies  
    long stime; // Kernel mode jiffies  
    long cutime; // User mode jiffies of children  
    long cstime; // Kernel mode jiffies of children  
    long starttime; // Start time in jiffies since system boot  
};  
  
ProcessStats getProcessStats(const std::string& sProcess) {  

    int pid = UTILITY::CState::GetProcessPIDByName(sProcess);
    std::ifstream file("/proc/" + std::to_string(pid) + "/stat");  
    std::string line;  
    std::getline(file, line);  
    std::istringstream iss(line);  
  
    std::string ignore;  
    ProcessStats stats;  
    // Extract the fields we need from /proc/[pid]/stat  
    iss >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore  
        >> ignore >> ignore >> ignore >> ignore >> ignore >> stats.utime >> stats.stime >> stats.cutime  
        >> stats.cstime >> ignore;  
  
    // The starttime field is actually the 22nd field, but we've already extracted 19 values  
    iss >> ignore >> ignore >> stats.starttime;  
  
    return stats;  
}  
  
double calculateCpuUsage(const ProcessStats& stats1, const ProcessStats& stats2, const long& elapsedJiffies) {  
    long totalJiffies = stats2.utime - stats1.utime + stats2.stime - stats1.stime;  
    totalJiffies += stats2.cutime - stats1.cutime + stats2.cstime - stats1.cstime;  
  
    // Calculate the CPU usage percentage  
    // elapsedJiffies should be calculated as the difference in jiffies between the two readings  
    return (totalJiffies / (double)elapsedJiffies) * 100.0;  
}  
  
double getCalculateCpuUsage(const std::string &sProcess) {  
   // const std::string pid = "YOUR_PID_HERE"; // Replace with the actual PID  
   // cbl::CTimeUtil timeUtil;
    ProcessStats stats1 = getProcessStats(sProcess); 
   // long lTimeStart = timeUtil.getTickCount();
    sleep(1); // Wait for a second before taking the second sample  
    ProcessStats stats2 = getProcessStats(sProcess);  
   // long lTimeEnd = timeUtil.getTickCount();
    // Calculate the number of jiffies elapsed between the two readings  
    // You might need to read /proc/timer_list or use sysconf(_SC_CLK_TCK) to get the number of jiffies per second  
    long elapsedJiffies = sysconf(_SC_CLK_TCK); // This is not correct, but you need to find the elapsed jiffies  
  //  long elapsedJiffies = lTimeEnd - lTimeStart;
    // Calculate the CPU usage  
    double cpuUsage = calculateCpuUsage(stats1, stats2, elapsedJiffies);  
  
    std::cout << "CPU usage for process " << sProcess << ": " << cpuUsage << "%" << std::endl;  
  
    return cpuUsage;  
}

Device::Device(const string& sHost, int nPort, const string& sAuth): 
        RedisMaster(sHost, nPort, sAuth), m_nInterval(1000),stopThread(false), m_bConnected(false) {

}
Device::~Device(){
}

int Device::run(){
    m_PubSub.config([this](const string& sBuffer){
        return this->m_Parser.parseRedisMessage(sBuffer);
    },   [this](string &sRespondInfo, const string &sDeviceId, bool bACK){
        return this->m_Parser.newACK(sRespondInfo, sDeviceId, bACK);
    });
    this->m_Parser.config([this](const REDIS_TASK_INFO_T &taskInfo){
            return this->m_Task.push(taskInfo);
    } , [this](const REDIS_REPORT_INFO_T &reportInfo){
            return 0;
    } );
    this->m_Task.config([this](const string& sChannel){
        return this->addAChannel(sChannel);
    } ,NULL);
    return 0;
}


int Device::report(){
    while (1)
    {
        if(g_bExit){
            break;
        }

        if(!m_bConnected){
            std::cout << "Waiting for Redis Server to start..." << std::endl;
            if(pingAndConnected()){
                std::cout << "Redis Server is online. Connecting..." << std::endl;
                this->start();
                m_bConnected = true;
            }
        }else{
            if(!pingAndConnected()){
                std::cout << "Redis Server is offline. Waiting for it to come back online..." << std::endl;
                m_bConnected = false;
            }
        }
        if(pingAndConnected()){
            string sReportInfo;
            this->grtMachineInfo(sReportInfo);
            this->publish("ReportToServer", sReportInfo);
            std::this_thread::sleep_for(std::chrono::milliseconds(m_nInterval));
        }
    }
    
    return 0;
}

int Device::setReportInterval(int interval){
    m_nInterval = interval;
    return 0;
}

bool Device::pingAndConnected(){
    int result = this->ping();
    return (result == 0) && this->isConnected();
}

int Device::start(){
    this->init();
    this->setCliConfig("");
    std::cout << "+++++++++++++++++" << std::endl;
    netT::getNetInfoCallBack([this](netT::NETINFO &stNetInfo, void *lpParameter) {

		return CallBack_NetInfo(stNetInfo);
	}, nullptr, true, 5000, true);

    return 0;
}
int Device::setCliConfig(const string& sFile){

    this->addAChannel("TaskFromServer");
    this->addAChannel("OrderFromServer");
    this->addAChannel("RespondToServer");
    
    // PLATFORM::CPlatBoard m_Board;
    // PLATFORM::DEVICE_BOARD_TYPE_T type = m_Board.getType();
    // if (type == PLATFORM::DEVICE_BOARD_TYPE_T::A8_FLASH_BOARD_v2 || type == PLATFORM::DEVICE_BOARD_TYPE_T::A8_FLASH_BOARD_3N_v2 || 
    //     type == PLATFORM::DEVICE_BOARD_TYPE_T::A8_EMMC_BOARD_v2){
    //     this->addAChannel("A8PLUS");
    //     return 0;
    // } else if (type == PLATFORM::DEVICE_BOARD_TYPE_T::A9_EMMC_BOARD_v2){
    //     this->addAChannel("A9");
    //     return 0;
    // } else if (type == PLATFORM::DEVICE_BOARD_TYPE_T::A7_FLASH_BOARD_v2){
    //     this->addAChannel("A7");
    //     return 0;
    // } else if (type == PLATFORM::DEVICE_BOARD_TYPE_T::RK3308B_FLASH64Mmem_BOARD_v2 || type == PLATFORM::DEVICE_BOARD_TYPE_T::RK3308B_FLASH_BOARD_v2 || 
    //     type == PLATFORM::DEVICE_BOARD_TYPE_T::rk3308b_eMMC_v2 || type == PLATFORM::DEVICE_BOARD_TYPE_T::RK3308B_FLASH64Mmem_IN_v2 ||  
    //     type == PLATFORM::DEVICE_BOARD_TYPE_T::rk3308b_eMMC_MD_v2 || type == PLATFORM::DEVICE_BOARD_TYPE_T::RK3308B_FLASH64Mmembox_BOARD_v2 ){
    //     this->addAChannel("RK3308");
    //     return 0;
    // } else if (type == PLATFORM::DEVICE_BOARD_TYPE_T::A8_FLASH_BOARD_v2 || type == PLATFORM::DEVICE_BOARD_TYPE_T::A8_FLASH_BOARD_3N_v2 || 
    //     type == PLATFORM::DEVICE_BOARD_TYPE_T::A8_EMMC_BOARD_v2){
    //     this->addAChannel("A8PLUS");
    //     return 0;
    // } else if (type == PLATFORM::DEVICE_BOARD_TYPE_T::ARM9_FLASH_2G_BOARD || type == PLATFORM::DEVICE_BOARD_TYPE_T::ARM9_FLASH_4G_BOARD){
    //     this->addAChannel("ARM9");
    //     return 0;
    // } else if (type == PLATFORM::DEVICE_BOARD_TYPE_T::SSD212_FLASH64Mmem_v2){
    //     this->addAChannel("SSD");
    //     return 0;
    // }


    return 0;
}

bool Device::isConnected(){

    return this->m_PubSub.isConnected();
}


int Device::ping(){

    return this->m_PubSub.ping();
}

int Device::grtMachineInfo(string& sReportInfo){
    std::string sIP;
    if(m_sCurrentNetworkCardDeviceName.empty()){
            netT::getNetInfoCallBack([this](netT::NETINFO &stNetInfo, void *lpParameter) {

            return CallBack_NetInfo(stNetInfo);
        }, nullptr, true, 5000, true);
    }

    if (getNicIp(m_sCurrentNetworkCardDeviceName, sIP) < 0) {
        cerr << "cant get the IP " << endl;
    }

    std::string sMac;
    if (getNicMac(m_sCurrentNetworkCardDeviceName, sMac) < 0) {
        cerr << "cant get the MAC " << endl;
    };

    SECURE::WMachine m_Machine;
    std::string sModel;
    if (!m_Machine.GetHMIType(sModel)) {
        cerr << "cant get the HMI Model " << endl;
    }

    std::string sDeviceId;
    if (m_Machine.getDeviceId(sDeviceId) < 0) {
        cerr << "cant get the HMI DeviceId " << endl;
    }

    std::string sVersion;
    if (cbl::loadFromFile(PACKAGE_VERSION_FILE, sVersion) <= 0) {

		sVersion = "0.0.0.22011801";
	}

    double dCpu;
   // UTILITY::CState::GetCpuPercent(nCpu);
    dCpu = getCalculateCpuUsage("HMIUI");
    unsigned int nTotalMemory, nFreeMemory;
    UTILITY::CState::GetMemoryStatus(nTotalMemory, nFreeMemory);
 
    unsigned int nTotalSize, nDiskSize;
    string sPath = "/mnt/data/";
    if (UTILITY::CState::GetDiskFreeSize(sPath, nTotalSize, nDiskSize)) {
        cerr << "cant get the Memory " << endl;
    }

    std::string sProcessName = "HMIUI";
    int nPID = UTILITY::CState::GetProcessPIDByName(sProcessName);
    int nNumber; 
    if(UTILITY::CState::GetProcessUsedFileHandle(sProcessName, nNumber) < 0){

    };
    unsigned long ulCurMem, ulPeakMem;
    if(nPID != 0) {
        UTILITY::CState::GetProcessMemByPID(nPID, ulCurMem, ulPeakMem);
    }

    m_Parser.newReportInfo(sReportInfo, "ReportToServer", sIP, sMac, sModel, sDeviceId, sVersion, dCpu,
                            nTotalMemory, nFreeMemory, nTotalSize, nDiskSize, nNumber, ulCurMem, 
                            ulPeakMem, this->m_Task.isRun(), this->m_Task.TaskListSize());
    return 0;
}


int Device::CallBack_NetInfo(netT::NETINFO &stNetInfo) {

	m_sCurrentNetworkCardDeviceName = stNetInfo.sDeviceName;

    cout << "+++++++++++++++++" << m_sCurrentNetworkCardDeviceName <<  endl;
	return 0;
}