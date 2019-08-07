#pragma once

#include <common\utils.h>
#include <farfrr\farfrr.h>
#include <vector>

namespace farfrr{

	struct Data{
		std::vector<std::string> lines;
	};


	struct ResultData{
		int64 count;
		farfrr::Score score;

		int64 countValnm;
		farfrr::Score scoreValnm;

		std::vector<std::string> diffMatchs;
		std::vector<std::string> samenoMatchs;
	};
}