#if !defined(REDIS_SUBSCRIBER_H__20230926)
#define REDIS_SUBSCRIBER_H__20230926
#include "cbl/cbl.h"
#include "cbl/redismq/RedisSubscribe.h"
#include "cbl/redismq/RedisPublish.h"
#include "winsock.h"
#include <QString>

using FUNCTION_PARSE = std::function<int(const std::string &)>;
using FUNCTION_ACK_PARSE = std::function<int(std::string &, const std::string &, bool )>;


class RedisSubscriber{
public:
    RedisSubscriber(const string& sHost, int nPort, const string& sAuth);
    ~RedisSubscriber();
    int config(FUNCTION_PARSE func, FUNCTION_ACK_PARSE func_ack);
    int init();
    int ping();
    int subisConnected();
    int subping();
    int isConnected();
    int publish(const string& Channel, const string &message);
    int addChannel(const string& Channel);
    int removeChannel(const string& Channel);
private:
    int parse(const string& sChannel, const string& sMessage, void *lpParameter);
    /*
    * Add Channel form the json file
    */
    int paresAddChannel(const string& sFile);

    int response(const string& sChannel);

    friend int recvChannelMessage(const string& sChannel, const string& sMessage, void *lpParameter);
    friend int onSubscribeClosed(void *lpParameter);
private:

    CRedisPublish m_Publish;
    CRedisSubscribe m_Subscribe;
    string m_sTaskMessage;
    FUNCTION_PARSE m_FuncParse;
    FUNCTION_ACK_PARSE m_ACk;
};
/*
* SetCallback function 
*/



#endif //REDIS_SUBSCRIBER_H__20230926
