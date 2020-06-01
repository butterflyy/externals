#include "EasyMatchBox.h"

EasyMatchBox* EasyMatchBox::_pThis = nullptr;


EasyMatchBox::EasyMatchBox()
{
	_pThis = this;

	int ret = WHSCOM_Initialize();
	if (ret < 0){
		assert(false);
		return;
	}

	ret = WHSCOM_SetCallback(ConnectedCallback, DisconnectedCallback, ErrorCallback);
	if (ret < 0){
		assert(false);
		return;
	}

	ret = WHSCOM_StartServer();
	if (ret < 0){
		assert(false);
		return;
	}

	//waiting device connect.
	utils::Thread::msleep(8000);
}

EasyMatchBox::~EasyMatchBox()
{
	WHSCOM_StopServer();
	WHSCOM_Finalize();
}

int EasyMatchBox::TmplSize(){
	return 8192;
}


int EasyMatchBox::Download(int index, whscom_ddr ddr, const std::vector<template_info>& infos)
{
	whscom_template_record record = WHSCOM_CreateTemplateRecord();
	for each (auto var in infos)
	{
		int ret = WHSCOM_AddTemplate(record, var.index, var.tmpl, var.size);
		if (ret < 0){
			_err = "add template failed";
			return -1;
		}
	}

	int ret = WHSCOM_DownloadTemplate(deviceHandle(index), ddr, record);
	if (ret < 0){
		_err = "download tmpl failed";
		return -1;
	}
	WHSCOM_ReleaseTemplateRecord(record);

	return 0;
}

int EasyMatchBox::Match(int index, whscom_ddr ddr, const std::string& tmpl1, const std::vector<std::string>& tmplsN, std::vector<match_result>& results)
{
	whscom_template_record record = WHSCOM_CreateTemplateRecord();
	//add one
	utils::Buffer tmpl;
	int size = utils::ReadFile(tmpl1, tmpl);
	if (size <= 0){
		_err = "read tmpl1 failed";
		return -1;
	}

	int ret = WHSCOM_AddTemplate(record, 0, tmpl.data(), tmpl.size());
	if (ret < 0){
		_err = "add tmpl1 failed";
		return -1;
	}

	//add N
	int tmpl_index = 1;
	for each (auto var in tmplsN)
	{
		size = utils::ReadFile(var, tmpl);
		if (size <= 0){
			_err = "read tmplN failed";
			return -1;
		}

		int ret = WHSCOM_AddTemplate(record, index++, tmpl.data(), tmpl.size());
		if (ret < 0){
			_err = "add tmplN failed";
			return -1;
		}
	}

	ret = WHSCOM_DownloadTemplate(deviceHandle(index), ddr, record);
	if (ret < 0){
		_err = "download tmpl failed";
		return -1;
	}
	WHSCOM_ReleaseTemplateRecord(record);

	return Match(index, ddr, 1, tmplsN.size(), results);
}

int EasyMatchBox::Match(int index, whscom_ddr ddr, int begin, int end, std::vector<match_result>& results)
{
	int result_size = end - begin + 1;
	int* results_score = new int[result_size];
	int* results_angle = new int[result_size];
	int* results_valnm = new int[result_size];
	memset(results_score, 0, sizeof(int)*result_size);
	memset(results_angle, 0, sizeof(int)*result_size);
	memset(results_valnm, 0, sizeof(int)*result_size);

	int ret = WHSCOM_Match2(deviceHandle(index), ddr, begin, end, results_score, results_angle, results_valnm, result_size);
	if (ret == 0){
		for (int i = 0; i < result_size; i++){
			match_result result;
			result.score = results_score[i];
			result.angle = results_angle[i];
			result.valnm = results_valnm[i];

			results.push_back(result);
		}
	}

	SAFE_DELETE_ARRAY(results_score);
	SAFE_DELETE_ARRAY(results_angle);
	SAFE_DELETE_ARRAY(results_valnm);

	if (ret < 0){
		_err = "match2 failed";
		return -1;
	}

	return 0;
}

