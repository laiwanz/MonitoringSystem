#if !defined(PRINT_TASK_H__20231011)
#define PRINT_TASK_H__20231011
#include <string>
#include "AbstractTask.h"
#include "AbstractTaskFactory.h"


class PrintTask : public AbstractTask{
public:
    PrintTask(int count, int interval, std::string message): AbstractTask(count, interval, message){

    };
    ~PrintTask(){};

public:
    int execute() override{
        std::cout << "Executing PrintTask: " << this->m_sMessage <<  std::endl;
        return 0;
    };
private:
};


class PrintTaskFactory : public AbstractTaskFactory{
public:
    AbstractTask* createTask(int count, int interval,const std::string& message){
        return new PrintTask(count, interval, message);
    }
    
};


#endif //PRINT_TASK_H__20231011
