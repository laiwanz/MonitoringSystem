#include "RedisPubSuber.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

CWSA wsa;
RedisSubscriber::RedisSubscriber(const string& sHost, int nPort, const string& sAuth) :
                m_Subscribe(sHost, nPort, sAuth), m_Publish(sHost, nPort, sAuth){
                    m_FuncParse = NULL;
                    m_ACk = NULL;
                };


RedisSubscriber::~RedisSubscriber(){

};

int RedisSubscriber::subping(){
    return this->m_Subscribe.ping();
}
int RedisSubscriber::subisConnected(){
    return this->m_Subscribe.isConnected();
}

int RedisSubscriber::config(FUNCTION_PARSE func, FUNCTION_ACK_PARSE func_ack){
    m_FuncParse = func;
    m_ACk = func_ack;
    return 0;
};


int RedisSubscriber::ping(){
    this->init();
    return this->m_Publish.ping();
}
int RedisSubscriber::isConnected(){
    return this->m_Publish.isConnected();
}

/*
* Parse the Json string for a Redis Publisher 
*/
int RedisSubscriber::parse(const string& sChannel, const string& sMessage, void *lpParameter){

    this->m_sTaskMessage = sMessage;
    if(m_FuncParse == NULL){
        cout << "Parse error in subscribe" << endl;
        return -1;
    } 
    return m_FuncParse(sMessage);

}

int recvChannelMessage(const string& sChannel, const string& sMessage, void *lpParameter){
//    cout << sMessage << endl;
    RedisSubscriber *pClient = static_cast<RedisSubscriber *> (lpParameter);
    if (pClient == NULL) {
        return -1;
    }

    return pClient->parse(sChannel, sMessage, lpParameter);
};

int onSubscribeClosed(void *lpParameter){
    cout << " Closed " << endl;
    return 0;
}

/*
* init connect to server and receive the Task message 
*/
int RedisSubscriber::init(){
    this->m_Subscribe.setCallback(recvChannelMessage, this, onSubscribeClosed, NULL);
    
    return 0;
}

int RedisSubscriber::publish(const string& Channel, const string &Message){
    this->m_Publish.publish(Channel, Message);
    return  0;
}

int RedisSubscriber::removeChannel(const string& Channel){
    this->m_Subscribe.removeChannel(Channel);
    return 0;
}

int RedisSubscriber::addChannel(const string& Channel){
    this->m_Subscribe.addChannel(Channel);
    return 0;
}
