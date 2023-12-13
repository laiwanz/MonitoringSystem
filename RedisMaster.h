#if !defined(CLIENT_MASTER_H__20230928)
#define CLIENT_MASTER_H__20230928
#include "RedisInclude.h"
#include "cbl/cbl.h"
#include <functional>

/*

* Management client
* Parses the Task Message and packages the request
* and Send the mission to the Specific task class
* 

*/


class RedisMaster {

protected:
    
    RedisSubscriber m_PubSub;
    
    RedisParse m_Parser;
    
    RedisTask m_Task;

public:
    RedisMaster(const string& sHost, int nPort, const string& sAuth = string());
    
    virtual ~RedisMaster();

    virtual int run();

    int removeChannel(const string& channel);
    
    int init();

    int addAChannel(const string&sChannel);
    
    int publish(const string& sChannel, const string& sMessage);

private:

};


#endif //CLIENT_MASTER_H__20230928