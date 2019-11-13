#ifndef __WHSCOMINNER_LIB__
#define __WHSCOMINNER_LIB__


#include "whscomlib.h"

#define BUILD_RELEASE     1  //when build release, increase 1.

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus


/** 设备信息*/
struct whscom_device_info{
	char ip[16]; //设备IP地址
	int port; //设备端口
	unsigned char mac[6]; //设备MAC地址
	int broad_id; //设备板卡ID
	char server_ip[16]; //服务器IP地址
	int server_port; //服务器端口
	unsigned char key[32]; //设备KEY
	bool key_valid; //设备KEY使能
	int match_win_mask_th; //win mask阈值, 默认 64
	int match_valnm_th;//valnm阈值, 默认 6554
	int match_score_th;//score阈值, 默认 6226
	int match_sf;//默认AE1, 2785
	int match_max_v;//默认2DB6，11702
};



/**
* 描述： 获取设备信息。
* 参数： device 设备句柄标识。
* 参数： info 设备信息指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_GetDeviceInfo(whscom_device_handle device, whscom_device_info* info);

/**
* 描述： 设置设备信息。
* 参数： device 设备句柄标识。
* 参数： info 设备信息指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_SetDeviceInfo(whscom_device_handle device, const whscom_device_info* info);


//========================================================
#ifdef WIN32

//ClientInfo*
WHSCOM_LIB int WINAPI TEST_StartServer(void* info);

WHSCOM_LIB void WINAPI TEST_StopServer();

//PMsgData
WHSCOM_LIB int WINAPI TEST_SendCommand(void* pData);

//PMsgData
WHSCOM_LIB int WINAPI TEST_RecvCommand(void* pData);

//Download Temp
WHSCOM_LIB int WINAPI TEST_DownloadTemplate(whscom_ddr ddr, void* pData);

//Encode
WHSCOM_LIB int WINAPI TEST_Encode(int mode, unsigned char image[640 * 480], unsigned char tmp[8192]);

WHSCOM_LIB void WINAPI TEST_LogInfo(const char* msg);

#endif

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __WHSCOMINNER_LIB__
