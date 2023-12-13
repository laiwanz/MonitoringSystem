#include "RedisTask.h"
#include "PrintTask.cpp"
#include "Update.h"
#include "reinstall.h"
#include "AbstractTaskFactory.h"
#include <sys/reboot.h>
#include "secure/secure.h"
#include "BeepTask.hpp"
// #include "CommandTask.hpp"
#include "BackLightTask.hpp"



RedisTask::RedisTask() : m_bStopFlag(false){
    this->getDeviceId();
}
RedisTask::~RedisTask(){
    stop();
}

int RedisTask::getDeviceId(){
    SECURE::WMachine m_Machine;
    if (m_Machine.getDeviceId(this->m_sDeviceId) < 0) {
        cerr << "cant get the HMI DeviceId " << endl;
    }
    return 0;
}

int RedisTask::config(FUNCTION_ADD_CHANNEL_FUNCTION func, FUNCTION_COMMAND_REPLY_TO_SERVER_FUNCTION func1){
    this->m_func = func;
    this->m_func1 = func1;
    return 0;
}
/*
* Parse the taskInfo and create the taskFactory object 
* and then push the taskFactory into the taskList 
*/    
int RedisTask::push(const REDIS_TASK_INFO_T &taskInfo){

    /*
    * Get the task sModel and judge the client Model whether matches the task model
    */

    int nTask = taskInfo.TaskName;
    switch (nTask){
    case REDIS_TASK_TYPE_REBOOT:
        
        if (taskInfo.vMembers[0] == "ALL" ){
            reboot(RB_AUTOBOOT);
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                reboot(RB_AUTOBOOT);
                break;
            }
        }
        break;
    case REDIS_TASK_TYPE_UPDATE:{
        
        if (taskInfo.vMembers[0] == "ALL" ){
            std::unique_ptr<AbstractTaskFactory> factory(new UpdateTaskFactory());
            std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
            m_listTasks.push_back(std::move(task));
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                std::unique_ptr<AbstractTaskFactory> factory(new UpdateTaskFactory());
                std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
                m_listTasks.push_back(std::move(task));
                break;
            }
        }
        break;
    }
    case REDIS_TASK_TYPE_REINSTALL:{
        /*
        * ReInstall the exe
        */
        if (taskInfo.vMembers[0] == "ALL" ){
            std::unique_ptr<AbstractTaskFactory> factory(new ReinstallTaskFactory());
            std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
            m_listTasks.push_back(std::move(task));
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
            std::unique_ptr<AbstractTaskFactory> factory(new ReinstallTaskFactory());
            std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
            m_listTasks.push_back(std::move(task));
                break;
            }
        }
        break;
    }
    case REDIS_TASK_TYPE_ADD_CHANNEL: {
        /*
        * Determine its DeviceId in the dynamic array , if it exists, add the Channel 
        */
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                m_func(taskInfo.sChannel);
            }
        }
        break;
    }
    case REDIS_TASK_TYPE_PRINT: {
        
         if (taskInfo.vMembers[0] == "ALL" ){
            std::unique_ptr<AbstractTaskFactory> factory(new PrintTaskFactory());
            std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
            m_listTasks.push_back(std::move(task));
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                std::unique_ptr<AbstractTaskFactory> factory(new PrintTaskFactory());
                std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
                m_listTasks.push_back(std::move(task));
                break;
            }
        }
        break;
    }
    case REDIS_TASK_TYPE_CLOSE:
        
        if (taskInfo.vMembers[0] == "ALL" ){
            system("./destruct.sh");
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                system("./destruct.sh");
                break;
            }
        }
        break;
    case REDIS_TASK_TYPE_ORDER: {
        
        // CommandTask m_command(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript);
        // m_command.config([this](const string & reply){
        //     return this->m_func1(reply);
        // });

        }
        break;
    case REDIS_TASK_TYPE_BEEP_ORDER: {

        if (taskInfo.vMembers[0] == "ALL" ){
            std::unique_ptr<AbstractTaskFactory> factory(new BeepTaskFactory());
            std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
            m_listTasks.push_back(std::move(task));
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                std::unique_ptr<AbstractTaskFactory> factory(new BeepTaskFactory());
                std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
                m_listTasks.push_back(std::move(task));
                break;
            }
        }
        break;
        }
    case REDIS_TASK_TYPE_BLACKLIGHT_ORDER: {
        if (taskInfo.vMembers[0] == "ALL" ){
            std::unique_ptr<AbstractTaskFactory> factory(new BackLightTaskFactory());
            std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
            m_listTasks.push_back(std::move(task));
            break;
        }
        for(std::string Id : taskInfo.vMembers){
            if(Id == m_sDeviceId){
                std::unique_ptr<AbstractTaskFactory> factory(new BackLightTaskFactory());
                std::unique_ptr<AbstractTask> task(factory->createTask(taskInfo.nTime, taskInfo.nInterval, taskInfo.sPostscript));
                m_listTasks.push_back(std::move(task));
                break;
            }
        }
        break;
    }
    default:
        break;
    }

    if(!m_WorkThread.joinable()){
        std::cout << "WorkThread" << std::endl;
        m_WorkThread = std::thread(&RedisTask::worker, this);
    }
    return 0;
}

 int RedisTask::stop(){
    m_bStopFlag = true;
    if(m_WorkThread.joinable()){
        m_WorkThread.join();
    }
    return 0;
 }



void RedisTask::worker(){

    while(!m_bStopFlag){
        if(m_listTasks.size() == 0){
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;   
        }
        auto currentTime = std::chrono::system_clock::now();
        for(auto it = m_listTasks.begin(); it != m_listTasks.end();){
            if((*it)->isReadyToExecute(currentTime)){
                (*it)->execute();
                (*it)->reduceExecutionTime();

                if((*it)->getExecutionCount() <= 0){
                    it = m_listTasks.erase(it);
                }else{
                    (*it)->updateNextExecutionTime();
                    ++it;
                }
            }else{
                ++it;
            }
        }
    }
}


bool RedisTask::isRun(){
    if(!m_listTasks.empty()){
        return true;
    }else{
        return false; 
    }
}

int RedisTask::TaskListSize(){
    return m_listTasks.size();
}