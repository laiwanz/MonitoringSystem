#if !defined(REDIS_DEFINE_H__20230928)
#define REDIS_DEFINE_H__20230928


#include <string>
using std::string;

typedef enum{
	REDIS_TASK_TYPE_REBOOT = 0,
	REDIS_TASK_TYPE_UPDATE,
	REDIS_TASK_TYPE_RE_CONFIG,	
	REDIS_TASK_TYPE_CHANGE_CHANNEL,
	REDIS_TASK_TYPE_PRINT,
	REDIS_TASK_TYPE_CLOSE,
	REDIS_TASK_TYPE_ORDER,
    REDIS_TASK_TYPE_NUL
    
    /*Reboot
    ...
    */
}REDIS_TASK_TYPE_T;

/*
* priority of the task message
*/

typedef struct tagREDIS_TASK_INFO_T{
	string sChannel;							/*task or order*/
    string sModel;		
	string sName;						/* Model */
	REDIS_TASK_TYPE_T TaskName;                 /* Type of task*/
	int nTime;                             		/* The number of times the task was executed*/
	int nInterval;                             	/* interval time*/
    string sPostscript;    
	tagREDIS_TASK_INFO_T(){
		sChannel = "";
		sName = "";
		sModel = "";
        TaskName = REDIS_TASK_TYPE_NUL;
        nTime = 0;
        nInterval = 0;
        sPostscript = "";
	}           /* Additional content for the task*/

}REDIS_TASK_INFO_T;


/*
* Report machine information
*/
typedef struct tagREDIS_REPORT_INFO_T{

    string sChannel;
    string sIp;							/*task or order*/
    string sMAC;
    string sModel;
    string sDeviceId;
    string sVersion;

    int nCpu;
    unsigned int nTotalMemory;
    unsigned int nFreeMemory;
    unsigned int nTotalSize;
    unsigned int nDiskSize;
    int nPID;
    unsigned long ulCurMem;
    unsigned long ulPeakMem;

    bool bIsExecuting;
    int nTimes;                             		/* The number of times the task should execute*/

    tagREDIS_REPORT_INFO_T(){
        sChannel = "";
        sIp = "";
        sMAC = "";
        sModel = "";
        sDeviceId = "";
        sVersion = "";

        nCpu = 0;
        nTotalMemory = 0;
        nFreeMemory = 0;
        nTotalSize = 0;
        nDiskSize = 0;
        nPID = 0;
        ulCurMem = 0;
        ulPeakMem = 0;

        bIsExecuting = 0;
        nTimes = 0;
    }
}REDIS_REPORT_INFO_T;

/*
* Response the request to the server
*/
typedef struct tagREDIS_RESPOSE_INFO_T{

	string sChannel;
	string sDeviceId;
	bool bRes;
	
	tagREDIS_RESPOSE_INFO_T(){
		sChannel = "";
		sDeviceId = "";
		bRes = false;
	}
}REDIS_RESPOSE_INFO_T;


#endif //REDIS_DEFINE_H__20230928
