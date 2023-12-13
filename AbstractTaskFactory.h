#if !defined(ABSTRACT_TASK_FACTORY_H__20231011)
#define ABSTRACT_TASK_FACTORY_H__20231011
#include "cbl/cbl.h"
#include "AbstractTask.h"


class AbstractTaskFactory{
public:
    virtual AbstractTask* createTask(int count, int interval,const std::string& message) = 0;
    virtual ~AbstractTaskFactory(){};
};

#endif //ABSTRACT_TASK_FACTORY_H__20231011
