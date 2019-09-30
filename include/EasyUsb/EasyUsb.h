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
	EasyUsb();

	struct match_result {
		int score;
		int angle;
		int valnm;
	};

	~EasyUsb();

	int DeviceSize() const{
		return _deviceSize;
	}

	int Encode(const byte* image_ptr, int width, int height,
		byte* template_ptr, int length, struct hs_encode_param* param);

	//0 success, < 0 failed, 1 encode fail.
	int Encode(const std::string& path, byte* template_ptr, int length, struct hs_encode_param* param);

	
	int Match(int index, const std::string& tmpl1,
		const std::vector<std::string>& tmplsN,
		std::vector<match_result>& results);

	int Match(int index, MATCH_RECORD record, std::vector<match_result>& results);

	static std::string GetAllParamAsString(const hs_encode_param* param);

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