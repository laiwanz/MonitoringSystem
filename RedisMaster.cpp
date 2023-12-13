#include "RedisMaster.h"

RedisMaster::RedisMaster(const string& sHost, int nPort, const string& sAuth)
                         : m_PubSub(sHost, nPort, sAuth), m_Parser(), m_Task(){



}
RedisMaster::~RedisMaster(){

}

int RedisMaster::publish(const string& sChannel, const string& sMessage) {
    m_PubSub.m_Publish.publish(sChannel, sMessage);
    return 0;
};






int RedisMaster::removeChannel(const string& channel){
    this->m_PubSub.add(channel);
    return 0;
}



int RedisMaster::run() {
    
    return 0;
};

int RedisMaster::init(){
    this->m_PubSub.init();


    return 0;
}

int RedisMaster::addAChannel(const string& Channel){

    if(this->m_PubSub.m_Subscribe.addChannel(Channel) < 0){
        return -1;
    };
    return 0;
}