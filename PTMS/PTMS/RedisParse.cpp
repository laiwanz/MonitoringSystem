#include "RedisParse.h"
using namespace cbl;


RedisParse::RedisParse(){

}
    
RedisParse::~RedisParse(){

}

int RedisParse::config(FUNCTION_PUSH_TASK func, FUNCTION_SHOW_TASK func1){

    this->m_func = func;   
    this->m_func1 = func1;
    return 0;
}

int RedisParse::parseRedisMessage(const string& sMessage){
    Json::Value root;
    int nRtn = 0;
    if ((nRtn = cbl::loadJsonString(sMessage, root)) < 0){
        cout << "Error: " << " load json "<< endl;
        return -1;
    }
    
    if (root.isMember("MachineInfo")) {
        REDIS_REPORT_INFO_T reportInfo;
        if (root["MachineInfo"].isMember("Channel")){
            reportInfo.sChannel  = root["MachineInfo"]["Channel"].isString() ? root["MachineInfo"]["Channel"].asString() : "";
        }
        if (root["MachineInfo"].isMember("IP")){
            reportInfo.sIp  = root["MachineInfo"]["IP"].isString() ? root["MachineInfo"]["IP"].asString() : "";
        }
        if (root["MachineInfo"].isMember("MAC")){
            reportInfo.sMAC  = root["MachineInfo"]["MAC"].isString() ? root["MachineInfo"]["MAC"].asString() : "";
        }
        if (root["MachineInfo"].isMember("Model")){
            reportInfo.sModel  = root["MachineInfo"]["Model"].isString() ? root["MachineInfo"]["Model"].asString() : "";
        }
        if (root["MachineInfo"].isMember("DeviceId")){
            reportInfo.sDeviceId  = root["MachineInfo"]["DeviceId"].isString() ? root["MachineInfo"]["DeviceId"].asString() : "";
        }
        if (root["MachineInfo"].isMember("Version")){
            reportInfo.sDeviceId  = root["MachineInfo"]["Version"].isString() ? root["MachineInfo"]["Version"].asString() : "";
        }
        if (root["MachineInfo"].isMember("CpuPercent")) {
            reportInfo.nCpu = root["MachineInfo"]["CpuPercent"].isInt()? root["MachineInfo"]["CpuPercent"].asInt() : 0;    
        }
        if (root["MachineInfo"].isMember("TotalMemory")) {
            reportInfo.nTotalMemory = root["MachineInfo"]["TotalMemory"].isUInt()? root["MachineInfo"]["TotalMemory"].asUInt() : 0;    
        }
        if (root["MachineInfo"].isMember("FreeMemory")) {
            reportInfo.nFreeMemory = root["MachineInfo"]["FreeMemory"].isUInt()? root["MachineInfo"]["FreeMemory"].asUInt() : 0;
        }
        if (root["MachineInfo"].isMember("TotalSize")) {
            reportInfo.nTotalSize = root["MachineInfo"]["TotalSize"].isUInt()? root["MachineInfo"]["TotalSize"].asUInt() : 0;
        }
        if (root["MachineInfo"].isMember("DiskSize")) {
            reportInfo.nDiskSize = root["MachineInfo"]["DiskSize"].isUInt()? root["MachineInfo"]["DiskSize"].asUInt() : 0;
        }
        if (root["MachineInfo"].isMember("PID")) {
            reportInfo.nPID = root["MachineInfo"]["PID"].isInt()? root["MachineInfo"]["PID"].asInt() : 0;
        }
        if (root["MachineInfo"].isMember("CurentMemory")) {
            reportInfo.ulCurMem = root["MachineInfo"]["CurentMemory"].isUInt()? root["MachineInfo"]["CurentMemory"].asUInt() : 0;
        }
        if (root["MachineInfo"].isMember("PeakMemory")) {
            reportInfo.ulPeakMem = root["MachineInfo"]["PeakMemory"].isUInt()? root["MachineInfo"]["PeakMemory"].asUInt() : 0;
        }
        if (root["MachineInfo"].isMember("IsExecuting")) {
            reportInfo.bIsExecuting = root["MachineInfo"]["IsExecuting"].isBool() ? root["MachineInfo"]["IsExecuting"].asBool() : false;
        }
        if (root["MachineInfo"].isMember("TaskTimes")) {
            reportInfo.nTimes = root["MachineInfo"]["TaskTimes"].isInt()? root["MachineInfo"]["TaskTimes"].asInt() : 0;
        }
        if(m_func1 == NULL) {
            cout << "Parse error" << endl;
            return -1;
        }
        return m_func1(reportInfo);

    }else if(root.isMember("Tasks")){
        REDIS_TASK_INFO_T  taskInfo;
        if (root["Tasks"].isMember("Channel")){
            taskInfo.sChannel  = root["Tasks"]["Channel"].isString() ? root["Tasks"]["Channel"].asString() : "";
        }
        if (root["Tasks"].isMember("Model")){
            taskInfo.sModel  = root["Tasks"]["Model"].isString() ? root["Tasks"]["Model"].asString() : "";
        }
        if (root["Tasks"].isMember("Members")) {
            if(root["Tasks"]["Members"].isArray()){
                for(int i = 0; i < (int)root["Tasks"]["Members"].size(); i++){
                    taskInfo.vMembers.push_back( root["Tasks"]["Members"][i].asString());
                }
            };
        }
        if (root["Tasks"].isMember("TaskType")){    
            int taskNum = root["Tasks"]["TaskType"].asUInt();   
                    
            switch (taskNum)
            {
            case 0:
                taskInfo.TaskName = REDIS_TASK_TYPE_REBOOT;
                break;
            case 1:
                taskInfo.TaskName = REDIS_TASK_TYPE_UPDATE;
                break;
            case 2:
                taskInfo.TaskName = REDIS_TASK_TYPE_REINSTALL;
                break;
            case 3:
                taskInfo.TaskName = REDIS_TASK_TYPE_ADD_CHANNEL;
                break;
            case 4:
                taskInfo.TaskName = REDIS_TASK_TYPE_PRINT;
                break;
            case 5:
                taskInfo.TaskName = REDIS_TASK_TYPE_CLOSE;
                break;
            case 6:
                taskInfo.TaskName = REDIS_TASK_TYPE_ORDER;
                break;
            case 7:
                taskInfo.TaskName = REDIS_TASK_TYPE_BEEP_ORDER;
                break;
            case 8:
                taskInfo.TaskName = REDIS_TASK_TYPE_BLACKLIGHT_ORDER;
                break;
            default:
                taskInfo.TaskName = REDIS_TASK_TYPE_NUL;
                break;
            }
        }
        if (root["Tasks"].isMember("Time")) {
            taskInfo.nTime = root["Tasks"]["Time"].isUInt()? root["Tasks"]["Time"].asUInt() : 0;    
        }
        if (root["Tasks"].isMember("Time")) {
            taskInfo.nTime = root["Tasks"]["Time"].isUInt()? root["Tasks"]["Time"].asUInt() : 0;    
        }
        if (root["Tasks"].isMember("Interval")) {
            taskInfo.nInterval = root["Tasks"]["Interval"].isUInt()? root["Tasks"]["Interval"].asUInt() : 0;
        }
        if (root["Tasks"].isMember("Postscript")) {
            taskInfo.sPostscript = root["Tasks"]["Postscript"].isString() ? root["Tasks"]["Postscript"].asString() : "";
        }
        if(m_func == NULL) {
            cout << "Parse error" << endl;
            return -1;
        }
        return this->m_func(taskInfo);

    }else if (root.isMember("Group")){
        REDIS_ADD_CHANNEL_INFO_T groupInfo;
        if (root["Group"].isMember("Channel")) {
            groupInfo.sChannel = root["Group"]["Channel"].isString() ? root["Group"]["Channel"].asString() : "";    
        }
        if (root["Group"].isMember("Members")) {
            if(root["Group"]["Members"].isArray()){
                for(int i = 0; i < (int)root["Group"]["Members"].size(); i++){
                    groupInfo.vMembers.push_back( root["Group"]["Members"][i].asString() );
                }
            };
        }
    }

    return -2;
};


int RedisParse::praseJsonFile(const string& nFile){
    Json::Value root;
    REDIS_TASK_INFO_T  taskInfo;
    int nRtn = 0;
    if ((nRtn = cbl::loadJsonFile(nFile, root)) < 0){
        cout << "Error: " << " load json "<< endl;
        return -1;
    }
    if (!root.isMember("Tasks")) {
        return -2;
    }
    if (root["Tasks"].isMember("Channel")){
        taskInfo.sChannel  = root["Tasks"]["Channel"].isString() ? root["Tasks"]["Channel"].asString() : "";
    }
    if (root["Tasks"].isMember("Model")){
        taskInfo.sModel  = root["Tasks"]["Model"].isString() ? root["Tasks"]["Model"].asString() : "";
    }
    if (root["Tasks"].isMember("TaskType")){    
        int taskNum = root["Tasks"]["TaskType"].asUInt();   
                
        switch (taskNum)
        {
        case 0:
            taskInfo.TaskName = REDIS_TASK_TYPE_REBOOT;
            break;
        case 1:
            taskInfo.TaskName = REDIS_TASK_TYPE_UPDATE;
            break;
        case 2:
            taskInfo.TaskName = REDIS_TASK_TYPE_REINSTALL;
            break;
        case 3:
            taskInfo.TaskName = REDIS_TASK_TYPE_ADD_CHANNEL;
            break;
        case 4:
            taskInfo.TaskName = REDIS_TASK_TYPE_PRINT;
            break;
        case 5:
            taskInfo.TaskName = REDIS_TASK_TYPE_CLOSE;
            break;
        case 6:
            taskInfo.TaskName = REDIS_TASK_TYPE_ORDER;
            break;
        default:
            taskInfo.TaskName = REDIS_TASK_TYPE_NUL;
            break;
        }
    }
    if (root["Tasks"].isMember("Time")) {
        taskInfo.nTime = root["Tasks"]["Time"].isUInt()? root["Tasks"]["Time"].asUInt() : 0;    
    }
    if (root["Tasks"].isMember("Time")) {
        taskInfo.nTime = root["Tasks"]["Time"].isUInt()? root["Tasks"]["Time"].asUInt() : 0;    
    }
    if (root["Tasks"].isMember("Interval")) {
        taskInfo.nInterval = root["Tasks"]["Interval"].isUInt()? root["Tasks"]["Interval"].asUInt() : 0;
    }
    if (root["Tasks"].isMember("Postscript")) {
        taskInfo.sPostscript = root["Tasks"]["Postscript"].isString() ? root["Tasks"]["Postscript"].asString() : "";
    }
    if(m_func == NULL) {
        cout << "Parse error" << endl;
        return -1;
    }
    return this->m_func(taskInfo);
}

int RedisParse::TaskInfoToJSONFile(const string &taskInfo){
    return 0;

}

int RedisParse::TaskInfoToString(REDIS_TASK_INFO_T& taskInfo){
    return 0;
}
    
int RedisParse::newTaskInfo(string &sTaskInfo, const string &channel, const string &model, const string &name, int type, 
                                    int time, int interval, const string &postscript){
    Json::Value root;
    root["Tasks"]["Channel"] = channel;
    root["Tasks"]["Model"] = model;
    root["Tasks"]["Name"] = name;
    root["Tasks"]["TaskType"] = type;
    root["Tasks"]["Time"] = time;
    root["Tasks"]["Interval"] = interval;
    root["Tasks"]["Postscript"] = postscript;
    saveJsonString(root, sTaskInfo);
    return 0;
}
int RedisParse::newReportInfo(string &ReportInfo, const string &sChannel, const string &sIp, const string &sMAC, const string &sModel,
                                    const string &sDeviceId, const string &sVersion, 
                                    double dCpu, 
                                    unsigned int nTotalMemory,
	                                unsigned int nFreeMemory,
                                    unsigned int nTotalSize,
                                    unsigned int nDiskSize,
                                    int nPID,
                                    unsigned long ulCurMem,
                                    unsigned long ulPeakMem,
                                    bool bIsExecuting,
	                                int nTimes){
    Json::Value root;
    root["MachineInfo"]["Channel"] = sChannel;
    root["MachineInfo"]["IP"] = sIp;
    root["MachineInfo"]["MAC"] = sMAC;
    root["MachineInfo"]["Model"] = sModel;
    root["MachineInfo"]["DeviceId"] = sDeviceId;
    root["MachineInfo"]["Version"] = sVersion;
    root["MachineInfo"]["CpuPercent"] = dCpu;
    root["MachineInfo"]["TotalMemory"] = nTotalMemory;
    root["MachineInfo"]["FreeMemory"] = nFreeMemory;
    root["MachineInfo"]["TotalSize"] = nTotalSize;
    root["MachineInfo"]["DiskSize"] = nDiskSize;
    root["MachineInfo"]["PID"] = nPID;
    root["MachineInfo"]["CurentMemory"] = (unsigned int)ulCurMem;
    root["MachineInfo"]["PeakMemory"] = (unsigned int)ulPeakMem;
    root["MachineInfo"]["IsExecuting"] = bIsExecuting;
    root["MachineInfo"]["TaskTimes"] = nTimes;
    saveJsonString(root, ReportInfo);
    
    return 0;
}

int RedisParse::newACK(string &RespondInfo, const string &sDeviceId, bool bACK){
    Json::Value root;
    root["Respond"]["Channel"] = "RespondFromAllClient";
    root["Respond"]["DeviceId"] = sDeviceId;
    root["Respond"]["ACK"] = bACK;
    saveJsonString(root, RespondInfo);
    return 0;
}


int RedisParse::printInfo(REDIS_TASK_INFO_T& taskInfo){
    cout << "--------------------------------" << endl;

    cout << "Channel: " << taskInfo.sChannel << endl;
    cout << "Model: " << taskInfo.sModel << endl;
    cout << "TaskName: " << taskInfo.sName << endl;
    cout << "Time: " << taskInfo.nTime << endl;
    cout << "Interval: " << taskInfo.nInterval << endl;
    cout << "Postscript: " << taskInfo.sPostscript << endl;
    
    return 0;
};

int RedisParse::printInfo(REDIS_REPORT_INFO_T& reportInfo){
    return 0;
}

