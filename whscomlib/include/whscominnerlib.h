#ifndef __WHSCOMINNER_LIB__
#define __WHSCOMINNER_LIB__


#include "whscomlib.h"

#define BUILD_RELEASE     1  //when build release, increase 1.

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus


/** �豸��Ϣ*/
struct whscom_device_info{
	char ip[16]; //�豸IP��ַ
	int port; //�豸�˿�
	unsigned char mac[6]; //�豸MAC��ַ
	int broad_id; //�豸�忨ID
	char server_ip[16]; //������IP��ַ
	int server_port; //�������˿�
	unsigned char key[32]; //�豸KEY
	bool key_valid; //�豸KEYʹ��
	int match_win_mask_th; //win mask��ֵ, Ĭ�� 64
	int match_valnm_th;//valnm��ֵ, Ĭ�� 6554
	int match_score_th;//score��ֵ, Ĭ�� 6226
	int match_sf;//Ĭ��AE1, 2785
	int match_max_v;//Ĭ��2DB6��11702
};



/**
* ������ ��ȡ�豸��Ϣ��
* ������ device �豸�����ʶ��
* ������ info �豸��Ϣָ�롣
* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� hs_device_error_code��
*/
WHSCOM_LIB int WINAPI WHSCOM_GetDeviceInfo(whscom_device_handle device, whscom_device_info* info);

/**
* ������ �����豸��Ϣ��
* ������ device �豸�����ʶ��
* ������ info �豸��Ϣָ�롣
* ���أ� 0 �ɹ�������ֵʧ�ܣ��ο� hs_device_error_code��
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
