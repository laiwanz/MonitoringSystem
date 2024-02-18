#if !defined(BACK_LIGHT_TASK_H__20231102)
#define BACK_LIGHT_TASK_H__20231102
#include <string>
#include "cbl/cbl.h"
#include "AbstractTask.h"
#include "AbstractTaskFactory.h"


class BackLightTask : public AbstractTask{
public:
    BackLightTask(int count, int interval, const std::string& message): AbstractTask(count, interval, message){

        this->m_lcd = new cbl::CLcd();

    };
    ~BackLightTask(){
        delete m_lcd;
    };

public:
    int execute() override{
        m_lcd->backlightOn();
        int nBackLight = atoi(m_sMessage.c_str());
        m_lcd->setBrightness(nBackLight);
        return 0;
    };
private:
    cbl::CLcd* m_lcd;

};


class BackLightTaskFactory : public AbstractTaskFactory{
public:
    AbstractTask* createTask(int count, int interval,const std::string& message){
        return new BackLightTask(count, interval, message);
    }
    
};


#endif //BACK_LIGHT_TASK_H__20231102
