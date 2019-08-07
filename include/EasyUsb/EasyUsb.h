#ifndef __EASY_USB_H__
#define __EASY_USB_H__

#include <bmp\readwrite.h>
#include <whsusbinnerapi.h>
#pragma comment(lib, "whsusbapi.lib")


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif


class EasyUsb{
public:
	EasyUsb():
		_deviceSize(0){
		int ret = HS_Initialize();
		if (ret < 0){
			assert(false);
			return;
		}
		_deviceSize = HS_GetDeviceList();
	}

	struct match_result {
		int score;
		int angle;
		int valnm;
	};

	~EasyUsb(){
		HS_Finalize();
	}

	int DeviceSize() const{
		return _deviceSize;
	}

	int Encode(const byte* image_ptr, int width, int height,
		byte* template_ptr, int length, struct hs_encode_param* param){
		return HS_Encode2(0, image_ptr, width, height, template_ptr, length, param);
	}

	//0 success, < 0 failed, 1 encode fail.
	int Encode(const std::string& path, byte* template_ptr, int length, struct hs_encode_param* param){
		int width = 768;
		int height = 768;
		utils::Buffer imagebuf(width* height);
		int size = bmp::ReadData(path, imagebuf.data(), imagebuf.size(),  &width, &height);
		if (size < 0){
			_err = "Read image data failed";
			return -1;
		}
		
		int ret = HS_Encode2(0, imagebuf.data(), width, height, template_ptr, length, param);
		if (ret < 0){
			_err = HS_StrError(ret);
			return -1;
		}
		else if (ret == 2){
			_err = "encode timeout";
			return -1; 
		}

		return ret;
	}

	
	int Match(int index, const std::string& tmpl1,
		const std::vector<std::string>& tmplsN,
		std::vector<match_result>& results){
		const int TMP_SIZE = 8192;
		utils::Buffer tmpl(TMP_SIZE);
		int size = utils::ReadFile(tmpl1, tmpl);
		if (size != TMP_SIZE){
			_err = "read tmpl1 failed";
			return -1;
		}

		MATCH_RECORD record = HS_CreateMatchRecord();
		int ret = HS_SetTemplateA(record, tmpl.data(), TMP_SIZE);
		if (ret < 0){
			return -1;
		}
		for (int i = 0; i < tmplsN.size(); i++){
			size = utils::ReadFile(tmplsN[i], tmpl);
			if (size != TMP_SIZE){
				_err = "read tmplN failed";
				return -1;
			}

			int ret = HS_InsertTemplateB(record, tmpl.data(), TMP_SIZE, "");
			if (ret < 0){
				return -1;
			}
		}

		MATCH_RESULT result = HS_CreateMatchResult();

		ret = HS_Match(index, record, result);
		if (ret < 0) {
			_err = HS_StrError(ret);
			return -1;
		}

		HS_ReleaseMatchRecord(record);

		//get match result
		int rsize = HS_MatchResultSize(result);
		if (rsize != tmplsN.size()){
			return -1;
		}

		for (int i = 0; i < rsize; i++) {
			match_result res;
			res.score = HS_MatchResultScore(result, i);
			res.angle = HS_MatchResultAngle(result, i);
			res.valnm = HS_MatchResultValnm(result, i);

			results.push_back(res);
		}

		HS_ReleaseMatchResult(result);

		return 0;
	}

	int Match(int index, MATCH_RECORD record, std::vector<match_result>& results){
		MATCH_RESULT result = HS_CreateMatchResult();
		int ret = HS_Match(index, record, result);
		if (ret < 0) {
			_err = HS_StrError(ret);
			return -1;
		}

		int rsize = HS_MatchResultSize(result);
		for (int i = 0; i < rsize; i++) {
			match_result res;
			res.score = HS_MatchResultScore(result, i);
			res.angle = HS_MatchResultAngle(result, i);
			res.valnm = HS_MatchResultValnm(result, i);

			results.push_back(res);
		}

		HS_ReleaseMatchResult(result);
		return 0;
	}

	static std::string GetAllParamAsString(const hs_encode_param* param){
		//save param
		char param_buff[2048] = { 0 };
		sprintf(param_buff, "\
down_sample_x       =   %d\r\n\
down_sample_y       =   %d\r\n\
full_sample_x       =   %d\r\n\
full_sample_y       =   %d\r\n\
pupil_circle_x      =   %f\r\n\
pupil_circle_y      =   %f\r\n\
pupil_circle_r      =   %f\r\n\
pupil_circle_o      =   %f\r\n\
iris_circle_x       =   %f\r\n\
iris_circle_y       =   %f\r\n\
iris_circle_r       =   %f\r\n\
iris_circle_o       =   %f\r\n\
eyelid_upper_a      =   %d\r\n\
eyelid_upper_b      =   %d\r\n\
eyelid_upper_c      =   %d\r\n\
eyelid_upper_e      =   %d\r\n\
eyelid_lower_a      =   %d\r\n\
eyelid_lower_b      =   %d\r\n\
eyelid_lower_c      =   %d\r\n\
eyelid_lower_e      =   %d\r\n\
upper_angle         =   %d\r\n\
lower_angle         =   %d\r\n\
left_angle          =   %d\r\n\
right_angle         =   %d\r\n\
gray                =   %d\r\n\
distance_ip         =   %f\r\n\
distance            =   %d\r\n\
dialoation          =   %f\r\n\
margin_left         =   %f\r\n\
margin_right        =   %f\r\n\
margin_top          =   %f\r\n\
margin_bottom       =   %f\r\n\
focus               =   %f\r\n\
validpix            =   %d\r\n\
quality             =   %d\r\n\
error_flag          =   %d\r\n\
warn1_flag          =   %d\r\n\
warn2_flag          =   %d\r\n\
warn3_flag          =   %d\r\n\
",
param->down_sample_x,
param->down_sample_y,
param->full_sample_x,
param->full_sample_y,
param->pupil_circle_x,
param->pupil_circle_y,
param->pupil_circle_r,
param->pupil_circle_o,
param->iris_circle_x,
param->iris_circle_y,
param->iris_circle_r,
param->iris_circle_o,
param->eyelid_upper_a,
param->eyelid_upper_b,
param->eyelid_upper_c,
param->eyelid_upper_e,
param->eyelid_lower_a,
param->eyelid_lower_b,
param->eyelid_lower_c,
param->eyelid_lower_e,
param->upper_angle,
param->lower_angle,
param->left_angle,
param->right_angle,
param->gray,
param->distance_ip,
param->distance,
param->dialoation,
param->margin_left,
param->margin_right,
param->margin_top,
param->margin_bottom,
param->focus,
param->valid,
param->quality,
param->flag.error_flag,
param->flag.warn1_flag,
param->flag.warn2_flag,
param->flag.warn3_flag
);
		return std::string(param_buff);
	}

	std::string lastError() const{
		return _err;
	}
private:
	std::string _err;

	int _deviceSize;
};


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //!__EASY_USB_H__