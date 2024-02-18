#if !defined(UPDATE_TASK_H__20231011)
#define UPDATE_TASK_H__20231011
#include <string>
#include "AbstractTask.h"
#include "AbstractTaskFactory.h"


class UpdateTask : public AbstractTask{
public:
    UpdateTask(int count, int interval,const std::string& message);
    ~UpdateTask();

public:
    int execute();
private:
    int checkUpdate();
    int getDeviceInfo();
    int getUrl(std::string &sUrl);
    int parseBuffer(const std::string &sbuffer);
	int installPackage();
    int downloadAndInstall();
private:

	std::string		m_sDeviceType;
	std::string		m_sDeviceSeries;
	std::string		m_sDeviceId;
	std::string		m_sPackageUrl;
	std::string		m_sPackageVersion;
	std::string		m_sPackageMd5;
	int				m_nForceUpdate;
    std::string	    m_sVersion;
    std::string     m_sToken;
    vector<std::string>	m_vecExtraName;
};


class UpdateTaskFactory : public AbstractTaskFactory{
public:
    AbstractTask* createTask(int count, int interval, const std::string &message){
        return new UpdateTask(count, interval, message);
    }
    
};


#endif //UPDATE_TASK_H__20231011
