#include "Update.h"
#include "platform/platform.h"
#include "secure/secure.h"
#include <string>
#include <iomanip>
#include <sys/reboot.h>
/*
* The purpose of the code is to selectively disable the deprecation function warning temporarily, 
* depending on the compiler, so that the code can compile normally.
*/
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif


#define		DEFAULT_HOST			"http://192.168.45.243:48080"
#define		PROTOCOL_VERSION		"1"
#define		TOKEN					"303332616aabc587e88ff8e0a2d64f9d"
#define		HOST_CONF_FILE			"/mnt/data/prj/server.json"
#define		PACKAGE_VERSION_FILE	"/mnt/wecon/app/usr/package"
#define		DOWNLOAD_DIR			"/tmp/RFU"
#define		PACKAGE_FILE			DOWNLOAD_DIR"/update.upk"
#define		PACKAGE_MOUNT_DIR		DOWNLOAD_DIR"/package"
#define		SETUP_FILE_PATH			PACKAGE_MOUNT_DIR"/setup.sh"

#define		SQUASHFS_KO_PATH		"/usr/lib"
#define		SQUASHFS_KO_FILE		SQUASHFS_KO_PATH "/squashfs.ko"
#define		SQUASHFS_INSTALL_SCRIPT	"/usr/bin/insmod_sqsh"
#define		MOUNT_PACKAGE_CMD		"mount -o loop %s %s"
#define		UMOUNT_PACKAGE_CMD		"umount %s"




using namespace PLATFORM_NAMESPACE;
using namespace SECURE_NAMESPACE;

UpdateTask::UpdateTask(int count, int interval, const std::string& message): AbstractTask(count, interval, message){

	m_sDeviceType = m_sDeviceSeries = m_sDeviceId = m_sPackageUrl = m_sPackageVersion = m_sPackageMd5 = "";
	m_nForceUpdate = 0;
    m_vecExtraName.push_back("114.114.114.114");        // 电信做的国内免费公众DNS服务
    m_vecExtraName.push_back("8.8.8.8");                        // Google提供的免费DNS服务器
    m_vecExtraName.push_back("223.5.5.5");                // 阿里推出公共DNS
    m_vecExtraName.push_back("8.8.4.4");                        // Google提供的免费DNS服务器,备
    m_vecExtraName.push_back("223.6.6.6");                // 阿里推出公共DNS
    m_vecExtraName.push_back("208.67.222.222");        // 美国opendns
    m_vecExtraName.push_back("180.76.76.76");                // 百度公共DNS
    m_vecExtraName.push_back("208.67.222.123");        // 美国opendns
};

UpdateTask::~UpdateTask(){};


static int getJsonValue(Json::Value& root, const std::string& sKey, std::string& sValue) {

	if (false == root.isMember(sKey)) {

		return -1;
	}

	sValue = root[sKey].asString();
	return 0;
}

static int getJsonValue(Json::Value& root, const std::string& sKey, int& nValue) {

	if (false == root.isMember(sKey)) {

		return -1;
	}

	if (Json::stringValue == root[sKey].type()) {

		std::string sbuffer = root[sKey].asString();
		if (!sbuffer.empty()) nValue = std::stoi(sbuffer);
	}
	else {

		nValue = root[sKey].asInt();
	}

	return 0;
}




int UpdateTask::execute(){


    if (!cbl::isDir(DOWNLOAD_DIR)) {

		cbl::makeDir(DOWNLOAD_DIR);
	}

	if (0 > getDeviceInfo()) {

		return -1;
	}


	cout << "execute" << endl;
    if (this->checkUpdate() == 0) {
        this->downloadAndInstall();
    }
	// if(m_sVersion != sVersion) {
	// 	std::cout << "reboot" << std::endl;
	// 	reboot(RB_AUTOBOOT);
	// }

    return 0;
}

int UpdateTask::checkUpdate(){
	int		nRtn = 0;
	std::string sUrl = "", sBuffer = "";

	if (0 > (nRtn = this->getUrl(sUrl)) ||
		sUrl.empty()) {
		printf("%s:getUrl errno: %d\r\n", __FUNCTION__, nRtn);
		return -1;
	}
	printf("remote firmware update url:%s\r\n", sUrl.c_str());

    cbl::CHttpDownload  httpDownload;

    if (( nRtn = httpDownload.setExtraNameServer(m_vecExtraName) ) < 0){
        printf("%s:setExtraNameServer error:%d\r\n", __FUNCTION__, nRtn);
        return -3;
    }

	if (0 != (nRtn = httpDownload.get(sUrl, sBuffer, 2048)) ||
		sBuffer.empty()) {

		printf("%s:httpDownload errno %d\r\n", __FUNCTION__, nRtn);
		return -2;
	}

	if (0 != (nRtn = parseBuffer(sBuffer))) {
		printf("%s:parseBuffer errno %d\r\n", __FUNCTION__, nRtn);
		return -3;
	}

	if (1 == m_nForceUpdate ||
		m_sPackageUrl.empty()) {
		printf("%s:Force update %d, url is %s\r\n", __FUNCTION__, m_nForceUpdate, m_sPackageUrl.c_str());
		return -4;
	}

    return 0;
};

int UpdateTask::getDeviceInfo(){
    
    switch (CPlatBoard::getType()) {
	case A8_FLASH_BOARD_v2:
		{
			m_sDeviceType = "common";
			m_sDeviceSeries = "3000";
		}
		break;
	case A8_EMMC_BOARD_v2:
		{
			m_sDeviceType = "common";
			m_sDeviceSeries = "8000";
		}
		break;
	case A9_EMMC_BOARD_v2:
		{
			m_sDeviceType = "common";
			m_sDeviceSeries = "9000";
		}
		break;
	case A7_FLASH_BOARD_v2:
		{
			m_sDeviceType = "i";
			m_sDeviceSeries = "3000";
		}
		break;
	case RK3308B_FLASH64Mmem_BOARD_v2:
		{
			m_sDeviceType = "ie";
			m_sDeviceSeries = "3000";
		}
		break;
	case RK3308B_FLASH_BOARD_v2:
		{
			m_sDeviceType = "ig";
			m_sDeviceSeries = "3000";
		}
		break;
	case rk3308b_eMMC_v2:
		{
			m_sDeviceType = "ig";
			m_sDeviceSeries = "8000";
		}
		break;
	default:
		break;
	}

	WMachine			wMachine;
	wMachine.GetProduct(m_sDeviceId);



	if (m_sDeviceType.empty() ||
		m_sDeviceSeries.empty() ||
		m_sDeviceId.empty()) {
		return -1;
	}
	return 0;
};


/*
* The steps of reading the local version, generating the timestamp, 
* calculating the signature, etc., construct a URL with the necessary parameters for version checking
*/
int UpdateTask::getUrl(std::string &sUrl){

	std::string sHostName = "", sVersion = "";
	sHostName = DEFAULT_HOST;

	if (cbl::loadFromFile(PACKAGE_VERSION_FILE, sVersion) <= 0) {
		sVersion = "0.0.0.22011801";
	}
	m_sVersion = sVersion;
	int nSeries;

	if (m_sDeviceType == "i") {
		nSeries = 1;
	} else if (m_sDeviceType == "ie") {
		nSeries = 2;
	} else if (m_sDeviceType == "ig") {
		nSeries = 3;
	} else if (m_sDeviceType == "IN")	{
		nSeries = 4;
	} else if (m_sDeviceType == "MD") {
		nSeries = 5;
	} else if (m_sDeviceType == "RE")	{
		nSeries = 6;
	} else if (m_sDeviceType == "common")	{
		nSeries = 0;
	}
	
	
	int nModel;

	if (m_sDeviceSeries == "3000") {
		nModel = 0;
	} else if (m_sDeviceSeries == "8000") {
		nModel = 1;
	} else if (m_sDeviceSeries == "9000") {
		nModel = 2;
	}
	sUrl = sHostName + std::string("/admin-api/version/check?") + "type=4" + "&version=" + 
	sVersion +"&series=" + std::to_string(nSeries) +"&model=" + std::to_string(nModel);
    return 0;
};


int UpdateTask::parseBuffer(const std::string &sbuffer){
    if (sbuffer.empty()) {

		return -1;
	};
	Json::Reader    reader;
	Json::Value     root;

	if (false == reader.parse(sbuffer, root) ||
		true == root.isNull()) {

		return -2;
	}

	if(root.isMember("data")) {

		if (0 != getJsonValue(root["data"], "isLastVersion", m_nForceUpdate)) {
			return -4;
		}	
	}

	if(root["data"].isMember("versionList")){
		
		if (0 != getJsonValue(root["data"]["versionList"][0], "version", m_sPackageVersion)) {

			return -5;
		}
	}
	if(root["data"].isMember("versionList")){
		
		if (0 != getJsonValue(root["data"]["versionList"][0], "fileId", m_sToken)) {

			return -6;
		}
	}
	if(root["data"].isMember("versionList")){
		
		if (0 != getJsonValue(root["data"]["versionList"][0], "fileMd5", m_sPackageMd5)) {

			return -7;
		}
	}

	std::string sUrl = R"+*(http://192.168.56.20:8080/auth/publish/download?)+*";
	std::string sQueryString = "", sToken = "";
	/* make token */
	if (cbl::md5String("v1#cn.com.we-con.upload", sToken) < 0) {
			return -1;
	}
	if (cbl::appendSignFieldToQueryString(format("v=1&file_id=%s&ts=%" PRIu64, m_sToken.c_str(), cbl::CTimeUtil().getTimestampUs()), sToken, sQueryString) < 0) {
			return -2;
	}

	sUrl += sQueryString;
	m_sPackageUrl = sUrl;
    return 0;

};


int UpdateTask::installPackage(){
	if (!cbl::isDir(PACKAGE_MOUNT_DIR)) {

		cbl::makeDir(PACKAGE_MOUNT_DIR);
	}

	char    szCmd[256] = { 0 };

	if (0 == access(SQUASHFS_KO_FILE, F_OK) &&
		0 == access(SQUASHFS_INSTALL_SCRIPT, F_OK)) {
		sprintf(szCmd, "%s %s", SQUASHFS_INSTALL_SCRIPT, SQUASHFS_KO_PATH);
		system(szCmd);
	}
    
	memset(szCmd, 0, sizeof(szCmd));
	sprintf(szCmd, MOUNT_PACKAGE_CMD, PACKAGE_FILE, PACKAGE_MOUNT_DIR);
	system(szCmd);

	memset(szCmd, 0, sizeof(szCmd));
	sprintf(szCmd, UMOUNT_PACKAGE_CMD, PACKAGE_MOUNT_DIR);
	if (0 != access(SETUP_FILE_PATH, F_OK)) {

		printf("%s:%s not exist!\n", __FUNCTION__, SETUP_FILE_PATH);
		return -1;
	}

	// /tmp/RFU/package/setup.sh
	system(SETUP_FILE_PATH);

    // umount /tmp/RFU/package
	system(szCmd);

	system("mount -o remount rw, /");

	cbl::saveToFile(m_sPackageVersion, PACKAGE_VERSION_FILE);

	system("mount -o remount r, /");
    return 0;
};

int UpdateTask::downloadAndInstall(){

    int		nRtn = 0;

    cbl::CHttpDownload  httpDownload;

    if (( nRtn = httpDownload.setExtraNameServer(m_vecExtraName) ) < 0) {
        printf("%s:setExtraNameServer error:%d\r\n", __FUNCTION__, nRtn);
        return -3;
    }
	if (0 != (nRtn = httpDownload.get(m_sPackageUrl, PACKAGE_FILE))) {
		printf("%s:Download errno %d\r\n", __FUNCTION__, nRtn);
		return -2;
	}
	std::cout << "Downloading: " << m_sPackageUrl << std::endl;
	std::string sMd5;
	cbl::md5File(PACKAGE_FILE, sMd5);
	std::cout << sMd5 << std::endl;
	if (sMd5 != m_sPackageMd5) {

		printf("%s:md5 error\r\n", __FUNCTION__);
		remove(PACKAGE_FILE);
		return -6;
	}

	
	installPackage();
	m_nForceUpdate = 0;

    return 0;
};

