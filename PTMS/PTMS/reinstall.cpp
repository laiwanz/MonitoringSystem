#include "reinstall.h"
#include "platform/platform.h"
#include "secure/secure.h"
#include <string>



using namespace PLATFORM_NAMESPACE;
using namespace SECURE_NAMESPACE;


ReinstallTask::ReinstallTask(int count, int interval, std::string message): AbstractTask(count, interval, message){
    
};

ReinstallTask::~ReinstallTask(){

};

int ReinstallTask::execute(){

    std::system("chmod +x /mnt/data/monitoring_system/reinstall.sh");

    int result = std::system("./reinstall.sh");

    return result;
}



AbstractTask* ReinstallTaskFactory::createTask(int count, int interval, const std::string& message){
    return new ReinstallTask(count, interval, message);
}

