#if !defined(REINSTALL_H__20231011)
#define REINSTALL_H__20231011
#include <string>
#include "AbstractTask.h"
#include "AbstractTaskFactory.h"


class ReinstallTask : public AbstractTask{
public:
    ReinstallTask(int count, int interval, std::string message);
    ~ReinstallTask();

public:
    int execute();
private:

private:

};


class ReinstallTaskFactory : public AbstractTaskFactory{
public:
    AbstractTask* createTask(int count, int interval, const std::string &message);
    
};


#endif //UPDATE_TASK_H__20231011st
