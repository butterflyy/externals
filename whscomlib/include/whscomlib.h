/**
* (C) Wuhan Hongshi Technologies. Copyright (c) 2016
*
*
* NOTICE: The intellectual and technical concepts and all information and source codes contained herein
* are, and remains the property of the copyright holders and may be covered by Patents,
* patents in process, and are protected by trade secret or copyright law.
*
* This code is proprietary code. Redistribution and use of source and binary forms, with or without modifications
* and any form of dissemination of this information or reproduction of this material, even partially, is strictly
* forbidden unless prior written permission obtained from the author and copyright holders.
*/

#ifndef __WHSCOM_LIB__
#define __WHSCOM_LIB__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined(WHSCOM_LIB_EXPORTS)
#define WHSCOM_LIB
#else
#define WHSCOM_LIB __declspec(dllimport)
#endif
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#else
#define WHSCOM_LIB
#define CALLBACK
#define WINAPI
#include <stdbool.h> //for bool
#endif

/** 版本号 */
#define WHSCOMAPI_VERSION "2.1.0"


#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

	
/** 设备错误码 */
enum whscom_error_code {
	/** 操作成功 */
	WHSCOM_SUCCESS = 0,

	/** 一般错误 */
	WHSCOM_ERROR = -1,

	/** 参数无效 */
	WHSCOM_INVALID_PARAM = -2,

	/** 网络错误 */
	WHSCOM_NETWORK_ERROR = -3,

	/** 网络超时 */
	WHSCOM_NETWORK_TIMEOUT = -4,

	/** 断开连接 */
	WHSCOM_NETWORK_DISCONNECTED = -5,

	/** 数据包错误 */
	WHSCOM_PACKAGE_ERROR = -6,

	/** 设备错误 */
	WHSCOM_DEVICE_ERROR = -7,

	/** 设备未找到 */
	WHSCOM_DEVICE_NOT_FOUND = -8,

	/** 设备忙 */
	WHSCOM_DEVICE_BUSY = -9,
};

/** 比对卡DDR
 * 一块比对卡有两块DDR，都可以进行下载和比对操作
 */
enum whscom_ddr{
	WHSCOM_DDR_A = 0x01,
	WHSCOM_DDR_B = 0x02,
};

/** 设备句柄标识，用以操作设备*/
typedef void* whscom_device_handle;


/** 描述： 显示关于对话框，仅支持Windows平台，其他平台则是调用printf打印对话框的信息， （已废弃，替代接口 HS_GetLibVersion）  */
WHSCOM_LIB void WINAPI WHSCOM_About();

/**
* 描述： 设备成功接入后，触发此事件回调。
* 参数： device 设备句柄标识。
* 参数： ip 设备IP地址。
*/
typedef void(CALLBACK *whscom_connected_callback)(whscom_device_handle device, const char* ip);

/**
* 描述： 设备断开后，触发此事件回调。
* 参数： device 设备句柄标识。
*/
typedef void(CALLBACK *whscom_disconnected_callback)(whscom_device_handle device);

/**
* 描述： 设备错误后，触发此事件回调。
* 参数： device 设备句柄标识。
* 参数： error_code 错误码，参考 whscom_error_code。
*/
typedef void(CALLBACK *whscom_error_callback)(whscom_device_handle device, int error_code);

/**
* 描述： 初始化设备资源，必须在其他所有的API之前调用。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_Initialize();

/**
* 描述： 释放设备资源，和 HS_Initialize 对应，程序结束时调用，关闭所有设备并释放资源。
*/
WHSCOM_LIB void WINAPI WHSCOM_Finalize();

/**
* 描述： 获取库版本号。
* 返回： 版本号描述字符串，例如 1.3.2。
*/
WHSCOM_LIB const char* WINAPI WHSCOM_GetLibVersion();

/**
* 描述： 返回错误码的简单字符串描述。
* 参数： error_code 错误码，参考 hs_device_error_code。
* 返回： 错误字符串描述，例如 -1，返回 “HS_ERROR”。
*/
WHSCOM_LIB const char* WINAPI WHSCOM_StrError(int error_code);

/**
* 描述： 设置热插拔回调函数，HS_Initialize 成功后调用。
* 参数： on_usbhotplug 热插拔回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_SetCallback(whscom_connected_callback on_connect, 
										whscom_disconnected_callback on_disconnect,
										whscom_error_callback on_error);

/**
* 描述： 启动服务。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_StartServer();

/**
* 描述： 停止服务。
*/
WHSCOM_LIB void WINAPI WHSCOM_StopServer();


//=======================================================================================================
//                           下载模板相关API
//=======================================================================================================

/**
* 描述： 模板数据集合的指针。
*		 API内部管理数据结构，创建和释放由用户控制。
*/
typedef void* whscom_template_record;

/**
* 描述： 创建模板数据集合。
* 返回： 模板数据集合的指针。
*/
WHSCOM_LIB whscom_template_record WINAPI WHSCOM_CreateTemplateRecord();

/**
* 描述： 设置模板A。
* 参数： record 模板数据集合的指针
* 参数： index 模板数据存储在DDR中的序号, 取值范围 0-200000， 0为比对模板1， 1-200000为比对模板N， 即1比N的比对模式。
* 参数： template_ptr 模板数据的指针。
* 参数： length 模板数据的大小。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_AddTemplate(whscom_template_record record, int index, 
	unsigned char* template_ptr, int length);

/**
* 描述： 释放比对数据集合。
* 参数： record 模板数据集合的指针
*/
WHSCOM_LIB void WINAPI WHSCOM_ReleaseTemplateRecord(whscom_template_record record);

/**
* 描述：下载模板。
* 参数： device 设备句柄标识。
* 参数： ddr 设备DDR，参考 whscom_ddr。
* 参数： record 模板数据集合的指针
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_DownloadTemplate(whscom_device_handle device, 
											  whscom_ddr ddr, 
											  whscom_template_record record);


//======================================================================================================


/**
* 描述：删除模板。
* 参数： device 设备句柄标识。
* 参数： ddr 设备DDR，参考 whscom_ddr。
* 参数： indexs 删除的模板位置列表
* 参数： size 删除的模板个数
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_RemoveTemplate(whscom_device_handle device, whscom_ddr ddr, 
											int indexs[], int size);


/**
* 描述： 模板比对。
* 参数： device 设备句柄标识。
* 参数： ddr 设备DDR，参考 whscom_ddr。
* 参数： begin 比对开始的位置，取值范围（1 - 200000）。
* 参数： end 比对结束的位置，必须大于等于开始的位置，取值范围（1 - 200000）。
* 参数： result_indexs 比对结果模板位置。
* 参数： result_size 比对结果个数。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_Match(whscom_device_handle device, 
									whscom_ddr ddr, 
									int begin, 
									int end,
									int result_indexs[16], 
									int* result_size);


/**
* 描述： 模板比对。
* 参数： device 设备句柄标识。
* 参数： ddr 设备DDR，参考 whscom_ddr。
* 参数： begin 比对开始的位置，取值范围（1 - 200000）。
* 参数： end 比对结束的位置，必须大于等于开始的位置，取值范围（1 - 200000）。
* 参数： results 比对结果分数列表，完全匹配分数：16384， 完全不匹配分数：0。
* 参数： size 比对结果的个数。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_Match2(whscom_device_handle device,
	whscom_ddr ddr,
	int begin,
	int end,
	int results_score[],
	int results_angle[],
	int results_valnm[],
	int size);


/**
* 描述： 清除设备，清除所有下载的模板（DDR A和DDR B的数据同时清除）。
* 参数： device 设备句柄标识。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSCOM_LIB int WINAPI WHSCOM_ClearDevice(whscom_device_handle device);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __WHSCOM_LIB__
