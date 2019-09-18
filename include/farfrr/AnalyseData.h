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

		ResultData(){
			clear();
		}

		void clear(){
			count = 0;
			memset(&score, 0, sizeof(farfrr::Score));
			countValnm = 0;
			memset(&scoreValnm, 0, sizeof(farfrr::Score));
			diffMatchs.clear();
			samenoMatchs.clear();
		}

		ResultData& operator +=(const ResultData& r){
			count += r.count;
			for (int i = 0; i < 101; i++){
				score.scoreSame[i] += r.score.scoreSame[i];
				score.scoreDiff[i] += r.score.scoreDiff[i];
			}

			countValnm += r.countValnm;
			for (int i = 0; i < 101; i++){
				scoreValnm.scoreSame[i] += r.scoreValnm.scoreSame[i];
				scoreValnm.scoreDiff[i] += r.scoreValnm.scoreDiff[i];
			}

			for (int i = 0; i < r.diffMatchs.size(); i++){
				diffMatchs.push_back(r.diffMatchs[i]);
			}
			for (int i = 0; i < r.samenoMatchs.size(); i++){
				samenoMatchs.push_back(r.samenoMatchs[i]);
			}

			return *this;
		}
	};
}