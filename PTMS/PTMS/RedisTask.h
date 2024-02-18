#if !defined(REDIS_TASK_H__20230926)
#define REDIS_TASK_H__20230926
#include <list>
#include <thread>
#include <atomic>
#include <chrono>
#include <unistd.h>
#include <memory>
#include <iostream>
#include "cbl/cbl.h"
#include "RedisRecDefine.h"
#include "AbstractTask.h"

using FUNCTION_ADD_CHANNEL_FUNCTION = std::function<int(const string&)>;

using FUNCTION_COMMAND_REPLY_TO_SERVER_FUNCTION = std::function<int(const REDIS_RESPOSE_INFO_T&)>;
/*
* Instantiate a concrete class
* crate the class means that the task will be executed
* till the class is destructed means that the task will be stoped;

*/


class RedisTask{
public:
    RedisTask();
     ~RedisTask();

public:
    /*
    * Parses the Message which bases on JSON
    */
    int getDeviceId();

    int config(FUNCTION_ADD_CHANNEL_FUNCTION func, FUNCTION_COMMAND_REPLY_TO_SERVER_FUNCTION func1);
  
    /*
    * Depends on the taskLists and create the TaskFactory to create the task
    * input the taskInfo.nTimes、nInterval and the message
    * differentiate the taskInfo.nType and the chose whether the task could run in other thread
    * when the task is finished and the taskList.pop()
    */

    bool isRun();

    int TaskListSize();
    
    int stop();

    int push(const REDIS_TASK_INFO_T &taskInfo);

private:
    /*
    * Runs the thread  
    * 
    */
    void worker();

private:

    FUNCTION_ADD_CHANNEL_FUNCTION m_func;

    FUNCTION_COMMAND_REPLY_TO_SERVER_FUNCTION m_func1;

    std::string m_sDeviceId;     
    
    std::list<std::unique_ptr<AbstractTask>> m_listTasks;

    std::atomic<bool> m_bStopFlag;

    std::thread m_WorkThread;

};

#endif //REDIS_TASK_H__20230926
