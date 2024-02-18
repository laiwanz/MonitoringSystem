#if !defined(REDIS_SUBSCRIBER_H__20230926)
#define REDIS_SUBSCRIBER_H__20230926
#include <cbl/cbl.h>
#include "RedisTask.h"
#include "secure/secure.h"

using FUNCTION_PARSE = std::function<int(const std::string &)>;
using FUNCTION_ACK_PARSE = std::function<int(std::string &, const std::string &, bool )>;


class RedisSubscriber{
public:
    RedisSubscriber(const string& sHost, int nPort, const string& sAuth = string());
    ~RedisSubscriber();
    int config(FUNCTION_PARSE func, FUNCTION_ACK_PARSE func_ack);
    int add(const string& channel);
    int ping();
    bool isConnected();
    int init();
private:
    int parse(const string& sChannel, const string& sMessage, void *lpParameter);

    /*
    * Add Channel form the json file
    */
    int paresAddChannel(const string& sFile);

    int response(const string& sChannel);

    friend int recvChannelMessage(const string& sChannel, const string& sMessage, void *lpParameter);
    friend int onSubscribeClosed(void *lpParameter);

    int respond();
private:
    string m_sTaskMessage;
    
public:
    CRedisSubscribe m_Subscribe;
    CRedisPublish m_Publish;

    FUNCTION_PARSE m_FuncParse;
    FUNCTION_ACK_PARSE m_ACk;
};
/*
* SetCallback function 
*/



#endif //REDIS_SUBSCRIBER_H__20230926