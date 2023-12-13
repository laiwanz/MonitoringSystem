#include "RedisPubSuber.h"

RedisSubscriber::RedisSubscriber(const string& sHost, int nPort, const string& sAuth) :
                m_Subscribe(sHost, nPort, sAuth), m_Publish(sHost, nPort, sAuth){
                    m_FuncParse = NULL;
                    m_ACk = NULL;
                };

RedisSubscriber::~RedisSubscriber(){

};

int RedisSubscriber::config(FUNCTION_PARSE func, FUNCTION_ACK_PARSE func_ack){
    m_FuncParse = func;
    m_ACk = func_ack;
    return 0;
};


int RedisSubscriber::ping(){
    return this->m_Publish.ping();
}
bool RedisSubscriber::isConnected(){
    return this->m_Publish.isConnected();
}

/*
* Parse the Json string for a Redis Publisher 
*/
int RedisSubscriber::parse(const string& sChannel, const string& sMessage, void *lpParameter){
    this->m_sTaskMessage = sMessage;

    /*
    * ACK 
    */

    respond();
    if(m_FuncParse == NULL){
        cout << "Parse error in subscribe" << endl;
        return -1;
    } 
    return m_FuncParse(sMessage);

}

int recvChannelMessage(const string& sChannel, const string& sMessage, void *lpParameter){
    RedisSubscriber *pClient = static_cast<RedisSubscriber *> (lpParameter);
    if (pClient == NULL) {
        return -1;
    }

    return pClient->parse(sChannel, sMessage, lpParameter);
};

int onSubscribeClosed(void *lpParameter){
    cout << "Remove Channel " << endl;
    return 0;
}

/*
* init connect to server and receive the Task message 
*/
int RedisSubscriber::init(){
    //std:: cout << "init" << std::endl;
    this->m_Subscribe.setCallback(recvChannelMessage, this, onSubscribeClosed, NULL);
    
    return 0;
}

int RedisSubscriber::respond(){
    SECURE::WMachine m_Machine;
    string sDeviceId;
    m_Machine.getDeviceId(sDeviceId);
    string sResponse;
    m_ACk(sResponse, sDeviceId, true);
    this->m_Publish.publish("RespondFromAllClient", sResponse);
    return 0;
}

int  RedisSubscriber::add(const string& channel){
    this->m_Subscribe.removeChannel(channel);
    return 0;
}
