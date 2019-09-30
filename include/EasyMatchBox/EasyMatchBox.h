#ifndef __EASY_MATCHBOX_H__
#define __EASY_MATCHBOX_H__

#include <common/utils.h>
#include <bmp\readwrite.h>
#include <whscominnerlib.h>
#pragma comment(lib, "whscomlib.lib")


#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif


class EasyMatchBox{
public:
	const int TMP_SIZE = 8192;

	struct template_info{
		int index;
		byte* tmpl;
	};

	struct match_result {
		int score;
		int angle;
		int valnm;
	};


	EasyMatchBox();

	~EasyMatchBox();

	int DeviceSize() const{
		return _devices.size();
	}

	int Download(int index, whscom_ddr ddr, const std::vector<template_info>& infos);
	
	int Match(int index, whscom_ddr ddr, const std::string& tmpl1,
		const std::vector<std::string>& tmplsN,
		std::vector<match_result>& results);

	int Match(int index, whscom_ddr ddr, int begin, int end, std::vector<match_result>& results);

	std::string lastError() const{
		return _err;
	}
private:
	static void CALLBACK ConnectedCallback(whscom_device_handle device, const char* ip){
		_pThis->_devices.push_back(std::pair<whscom_device_handle, std::string>(device, ip));
	}

	static void CALLBACK DisconnectedCallback(whscom_device_handle device){
	}

	static void CALLBACK ErrorCallback(whscom_device_handle device, int error_code){
	}

	whscom_device_handle deviceHandle(int index){
		return _devices[index].first;
	}
private:
	std::string _err;

	static EasyMatchBox* _pThis;

	std::vector<std::pair<whscom_device_handle, std::string>> _devices;
};


#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif //!__EASY_MATCHBOX_H__