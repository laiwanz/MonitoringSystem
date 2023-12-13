#if !defined(COMMAND_TASK_H__20231102)
#define COMMAND_TASK_H__20231102
#include <string>
#include <list>
#include "cbl/cbl.h"
#include "secure/secure.h"
#include "RedisRecDefine.h"
#include "AbstractTask.h"
#include "AbstractTaskFactory.h"

using FUNCTION_COMMAND_REPLY_TASK = std::function<int(const REDIS_RESPOSE_INFO_T &)>;


class CommandTask : public AbstractTask{
public:
    CommandTask(int count, int interval, const std::string& message) : AbstractTask(count, interval, message) {
        m_PlistOutput = new list<string>();
        
    };
    ~CommandTask(){
        delete m_PlistOutput;
    };

public:
    int config(FUNCTION_COMMAND_REPLY_TASK func){
        this->m_func = func;
    }
    int execute(){
        std::string sCommandReply;
        if(cbl::execCmd(this->m_sMessage, this->m_PlistOutput) == 0) {
			for(auto s = m_PlistOutput->begin(); s != m_PlistOutput->end(); s++) {
                sCommandReply += (*s);
                sCommandReply += "\n";
			}
            REDIS_RESPOSE_INFO_T m_reply;
            m_reply.sChannel = "RespondFromAllClient";
            SECURE::WMachine m_Machine;
            std::string sDeviceId;
            if (m_Machine.getDeviceId(sDeviceId) < 0) {
                cerr << "cant get the HMI DeviceId " << endl;
            }
            m_reply.sDeviceId = sDeviceId;
            m_reply.sReply = sCommandReply;
            m_reply.bRes = true;

            this->m_func(m_reply);
	    } else {
			 sCommandReply += "Command execution failed." ; 
		};


    );

private:
    list<string> *m_PlistOutput
    FUNCTION_COMMAND_REPLY_TASK m_func;
};


class CommandTaskFactory : public AbstractTaskFactory{
public:
    AbstractTask* createTask(int count, int interval, const std::string &message){
        return new CommandTask(count, interval, message);
    }
    
};




#endif //COMMAND_TASK_H__20231102