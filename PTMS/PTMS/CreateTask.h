#if !defined(CREATE_TASK_H__20231011)
#define CREATE_TASK_H__20231011
#include <string>
#include "AbstractTask.h"

class AbstractCreateTask{
public:
    virtual ~AbstractCreateTask(){};
    virtual void produce() = 0;
}

class CreateTask

#endif //CREATE_TASK_H__20231011
