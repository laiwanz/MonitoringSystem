#ifndef MASTER_H
#define MASTER_H

#include "RedisPubSuber.h"
#include "RedisParse.h"
#include "RedisRecDefine.h"
#include <functional>
#include <QThread>
#include <QDebug>

using FUNCTION_PRINT_FUNCRION = std::function<int(REDIS_REPORT_INFO_T)>;
using FUNCTION_RECVACK_FUNCRION = std::function<int(REDIS_RESPOSE_INFO_T)>;
using FUNCTION_NEWTASK_TASK = std::function<int(const std::string &,
const std::string &, const std::string &, int ,  int , int , const std::string &)>;

class Master
{
public:
    Master(const string& sHost, int nPort, const string& sAuth);
    int init();
public:
    int masterOpen();
    int masterStop();
    int setFuncConfig(FUNCTION_PRINT_FUNCRION func, FUNCTION_RECVACK_FUNCRION func1, FUNCTION_NEWTASK_TASK func2);

    int ping();
    int isConnected();
    int showMachineInfo(const REDIS_REPORT_INFO_T& reportInfo);
    int recvACK(const REDIS_RESPOSE_INFO_T & respondInfo);
    REDIS_REPORT_INFO_T getMachineInfo();
    int setConfig();
    int publish(const string& Channel, const string & Message);

    int newATask(string &sTaskInfo, const string &channel, const string &model, const string &name, int type,
                 int time, int interval, const string &postscript);
    RedisSubscriber m_SubPub;
private:
    RedisParse m_Parser;

    REDIS_REPORT_INFO_T m_MachineInfo;
    FUNCTION_RECVACK_FUNCRION m_RecvACK;
    FUNCTION_PRINT_FUNCRION m_Printfunc;
    FUNCTION_NEWTASK_TASK m_NewTask;
};

#endif // MASTER_H
