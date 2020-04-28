/**
* (C) Wuhan Hongshi Technologies. Copyright (c) 2018
*
* @Author: Tomas Fernandes  <fernandes@hongshi-tech.com>
*          Yonggang Liu <yonggang.liu@hongshi-tech.com>
*
* NOTICE: The intellectual and technical concepts and all information and source codes contained herein
* are, and remains the property of the copyright holders and may be covered by Patents,
* patents in process, and are protected by trade secret or copyright law.
*
* This code is proprietary code. Redistribution and use of source and binary forms, with or without modifications
* and any form of dissemination of this information or reproduction of this material, even partially, is strictly
* forbidden unless prior written permission obtained from the author and copyright holders.
*/

#ifndef __WHSUSB_API__
#define __WHSUSB_API__

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined(WHSUSB_API_EXPORTS)
# define WHSUSB_API
#else
# define WHSUSB_API __declspec(dllimport)
#endif
# define CALLBACK    __stdcall
# define WINAPI      __stdcall
#else
# define WHSUSB_API
# define CALLBACK
# define WINAPI
#include <stdbool.h> //for bool
#endif


/** 
* whsusbapi 库版本号 
* 头文件声明的库版本号， 通过 HS_GetLibVersion 接口获取实际加载库的版本号。
*/
#define WHSUSBAPI_VERSION "4.5.9"

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus

/** 设备错误码 */
enum hs_device_error_code {
    /** 操作成功 */
	HS_SUCCESS = 0,

	/** 一般错误 */
	HS_ERROR = -1,

	/** 参数无效 */
	HS_ERROR_INVALID_PARAM = -2,

	/** 内存不足 */
	HS_ERROR_NO_MEMORY = -3,

	/** 无效的比对数据集合 */
	HS_ERROR_BAD_RECORD = -4,

	/** 无效的比对ID数据集合 */
	HS_ERROR_BAD_ID_ARRAY = -5,

	/** 模板数据错误 */
	HS_ERROR_TEMPLATE = -6,

	/** 没有模板B */
	HS_ERROR_NO_TEMPLATE_B = -7,

	/** 库不支持当前设备 */
	HS_ERROR_LIB_INCOMPATIBLE = -8,

	/** USB错误 */
    HS_ERROR_USB = -101,

	/** USB IO错误 */
    HS_ERROR_USB_IO = -102,

	/** USB权限不够 */
    HS_ERROR_USB_ACCESS = -103,

	/** USB操作超时 */
    HS_ERROR_USB_TIMEOUT = -104,

	/** USB PIPE错误 */
    HS_ERROR_USB_PIPE = -105,

	/** USB接口不支持 */
    HS_ERROR_USB_NOT_SUPPORTED = -106,

	/** 设备错误 */
    HS_ERROR_DEVICE = -201,

	/** 设备未找到 */
    HS_ERROR_DEVICE_NOT_FOUND = -202,

	/** 设备忙 */
    HS_ERROR_DEVICE_BUSY = -203,

	/** 设备数据包错误 */
    HS_ERROR_DEVICE_PACKAGE = -204,

	/** 设备数据包类型错误 */
    HS_ERROR_DEVICE_DATA_TYPE = -205,

	/** 设备硬件错误 */
    HS_ERROR_DEVICE_HARDWARE = -206,

	/** 设备数据包大小错误 */
	HS_ERROR_DEVICE_DATA_SIZE = -207,
};

/** 设备状态值 */
enum hs_device_status {
    /** 空闲 */
    HS_IDLE = 0x01,

    /** 采集中，当设备启动采集后，设备会进入此状态 */
    HS_CAPTURING = 0x02,

    /** 比对中，当设备启动比对后，设备会进入此状态 */
    HS_MATCHING = 0x04,

    /** 休眠中，一段时间不操作设备，设备会进入此状态 */
    HS_SLEEPING = 0x08,
};


/** USB设备热插拔事件 */
enum hs_usb_hotplug_event {
    /** 设备成功接入 */
    HS_USB_HOTPLUG_EVENT_DEVICE_ARRIVED = 0x01,

    /** 设备断开或者不可用 */
    HS_USB_HOTPLUG_EVENT_DEVICE_LEFT = 0x02,
};


/* 采集的眼睛类型 */
enum hs_eye_type {
    /** 不确定，单眼设备支持 */
    HS_UNKNOW_EYE = 0x01,

    /** 左眼，双眼设备支持 */
    HS_LEFT_EYE = 0x02,

    /** 右眼，双眼设备支持 */
    HS_RIGHT_EYE = 0x04,
};

/* 比对模式 */
enum hs_match_mode{
	/** 只比对一次, 成功或者失败后返回，调用 HS_StopMatch 可以结束比对过程 */
	HS_MATCH_ONCE = 0x01,

	/** 连续比对模式 , 只有比对成功后, 才会返回，只对不设置模板A的比对模式有效，调用 HS_StopMatch 可以结束比对过程 */
	HS_MATCH_CONTINUOUS = 0x02,
};

/**
* 描述： 设备成功接入或者断开后，触发此事件回调。
* 参数： index 设备序列号，当新的设备接入时，由于还没有分配设备资源，序列号为-1。
* 参数： event 热插拔事件类型，参考 hs_usb_hotplug_event。
*/
typedef void(CALLBACK* usb_hotplug_callback)(int index, int hotplug_event);

/**
* 描述： 设备进入休眠模式后，触发此事件回调。
        默认设备进入休眠时间为300秒，当设备一段时间不操作后，自动停止回显，采集，比对过程，进入休眠模式，设备的红外线灯会熄灭，功耗降低。
		当进行采集，比对或者回显操作后，设备会自动唤醒。
* 参数： index 设备序列号。
*/
typedef void(CALLBACK* sleep_mode_callback)(int index);

/**
* 描述： 设备在采集或者比对的过程中出现错误，触发此事件回调。
* 参数： index 设备序列号。
* 参数： error_code 错误码，参考 hs_device_error_code。
*/
typedef void(CALLBACK* error_callback)(int index, int error_code);

/**
* 描述： 采集成功后，触发此事件回调，返回眼睛模板，眼睛图片。
* 参数： index 设备序列号。
* 参数： type 眼睛类型，参考 hs_eye_type。
* 参数： image_ptr 眼睛图片数据的指针，图片格式为灰度图片，具体处理过程参考相关示例。
* 参数： width 图片的宽度。
* 参数： height 图片的高度。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： length 眼睛模板数据的大小。
* 注意： 标准虹膜图片的格式为8位灰度索引，格式为bmp。为了通用性，建议图片保存为此格式。
*/
typedef void(CALLBACK* image_callback)(int index, int type,
                                       const unsigned char* image_ptr, int width, int height,
                                       const unsigned char* template_ptr, int length);

/**
* 描述： 比对完成后，触发此事件回调，返回比对结果。
* 参数： index 设备序列号。
* 参数： id_array 比对数据ID集合，配合 HS_MatchIdClips 获取具体的ID数据。
* 参数： size 比对ID数据中ID个数，配合 HS_MatchIdClips 获取具体的ID数据。
* 参数： result 比对结果，true 成功，false 失败，为true的时候，id_array 和 size 参数才有意义。
* 注意： 返回的id列表是去重的, 并且是按照比对分数由高到低进行排序, 所以比对最高分就是第一个id。
*/
typedef void(CALLBACK* match_callback)(int index, const void* id_array, int size, bool result);

/**
* 描述： 采集过程中，触发此事件回调，返回采集状态信息。
* 参数： index 设备序列号。
* 参数： status 采集状态， 0 不在有效采集区域中， 1 在有效采集区域中。
* 注意： 此回调为同步回调，不能在回调中做过多操作，否则会阻塞采集过程!
*/
typedef void(CALLBACK* capturing_callback)(int index, int status);

/**
* 描述： 获取比对数据ID集合中ID数据。
* 参数： id_array 比对数据ID集合。
* 参数： index 指定获取的ID的序号。
* 返回： ID字符串，最大长度为32个字节。
*/
WHSUSB_API const char* WINAPI HS_MatchIdClips(const void* id_array, int index);

/**
* 描述： 初始化设备资源，必须在其他所有的API之前调用。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_Initialize();

/**
* 描述： 释放设备资源，和 HS_Initialize 对应，程序结束时调用，关闭所有设备并释放资源。
*/
WHSUSB_API void WINAPI HS_Finalize();

/**
* 描述： 获取库版本号，库版本是向下兼容的，即高版本库兼容低版本库。
* 返回： 版本号描述字符串，例如 1.3.2。
*/
WHSUSB_API const char* WINAPI HS_GetLibVersion();


/**
* 描述： 返回错误码的简单字符串描述。
* 参数： error_code 错误码，参考 hs_device_error_code。
* 返回： 错误字符串描述，例如 -1，返回 “HS_ERROR”。
*/
WHSUSB_API const char* WINAPI HS_StrError(int error_code);

/**
* 描述： 设置热插拔回调函数，HS_Initialize 成功后调用。
* 参数： on_usbhotplug 热插拔回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetUsbHotPlugCallback(usb_hotplug_callback on_usbhotplug);

/**
* 描述： 设置休眠回调函数，HS_Initialize 成功后调用。
* 参数： on_sleep 休眠回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetSleepModeCallback(sleep_mode_callback on_sleep);

/**
* 描述： 设置错误回调函数，HS_Initialize 成功后调用。
* 参数： on_error 采集回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetErrorCallback(error_callback on_error);

/**
* 描述： 设置采集回调函数，HS_Initialize 成功后调用。
* 参数： on_capture 采集回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetImageCallback(image_callback on_capture);

/**
* 描述： 设置比对回调函数，HS_Initialize 成功后调用。
* 参数： on_match 比对回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetMatchCallback(match_callback on_match);


/**
* 描述： 设置采集状态回调函数，HS_Initialize 成功后调用。
* 参数： on_capturing 采集状态函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
* 注意： 此回调为同步回调，不能在回调中做过多操作，否则会阻塞采集过程!
*/
WHSUSB_API int WINAPI HS_SetCapturingCallback(capturing_callback on_capturing);


/**
* 描述： 获取设备列表。
*        获取所有接入的设备，并打开设备，当有新的设备接入后，需要重新调用此函数，才能操作设备。
*        返回可用设备的个数，当操作设备时，用序列号指定设备，从0开始。
* 返回： 大于或者等于0成功，表示设备个数，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetDeviceList();


/**
* 描述： 获取设备状态。
* 参数： index 设备序列号。
* 返回： 大于0成功，参考 hs_device_status，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetDeviceStatus(int index);


/**
* 描述： 获取设备名称。
* 参数： index 设备序列号。
* 返回： 设备名称，最大长度为48字节。
*/
WHSUSB_API const char* WINAPI HS_GetDeviceName(int index);

/**
* 描述： 获取设备ID。
* 参数： index 设备序列号。
* 返回： 设备ID，长度为32字符。
*/
WHSUSB_API const char* WINAPI HS_GetDeviceID(int index);

/**
* 描述： 获取设备硬件版本号。
* 参数： index 设备序列号。
* 返回： 版本号描述字符串，例如 1.3.2。
*/
WHSUSB_API const char* WINAPI HS_GetDeviceVersion(int index);

/**
* 描述： 获取设备描述信息。
* 参数： index 设备序列号。
* 返回： 描述信息，最大长度为1024字节。
*/
WHSUSB_API const char* WINAPI HS_GetDeviceDescriptor(int index);

/**
* 描述： 获取模板大小。
* 参数： index 设备序列号。
* 返回： 大于0成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetTemplateSize(int index);

/**
* 描述： 获取眼睛图片的宽度。
* 参数： index 设备序列号。
* 返回： 大于0成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetImageWidth(int index);

/**
* 描述： 获取眼睛图片的高度。
* 参数： index 设备序列号。
* 返回： 大于0成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetImageHeight(int index);

/**
* 描述： 获取回显视频帧图片的宽度。
* 参数： index 设备序列号。
* 返回： 大于0成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetVideoWidth(int index);

/**
* 描述： 获取回显视频帧图片的高度。
* 参数： index 设备序列号。
* 返回： 大于0成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_GetVideoHeight(int index);

/**
* 描述： 开始采集。
*        开始采集成功后，设备会进入采集状态中，如果采集眼睛的数量无限制，采集不会停止，除非调用 HS_StopCapture 关闭采集。
*        如果采集的数量有限制，当达到采集指定的数量后，采集会停止，设备进入空闲状态。
*        采集过程中，如果调用 HS_StartMatch 会返回失败（设备忙）。
* 参数： index 设备序列号。
* 参数： eye_count 采集的眼睛个数，-1表示无限制，如果设备为双眼设备，采集的眼睛个数为双眼的个数。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_StartCapture(int index, int eye_count);


/**
* 描述： 停止采集。
* 参数： index 设备序列号。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_StopCapture(int index);

/**
* 描述： 比对数据集合的指针。
*        API内部管理数据结构，创建和释放由用户控制。
*/
typedef void* MATCH_RECORD;

/**
* 描述： 创建比对数据集合。
* 返回： 比对数据集合的指针。
*/
WHSUSB_API MATCH_RECORD WINAPI HS_CreateMatchRecord();

/**
* 描述： 设置模板A。
* 参数： record 比对数据集合的指针。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： length 眼睛模板数据的大小。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetTemplateA(MATCH_RECORD record, const unsigned char* template_ptr, int length);

/**
* 描述： 插入模板B。
* 参数： record 比对数据集合的指针。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： length 眼睛模板数据的大小。
* 参数： id 用户自定义的模板ID(可重复)，比对成功后，比对回调里面会返回这个ID，最大长度为32个字节。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_InsertTemplateB(MATCH_RECORD record, const unsigned char* template_ptr, int length, const char* id);

/**
* 描述： 删除指定ID的模板B。
* 参数： record 比对数据集合的指针。
* 参数： id 用户自定义的模板ID，最大长度为32个字节。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_RemoveTemplateB(MATCH_RECORD record, const char* id);

/**
* 描述： 释放比对数据集合。
* 参数： record 比对数据集合的指针。
*/
WHSUSB_API void WINAPI HS_ReleaseMatchRecord(MATCH_RECORD record);

/**
* 描述： 开始比对。
*        开始采集成功后，设备会进入比对状态中，比对有两种模式，一种模式是设置模板A，那么比对过程中不需要采集用户模板，直接完成比对过程；
*        另一种模式是不设置模板A，那么需要采集用户模板，然后和模板B进行比对。
*        比对过程中，如果调用 HS_StartCapture 或者 HS_SetVideoEnabled 会返回失败（设备忙）。
* 参数： index 设备序列号。
* 参数： mode 比对模式， 参考 hs_match_mode， 连续比对模式 HS_MATCH_CONTINUOUS 只对不设置模板A的模式有效。
* 参数： record 比对数据集合的指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_StartMatch(int index, int mode, MATCH_RECORD record);

/**
* 描述： 停止比对。
* 参数： index 设备序列号。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_StopMatch(int index);

/**
* 描述： 回显视频是否开启。
* 参数： index 设备序列号。
* 参数： enable 是否开启回显视频。
* 返回： true 回显视频开启，false 回显视频关闭或者获取失败。
*/
WHSUSB_API bool WINAPI HS_IsVideoEnabled(int index);

/**
* 描述： 设置回显视频开启或者关闭。
* 参数： index 设备序列号。
* 参数： enabled 是否开启。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetVideoEnabled(int index, bool enabled);

/**
* 描述： 是否有新的回显视频帧。
*        当有新的帧后，调用 HS_GetVideoImage 获取具体的图片数据，图片格式为灰度图片，具体处理过程参考相关示例。
* 参数： index 设备序列号。
* 返回： true 有新的回显视频帧，false 没有新的回显视频帧或者获取失败。
*/
WHSUSB_API bool WINAPI HS_IsNewFrame(int index);

/**
* 描述： 获取回显视频帧数据。
* 参数： index 设备序列号。
* 返回： 回显视频帧数据的指针，如果为NULL，没有数据或者获取失败。
*/
WHSUSB_API const unsigned char* WINAPI HS_GetVideoImage(int index);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //   __WHSUSB_API__
