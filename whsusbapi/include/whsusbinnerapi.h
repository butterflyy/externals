#ifndef __WHSUSBINNER_API__
#define __WHSUSBINNER_API__

#include "whsusbapi.h"

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;

typedef unsigned char      byte;

#ifdef __cplusplus
extern "C" {
#endif   // __cplusplus


#pragma pack (push, 1)



enum hs_encode_mode{
	ENCODE_MIX,  /* 单目设备采集参数，双目设备不使用此参数 */
	ENCODE_LEFT, /* 左眼采集 */
	ENCODE_RIGHT, /* 右眼采集 */
	ENCODE_DOUBLE, /* 双眼采集 */
};

#define VIDEO_X_LIMIT 80
#define VIDEO_Y_LIMIT 240

struct hs_video_offset{
	int16 left_x;
	int16 left_y;
	int16 right_x;
	int16 right_y;
};

struct DeviceInfo {
	bool is_doubleeye;
	uint16  video_width;
	uint16  video_height;
	uint16  image_width;
	uint16  image_height;
	uint16  template_size;
	uint16  match_thresold;
	byte AESKEY[32];
    bool KEY_enable;
	hs_video_offset video_offset;
	uint16 device_model;
	uint32 device_serial;
};


struct hs_encode_flag{
	byte error_flag;
	uint16 warn1_flag;
	uint16 warn2_flag;
	uint16 warn3_flag;
};

struct hs_encode_param{
	//压缩成256 * 256的眼睛中心位置
	uint16 down_sample_x; //down sample pupil estimate center, UFIX_8_0. cx
	uint16 down_sample_y; //down sample pupil estimate center, UFIX_8_0. cy

	//768*768的眼睛位置
	uint16 full_sample_x; //full sample pupil estimate center, UFIX_8_0. cx
	uint16 full_sample_y; //full sample pupil estimate center, UFIX_8_0. cy

	//768*768虹膜中瞳孔的 位置，半径
	double pupil_circle_x; //pupil circle, UFIX_8_4. cx
	double pupil_circle_y; //pupil circle, UFIX_8_4. cy
	double pupil_circle_r; //pupil circle, UFIX_8_4. r
	double pupil_circle_o; //pupil circle, UFIX_8_4. (pupil_q_o)

	//768*768虹膜中外缘的 位置，半径
	double iris_circle_x;  //iris circle, UFIX_8_4. cx
	double iris_circle_y;  //iris circle, UFIX_8_4. cy
	double iris_circle_r;  //iris circle, UFIX_8_4. r
	double iris_circle_o;  //iris circle, UFIX_8_4. (iris_q_o)

	//Y = a (x - b)-2 + c
	uint16 eyelid_upper_a; //eyelid upper. (eyelid_u_a_o)
	uint16 eyelid_upper_b; //eyelid upper. (eyelid_u_b_o)
	uint16 eyelid_upper_c; //eyelid upper. (eyelid_u_c_o)
	uint16 eyelid_upper_e; //eyelid upper. (eyelid_u_e_o)

	uint16 eyelid_lower_a; //eyelid lower. (eyelid_l_a_o)
	uint16 eyelid_lower_b; //eyelid lower. (eyelid_l_b_o)
	uint16 eyelid_lower_c; //eyelid lower. (eyelid_l_c_o)
	uint16 eyelid_lower_e; //eyelid lower. (eyelid_l_e_o)

	//眼皮像素点
	uint16 upper_angle;  //upper eyelid covered angle
	uint16 lower_angle;  //lower eyelid covered angle
	uint16 left_angle;   //uncover left angle
	uint16 right_angle;  //uncover right angle

	//平均灰度
	uint16 gray;  //mean value of iris gray

	//瞳孔中心到虹膜中心的距离 / （虹膜半径大小 - 瞳孔半径大小）
	double distance_ip;  //distance / (iris_r – pupil_r), UFIX_1_8, [0, 1]
	uint16 distance;     //distance between pupil center and iris center

	//瞳孔半径 / 虹膜半径
	double dialoation;   //dialoation, UFIX_1_8, [0, 1]

	//瞳孔角度
	double margin_left;  //margin, UFIX_2_8, [0, 2]. Left
	double margin_right; //margin, UFIX_2_8, [0, 2]. Right
	double margin_top;   //margin, UFIX_2_8, [0, 2]. Top
	double margin_bottom; //margin, UFIX_2_8, [0, 2]. Bottom

	double focus;   //清晰度

	uint16 valid;   //有效像素点

	uint16 quality; //质量等级

	struct hs_encode_flag flag;
};

struct hs_quality_param{
	float gray_utilisation;
	int iris_radius;
	float iris_sclera_contrast;
	float iris_pupil_contrast;
	float usable_iris_area;

	int margin_left;
	int margin_right;
	int margin_top;
	int margin_bottom;

	int quality_score;
};


#define IMAGE_NORMAL_WIDTH      640
#define IMAGE_NORMAL_HEIGHT     480
#define IMAGE_BIG_WIDTH         960
#define IMAGE_BIG_HEIGHT        960
#define IMAGE_ENCODE_WIDTH      768
#define IMAGE_ENCODE_HEIGHT     768


struct hs_time{
	uint32 year;
	uint32 mon;
	uint32 day;
	uint32 hour;
	uint32 min;
	uint32 sec;
};
struct hs_image_info{
	uint16 organization;   //组织编号，目前未使用
	uint16 identifier;     //算法标识，目前未使用
	byte lversion[3];      //库版本号
	byte dversion[3];      //设备版本号
	byte id[16];           //采集设备ID
	hs_time time;          //采集时间，格式为 YYYY
	byte resvert[8];       //保留字段
	hs_encode_param param; //编码参数
	hs_quality_param quality; //编码质量
	byte check_quality_ok;    //是否满足设定的质量等级要求
};

enum UserCommand{
	UCMD_CAPTURING,
	UCMD_ENCODE_FLAG,
	UCMD_DEVICE_ERROR,
	UCMD_BUSSNESS_ERROR,
};

enum Command {
	CMD_FPGA_GPIO_UP,                            /* FPGA GPIO up  */
	CMD_FPGA_GPIO_DOWN,                          /* FPGA GPIO down */
	CMD_FPGA_GPIO_RESET,                         /* FPGA GPIO down and up*/
	CMD_FPGA_POWER_ON,                           /* FPGA power on  */
	CMD_FPGA_POWER_OFF,                          /* FPGA power off */
	CMD_FX3_SYSTEM_RESET,                        /* fx3 & fpga Reset */
};

#pragma pack (pop)


/**
* 描述： 用户自定义命令。
* 参数： index 设备序列号。
* 参数： command 命令类型。
* 参数： data 命令数据。
*/
typedef void(CALLBACK* user_command_callback)(int index, int command, void* data);


/**
* 描述： 采集成功后，触发此事件回调，返回眼睛模板，眼睛图片。
* 参数： index 设备序列号。
* 参数： type 眼睛类型，参考 hs_eye_type。
* 参数： image_normal_ptr 640*480图像数据。
* 参数： image_big_ptr 960*960图像数据。
* 参数： image_encode_ptr 768*768图像数据。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： param 图片参数
* 参数： quality 图片质量
* 参数： check_quality_ok 是否通过质量检测
* 注意： 标准虹膜图片的格式为8位灰度索引，格式为bmp。为了通用性，建议图片保存为此格式。
*/
	typedef void(CALLBACK* image2_callback)(int index, 
		int type,
		const byte* image_normal_ptr, 
		const byte* image_big_ptr,
		const byte* image_encode_ptr,
		const byte* template_ptr,
		const struct hs_encode_param* param,
		const struct hs_quality_param* quality,
		bool check_quality_ok);

/**
* 描述： 设置采集回调函数，HS_Initialize 成功后调用。
* 参数： on_capture 采集回调函数指针。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetImage2Callback(image2_callback on_capture);


/** 描述： 显示版本号关于对话框，仅支持Windows平台，其他平台则是调用printf打印对话框的信息， （已废弃，替代接口 HS_GetLibVersion） */
WHSUSB_API void WINAPI About();


/**
* 描述： 设备是否支持双眼。 （已废弃，目前只生产双目设备了）
* 参数： index 设备序列号。
* 返回： true 支持双眼，false，不支持双眼或者获取失败。
*/
WHSUSB_API bool WINAPI HS_IsSupportDoubleEye(int index);


/**
* 描述： 获取设备ID。
* 参数： index 设备序列号。
* 返回： 设备ID，长度为16字节。
*/
WHSUSB_API const unsigned char* WINAPI HS_GetDeviceID2(int index);


/**
* 描述： 插入模板B。
* 参数： record 比对数据集合的指针。
* 参数： type 眼睛类型，参考 hs_eye_type。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： length 眼睛模板数据的大小。
* 参数： id 用户自定义的模板ID，比对成功后，比对回调里面会返回这个ID，最大长度为32个字节。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_InsertTemplateB2(MATCH_RECORD record, int type, const unsigned char* template_ptr, int length, const char* id);


/**
* 描述： 图片编码模板。
* 参数： index 设备序列号。
* 参数： image_ptr 眼睛图片数据的指针，图片格式为灰度图片，具体处理过程参考相关示例。
* 参数： width 图片的宽度。
* 参数： height 图片的高度。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： length 眼睛模板数据的大小。
* 返回： 0 成功，1 编码失败， 2 编码超时  其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_Encode(int index, const byte* image_ptr, int width, int height, byte* template_ptr, int length);

/**
* 描述： 图片编码模板。
* 参数： index 设备序列号。
* 参数： image_ptr 眼睛图片数据的指针，图片格式为灰度图片，具体处理过程参考相关示例。
* 参数： width 图片的宽度。
* 参数： height 图片的高度。
* 参数： template_ptr 眼睛模板数据的指针。
* 参数： length 眼睛模板数据的大小。
* 参数： hs_encode_param  编码参数
* 返回： 0 成功，1 编码失败， 2 编码超时  其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_Encode2(int index, const byte* image_ptr, int width, int height, 
	byte* template_ptr, int length, struct hs_encode_param* param);


/**
* 描述： 读取图片数据中的参数。
* 参数： image_ptr 眼睛图片数据的指针。
* 参数： hs_image_info  图像信息，包括版本号，编码参数等。
* 返回： true 成功，false 没有参数。
*/
WHSUSB_API bool WINAPI HS_ReadImageInfo(const byte* image_ptr, struct hs_image_info* info);


/**
* 描述： VR灯光控制。
* 参数： index 设备序列号。
* 参数： enabled true 启用程序控制,按键控制将不可用; false 禁用程序控制,按键将可用。
* 参数： level 灯的亮度等级  0 关闭； 1 暗； 2 中； 3 亮； 4 更亮。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetLight(int index, bool enabled, int level);


/**
* 描述： 开始采集。
*        开始采集成功后，设备会进入采集状态中，如果采集眼睛的数量无限制，采集不会停止，除非调用 HS_StopCapture 关闭采集。
*        如果采集的数量有限制，当达到采集指定的数量后，采集会停止，设备进入空闲状态。
*        采集过程中，如果调用 HS_StartMatch 会返回失败（设备忙）。
* 参数： index 设备序列号。
* 参数： eye_count 采集的眼睛个数，-1表示无限制，如果设备为双眼设备，采集的眼睛个数为双眼的个数。
* 参数： mode 采集模式，左眼采集，右眼采集，或者双眼采集，参考 hs_encode_mode。
* 参数： fource 是否强制采集。
* 参数： light 是否自动开启红外光（C20E设备支持功能）， true 自动开启红外光采集，结束后自动关闭； 
		false 不管红外光，可以使用 HS_SetInfrared 控制灯光常开或者常关。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_StartCapture2(int index, int eye_count, int mode, bool fource, bool light);


/**
* 描述： 强制采集。
* 参数： index 设备序列号。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_StartCaptureForce(int index);


/**
* 描述： 双镜头设备，分别获取左右眼回显视频帧数据。
* 参数： index 设备序列号。
* 参数： type 双目设备回显侦的左眼视频，或者右眼视频，参考 hs_eye_type。
* 返回： 回显视频帧数据的指针，如果为NULL，没有数据或者获取失败。
*/
WHSUSB_API const unsigned char* WINAPI HS_GetVideoImageSeparate(int index, int* type);


//match
typedef void* MATCH_RESULT;
WHSUSB_API MATCH_RESULT WINAPI HS_CreateMatchResult();
WHSUSB_API int WINAPI HS_MatchResultSize(MATCH_RESULT result);
WHSUSB_API int WINAPI HS_MatchResultScore(MATCH_RESULT result, int index);
WHSUSB_API int WINAPI HS_MatchResultAngle(MATCH_RESULT result, int index);
WHSUSB_API int WINAPI HS_MatchResultValnm(MATCH_RESULT result, int index);
WHSUSB_API void WINAPI HS_ReleaseMatchResult(MATCH_RESULT result);
WHSUSB_API int WINAPI HS_Match(int index, MATCH_RECORD record, MATCH_RESULT result);

WHSUSB_API int WINAPI HS_SetSleepModeEnabled(bool enabled, int duration);

WHSUSB_API int WINAPI HS_SetUserCommandCallback(user_command_callback on_user_command);

//CONFIG FILE SET==============================================
//enabled true generate file, false do not generate file.
//must call before HS_Initialize
WHSUSB_API const char* WINAPI HS_GetDllDir();

WHSUSB_API int WINAPI HS_ConfigSetPath(bool enabled, const char* path);

//是否使用老的左右眼类型，和新版本的左右眼相反（已废弃）
WHSUSB_API int WINAPI HS_ConfigSetOldEyetypeEnabled(bool enabled);

//HS_IMAGE_UPPER_LOW = 0,
//HS_IMAGE_LOW = 1,
//HS_IMAGE_MIDDLE = 2,
//HS_IMAGE_HIGHT = 3,
WHSUSB_API int WINAPI HS_ConfigSetCaptureImageQuality(int quality);
WHSUSB_API int WINAPI HS_ConfigSetMatchImageQuality(int quality);
WHSUSB_API int WINAPI HS_ConfigSetEncodeImageQuality(int quality);

//const int INFO = 0, WARNING = 1, ERROR = 2, FATAL = 3
WHSUSB_API int WINAPI HS_LogInfo(int level, const char* info);


//升级进度回调函数
typedef void(CALLBACK* firmware_upgrade_callback)(int total, int current);

WHSUSB_API int WINAPI HS_FirmwareUpgrade(int index, byte* bin, int size, firmware_upgrade_callback cb);

WHSUSB_API int WINAPI HS_SetStartupLogo(int index, byte* logo, int size);

WHSUSB_API int WINAPI HS_GetDeviceInfo(int index, DeviceInfo& info);

WHSUSB_API int WINAPI HS_SetDeviceInfo(int index, const DeviceInfo& info);

//960 image support
WHSUSB_API int WINAPI HS_SetReadInnerImageMode(int index, bool enable);


/**
* 描述： 红外光控制（c20e设备支持）。
* 参数： index 设备序列号。
* 参数： enable true 开启红外光， false 关闭红外光。
* 返回： 0 成功，其他值失败，参考 hs_device_error_code。
*/
WHSUSB_API int WINAPI HS_SetInfrared(int index, bool enable);

WHSUSB_API int WINAPI HS_DebugVideoAdjustOffset(int index, hs_video_offset offset);

WHSUSB_API int WINAPI HS_SetVideoAdjustOffset(int index, hs_video_offset offset);

//Device model,  1 C20e ; 2 C20b ; 3 C21;  4 C41  other undefined
WHSUSB_API int WINAPI HS_GetDeviceModel(int index);
WHSUSB_API int WINAPI HS_SetDeviceModel(int index, int model);

WHSUSB_API int WINAPI HS_GetDeviceSerial(int index);
WHSUSB_API int WINAPI HS_SetDeviceSerial(int index, int serial);

//读取fpga flash中可配置参数。
WHSUSB_API int WINAPI HS_ReadBinParam(int index, byte* data, int size);



/**
* @brief:  Write raw bmp data to file.
* @method:  HS_WriteBmpData
* @param: const char * path
* @param: const byte * image_ptr
* @param: int width
* @param: int height
* @return:   WHSUSB_API int WINAPI
*/
WHSUSB_API int WINAPI HS_WriteBmpData(const char* path, const byte* image_ptr, int width, int height);


/**
* @brief:    Read bmp raw data from file.
* @method:  HS_ReadBmpData
* @param: const char * path
* @param: byte * image_ptr
* @param: int size
* @param: int * width
* @param: int * height
* @return:   WHSUSB_API int WINAPI
*/
WHSUSB_API int WINAPI HS_ReadBmpData(const char* path, byte* image_ptr, int size, int* width, int* height);

//========================================================
#ifdef WIN32

WHSUSB_API int WINAPI HS_SendCommand(int index, Command cmd);

//read and write rigster
WHSUSB_API int WINAPI HS_RigsterRead2(int index, uint32 addr, byte* data, int len);

WHSUSB_API int WINAPI HS_RigsterWrite2(int index, uint32 addr, const byte* data, int len);

WHSUSB_API int WINAPI HS_RigsterRead3(int index, uint32 addr, uint32* data);

WHSUSB_API int WINAPI HS_RigsterWrite3(int index, uint32 addr, uint32 data);

WHSUSB_API int WINAPI HS_WriteImage(int index, byte* image, int size, int width, int height);

WHSUSB_API int WINAPI HS_GetImage(int index, byte* image, int size, int width, int height);

WHSUSB_API int WINAPI HS_GetTemplate(int index, byte* tmpl, int size);

WHSUSB_API int WINAPI HS_WriteTemplateA(int index, byte* tmpl, int size);

WHSUSB_API int WINAPI HS_WriteTemplateB(int index, byte* tmpl, int size);

WHSUSB_API void WINAPI HS_UsbRead(int index, char* outmsg);

WHSUSB_API void WINAPI HS_UsbWrite(int index, char* outmsg);

WHSUSB_API void WINAPI HS_VideoRead(int index, char* outmsg);

#endif

#ifdef __ANDROID__
//check android verion is root.
WHSUSB_API bool WINAPI HS_IsRootVersion();
#endif

#ifdef __ANDROID__ 

struct libusb_device_handle;


typedef int(CALLBACK* libusb_control_transfer_callback)(struct libusb_device_handle *dev_handle,
	int request_type, int bRequest, int wValue, int wIndex,
	unsigned char *data, int wLength, unsigned int timeout);


typedef int(CALLBACK* libusb_bulk_transfer_callback)(struct libusb_device_handle *dev_handle,
	unsigned char endpoint, unsigned char *data, int length,
	int *actual_length, unsigned int timeout);

WHSUSB_API int WINAPI HS_SetLibUsbCallback(libusb_control_transfer_callback control_transfer, 
	libusb_bulk_transfer_callback bulk_transfer);

WHSUSB_API int WINAPI HS_AddDevice(int index);

WHSUSB_API int WINAPI HS_RemoveDevice(int index);

WHSUSB_API int WINAPI HS_RemoveAll();

WHSUSB_API int WINAPI HS_OpenDevice(int index);

WHSUSB_API int WINAPI HS_CloseDevice(int index);
#endif

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif //!__WHSUSBINNER_API__
