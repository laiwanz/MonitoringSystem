#include "Device.h"
#include "platform/board/board.h"


#define		PACKAGE_VERSION_FILE	"/mnt/wecon/app/usr/package"

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
    if (getNicIp("eth0", sIP) < 0) {
        cerr << "cant get the IP " << endl;
    }

    std::string sMac;
    if (getNicMac("eth0", sMac) < 0) {
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

    int nCpu;
    UTILITY::CState::GetCpuPercent(nCpu);

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

    m_Parser.newReportInfo(sReportInfo, "ReportToServer", sIP, sMac, sModel, sDeviceId, sVersion, nCpu,
                            nTotalMemory, nFreeMemory, nTotalSize, nDiskSize, nNumber, ulCurMem, 
                            ulPeakMem, this->m_Task.isRun(), this->m_Task.TaskListSize());
    return 0;
}