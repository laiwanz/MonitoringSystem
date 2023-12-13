#if !defined(ABSTRACT_TASK_H__20231011)
#define ABSTRACT_TASK_H__20231011
#include "cbl/cbl.h"
#include <atomic>
#include <chrono>


class AbstractTask{
public:
    // AbstractTask(std::string message):m_message(message), m_nExecutionsCount(1), m_nInterval(0){
    //     updateNextExecutionTime();
    // }
    AbstractTask(int count, int interval, std::string message):m_nExecutionsCount(count), m_nInterval(interval), m_sMessage(message){
        updateNextExecutionTime();
    };

    virtual ~AbstractTask(){};

public:
    virtual int execute(){
        return 0;
    };

    bool isReadyToExecute(const std::chrono::system_clock::time_point currentTime) const{
        return currentTime >= m_nExecutionTime;
    }

    void updateNextExecutionTime(){

        m_nExecutionTime = std::chrono::system_clock::now() + std::chrono::milliseconds(m_nInterval);
    }

    void reduceExecutionTime(){

        --m_nExecutionsCount;
    }

    int getExecutionCount(){
        
        return m_nExecutionsCount;
    }


protected:
    int m_nExecutionsCount;
    int m_nInterval;
    std::string m_sMessage;
    std::chrono::system_clock::time_point m_nExecutionTime;
};

#endif //ABSTRACT_TASK_H__20231011
