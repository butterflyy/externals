#pragma once

#include <common/utils.h>
#include <whsusbinnerapi.h>

namespace ParseParam{
	static std::string GetEncodeParamAsString(const hs_encode_param* param){
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

	static bool ParseStringEncodeParam(const std::string& str, hs_encode_param& param){
		std::vector<std::string> lines;
		int size = utils::ReadLines(str, lines);
		if (size <= 0) return false;

		std::vector<std::pair<std::string, std::string>> items;
		for each (auto var in lines)
		{
			std::vector<std::string> dests;
			utils::Spit(var, "=", dests);
			if (dests.size() != 2){
				assert(false); 
				return false;
			}
			items.push_back(std::pair<std::string, std::string>(utils::Trim(dests[0]), utils::Trim(dests[1])));
		}

		for each (auto var in items)
		{
			if (var.first == "down_sample_x"){
				param.down_sample_x = atoi(var.second.c_str());
			}
			else if (var.first == "down_sample_y"){
				param.down_sample_y = atoi(var.second.c_str());
			}
			else if (var.first == "full_sample_x"){
				param.full_sample_x = atoi(var.second.c_str());
			}
			else if (var.first == "full_sample_y"){
				param.full_sample_y = atoi(var.second.c_str());
			}
			else if (var.first == "pupil_circle_x"){
				param.pupil_circle_x = atof(var.second.c_str());
			}
			else if (var.first == "pupil_circle_y"){
				param.pupil_circle_y = atof(var.second.c_str());
			}
			else if (var.first == "pupil_circle_r"){
				param.pupil_circle_r = atof(var.second.c_str());
			}
			else if (var.first == "pupil_circle_o"){
				param.pupil_circle_o = atof(var.second.c_str());
			}
			else if (var.first == "iris_circle_x"){
				param.iris_circle_x = atof(var.second.c_str());
			}
			else if (var.first == "iris_circle_y"){
				param.iris_circle_y = atof(var.second.c_str());
			}
			else if (var.first == "iris_circle_r"){
				param.iris_circle_r = atof(var.second.c_str());
			}
			else if (var.first == "iris_circle_o"){
				param.iris_circle_o = atof(var.second.c_str());
			}
			else if (var.first == "eyelid_upper_a"){
				param.eyelid_upper_a = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_upper_b"){
				param.eyelid_upper_b = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_upper_c"){
				param.eyelid_upper_c = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_upper_e"){
				param.eyelid_upper_e = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_lower_a"){
				param.eyelid_lower_a = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_lower_b"){
				param.eyelid_lower_b = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_lower_c"){
				param.eyelid_lower_c = atoi(var.second.c_str());
			}
			else if (var.first == "eyelid_lower_e"){
				param.eyelid_lower_e = atoi(var.second.c_str());
			}
			else if (var.first == "upper_angle"){
				param.upper_angle = atoi(var.second.c_str());
			}
			else if (var.first == "lower_angle"){
				param.lower_angle = atoi(var.second.c_str());
			}
			else if (var.first == "left_angle"){
				param.left_angle = atoi(var.second.c_str());
			}
			else if (var.first == "right_angle"){
				param.right_angle = atoi(var.second.c_str());
			}
			else if (var.first == "gray"){
				param.gray = atoi(var.second.c_str());
			}
			else if (var.first == "distance_ip"){
				param.distance_ip = atof(var.second.c_str());
			}
			else if (var.first == "distance"){
				param.distance = atoi(var.second.c_str());
			}
			else if (var.first == "dialoation"){
				param.dialoation = atof(var.second.c_str());
			}
			else if (var.first == "margin_left"){
				param.margin_left = atof(var.second.c_str());
			}
			else if (var.first == "margin_right"){
				param.margin_right = atof(var.second.c_str());
			}
			else if (var.first == "margin_top"){
				param.margin_top = atof(var.second.c_str());
			}
			else if (var.first == "margin_bottom"){
				param.margin_bottom = atof(var.second.c_str());
			}
			else if (var.first == "focus"){
				param.focus = atof(var.second.c_str());
			}
			else if (var.first == "validpix"){
				param.valid = atoi(var.second.c_str());
			}
			else if (var.first == "quality"){
				param.quality = atoi(var.second.c_str());
			}
			else if (var.first == "error_flag"){
				param.flag.error_flag = atoi(var.second.c_str());
			}
			else if (var.first == "warn1_flag"){
				param.flag.warn1_flag = atoi(var.second.c_str());
			}
			else if (var.first == "warn2_flag"){
				param.flag.warn2_flag = atoi(var.second.c_str());
			}
			else if (var.first == "warn3_flag"){
				param.flag.warn3_flag = atoi(var.second.c_str());
			}
			else{
				assert(false);
				return false;
			}
		}

		return true;
	}

	inline bool WriteEncodeParamData(const std::string& path, const hs_encode_param& param){
		return utils::WriteFile(path, (byte*)&param, sizeof(hs_encode_param)) > 0;
	}

	inline bool ReadEncodeParamData(const std::string& path, hs_encode_param& param){
		utils::Buffer buffer(sizeof(hs_encode_param));
		if (utils::ReadFile(path, buffer) < 0){
			return false;
		}

		memcpy(&param, buffer.data(), buffer.size());
		return true;
	}
}