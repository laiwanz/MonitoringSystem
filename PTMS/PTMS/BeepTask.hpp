#if !defined(BEEP_TASK_H__20231102)
#define BEEP_TASK_H__20231102
#include <string>
#include "cbl/cbl.h"
#include <unistd.h>
#include "AbstractTask.h"
#include "AbstractTaskFactory.h"


class BeepTask : public AbstractTask{
public:
    BeepTask(int count, int interval, const std::string& message): AbstractTask(count, interval, message){
        m_buzzer = new cbl::CBuzzer();
    };
    ~BeepTask(){
        delete m_buzzer;
    };

public:
    int execute() override{
        m_buzzer->run();
        sleep(1);
        m_buzzer->stop();
        return 0;
    };
private:
    cbl::CBuzzer* m_buzzer;

};


class BeepTaskFactory : public AbstractTaskFactory{
public:
    AbstractTask* createTask(int count, int interval,const std::string& message){
        return new BeepTask(count, interval, message);
    }
    
};


#endif //BEEP_TASK_H__20231102
