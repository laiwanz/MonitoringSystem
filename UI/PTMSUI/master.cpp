#include "master.h"
#include <QtConcurrent>

Master::Master(const string& sHost, int nPort, const string& sAuth):
      m_SubPub(sHost, nPort, sAuth),
      m_Parser()
{

}

int Master::showMachineInfo(const REDIS_REPORT_INFO_T& reportInfo){

    return m_Printfunc(reportInfo);
}

int Master::ping(){

    qDebug() << "publish ping : " << m_SubPub.ping() << " Subscribe ping " << m_SubPub.subping();

    return  ((m_SubPub.ping() == 0) && m_SubPub.subping() == 0);
}
int Master::isConnected(){
//    qDebug() << "publish isConnected : " << m_SubPub.isConnected() << " Subscribe isConnected " << m_SubPub.subisConnected();

    return  ((m_SubPub.isConnected() == 1) && m_SubPub.subisConnected() == 1);
}

int Master::recvACK(const REDIS_RESPOSE_INFO_T & respondInfo){

    return m_RecvACK(respondInfo);
}
int Master::publish(const string& Channel, const string & Message){

    m_SubPub.publish(Channel, Message);
    return 0;
}

int Master::setConfig(){


       m_SubPub.config([this](const string& sBuffer){
           return this->m_Parser.parseRedisMessage(sBuffer);
       },   [this](string &sRespondInfo, const string &sDeviceId, bool bACK){
           return this->m_Parser.newACK(sRespondInfo, sDeviceId, bACK);
       });
       this->m_Parser.config(NULL , [this]( REDIS_REPORT_INFO_T reportInfo){
               return showMachineInfo(reportInfo);
    }, [this](REDIS_RESPOSE_INFO_T respondInfo){
        return recvACK(respondInfo);
    });

    return 0;
}

int Master::masterStop(){

    m_SubPub.removeChannel("RespondFromClient");
    m_SubPub.removeChannel("RequestsFromClient");
    m_SubPub.removeChannel("ReportToServer");
    return 0;
}
int Master::masterOpen(){

    m_SubPub.addChannel("RespondFromClient");
    m_SubPub.addChannel("RequestsFromClient");
    m_SubPub.addChannel("ReportToServer");
    return 0;
}


int Master::newATask(string &sTaskInfo, const string &channel, const string &model, const string &name, int type,
             int time, int interval, const string &postscript){
    return m_Parser.newTaskInfo(sTaskInfo, channel, model, name, type, time, interval, postscript);
}

int Master::setFuncConfig(FUNCTION_PRINT_FUNCRION func, FUNCTION_RECVACK_FUNCRION func1, FUNCTION_NEWTASK_TASK func2){
    this->m_Printfunc = func;
    this->m_RecvACK = func1;
    this->m_NewTask = func2;
    return 0;
}
REDIS_REPORT_INFO_T Master::getMachineInfo(){

    return m_MachineInfo;
}

int Master::init(){
    m_SubPub.init();
    return 0;
}
