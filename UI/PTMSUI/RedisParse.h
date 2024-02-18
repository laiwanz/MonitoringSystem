﻿#if !defined(REDIS_PARSE_H_20231012)
#define REDIS_PARSE_H_20231012
#include "RedisRecDefine.h"
#include "cbl/cbl.h"
#include "cbl/util/JsonUtil.h"


using FUNCTION_PUSH_TASK = std::function<int(REDIS_TASK_INFO_T)>;
using FUNCTION_SHOW_TASK = std::function<int(REDIS_REPORT_INFO_T)>;
using FUNCTION_ACK_TASK = std::function<int(REDIS_RESPOSE_INFO_T)>;

class RedisParse{
public:
    RedisParse();
    
    ~RedisParse();
    
    int config(FUNCTION_PUSH_TASK func, FUNCTION_SHOW_TASK func1, FUNCTION_ACK_TASK func2);

    /*
    * String Message to TaskInfo object
    */
    int parseRedisMessage(const string& task);

    /*
    * JSON File to TaskInfo object 
    */
    int praseJsonFile(const string& nFile);

    /*
    * Print task info
    */
    int printInfo(REDIS_TASK_INFO_T& taskInfo);
    
    /*
    * Print task info
    */
    int printInfo(REDIS_REPORT_INFO_T& reportInfo);

    /*
    * New A TaskInfo string to Publish to RedisClient   
    */
    int newTaskInfo(string & sTaskInfo, const string &channel, const string &model, const string &name, int type, int time, int interval, const string &postscript);

    int newReportInfo(string &ReportInfo, const string &sChannel, const string &sIp, const string &sMAC, const string &sModel,
                            const string &sDeviceId,  const string &sVersion,
                            int nCpu,
                            unsigned int nTotalMemory,
	                        unsigned int nFreeMemory,
                            unsigned int nTotalSize,
                            unsigned int nDiskSize,
                            int nPID,
                            unsigned long ulCurMem,
                            unsigned long ulPeakMem,
                            bool bIsExecuting,
	                        int nTimes);
    int newACK(string &RespondInfo, const string &sDeviceId, bool bACK);
private:
    FUNCTION_PUSH_TASK m_func;
    FUNCTION_SHOW_TASK m_func1;
    FUNCTION_ACK_TASK m_func2;
};

#endif // REDIS_PARSE_H_20231012
