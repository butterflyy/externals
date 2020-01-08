#ifndef __FAR_FRR_H__
#define __FAR_FRR_H__

#include <cstdlib>

#ifdef _WIN32
typedef __int64            int64;
typedef unsigned __int64   uint64;
#else
typedef ulong long         int64;
typedef unsigned long long uint64;
#endif

namespace farfrr{
	struct Score{
		int64 scoreSame[101];
		int64 scoreDiff[101];
	};

	struct Integral{
		double far_inte[101];
		double frr_inte[101];
	};

	struct FarFrr {
		double score;
		double err;
		double frrL[5];
		double frrS[5];
	};

	inline std::string Int64ToStr(int64 i){
		char buff[50] = { 0 };
		_i64toa_s(i, buff, 50, 10);
		return std::string(buff);
	}

	/*if read double value from file, may be loss precision*/
	static void GetIntegral(const Score& score, Integral& inte){
		Score inte_score;
		memset(&inte_score, 0, sizeof(Score));
		for (int i = 0; i < 101; i++) {
			if (i == 0){
				inte_score.scoreSame[i] = score.scoreSame[i];
			}
			else{
				inte_score.scoreSame[i] = inte_score.scoreSame[i - 1] + score.scoreSame[i];
			}
		}

		for (int i = 100; i >= 0; i--) {
			if (i == 100){
				inte_score.scoreDiff[i] = score.scoreDiff[i];
			}
			else{
				inte_score.scoreDiff[i] = inte_score.scoreDiff[i + 1] + score.scoreDiff[i];
			}
		}

		for (int i = 0; i < 101; i++){
			inte.frr_inte[i] = inte_score.scoreSame[i]*1.0 / inte_score.scoreSame[100];
			inte.far_inte[i] = inte_score.scoreDiff[i] * 1.0 / inte_score.scoreDiff[0];
		}
	}

	/*if read double value from file, may be loss precision*/
	static void GetFarFrr(const Integral& inte, FarFrr& farfrr){
		int x1 = 0, x2 = 0;
		for (int i = 0; i < 101; i++) {
			if (inte.frr_inte[i] >= inte.far_inte[i]) {
				x1 = i - 1;
				x2 = i;
				break;
			}
		}

		double y1 = inte.far_inte[x1];
		double y2 = inte.frr_inte[x1];
		double y3 = inte.frr_inte[x2];
		double y4 = inte.far_inte[x2];

		double x = x1 + (y2 - y1) / (y4 - y1 - y3 + y2);
		double y = (y1 * y3 - y2 * y4) / (y3 - y2 - y4 + y1);

		farfrr.score = x;
		farfrr.err = y;

		double farL[5] = { 0.0001, 0.00001, 0.000001, 0.0000001, 0 };
		for (int iL = 0; iL < 5; iL++) {
			//
			// get index form far
			//
			x1 = 0;
			x2 = 0;
			for (int i = 0; i < 101; i++) {
				if (farL[iL] >= inte.far_inte[i]) {
					x1 = i - 1;
					x2 = i;
					break;
				}
			}
			if (x1 == 0 && x2 == 0) {
				continue;
			}

			y1 = inte.far_inte[x1];
			// double y2 = frr[x1];
			// double y3 = frr[x2];
			y4 = inte.far_inte[x2];

			double xD = (farL[iL] - y1) / (y4 - y1) + x1;

			//
			// get frr from index
			//
			// x1 = (int) xD;
			// x2 = x1 + 1;

			// double y1 = far[x1];
			y2 = inte.frr_inte[x1];
			y3 = inte.frr_inte[x2];
			// double y4 = far[x2];

			farfrr.frrL[iL] = (y3 - y2) * (xD - x1) + y2;
			farfrr.frrS[iL] = xD;
		}
	}

	inline void GetFarFrr(const Score& score, FarFrr& farfrr){
		Integral inte;
		GetIntegral(score, inte);
		GetFarFrr(inte, farfrr);
	}

	static std::string ScoreToStr(const Score& score){
		std::string str = "i same diff\r\n";
		for (int i = 0; i < 101; i++){
			str += utils::StrFormat("%d %s %s\r\n", i, Int64ToStr(score.scoreSame[i]).c_str(), 
				Int64ToStr(score.scoreDiff[i]).c_str());
		}
		return str;
	}

	/*if read double value from file, may be loss precision*/
	static std::string IntegralToStr(const Integral& inte){
		std::string str = "i frr far\r\n";
		for (int i = 0; i < 101; i++){
			str += utils::StrFormat("%d %.10f %.10f\r\n", i, inte.frr_inte[i], inte.far_inte[i]);
		}
		return str;
	}

	/*if read double value from file, may be loss precision*/
	static std::string IntegralToCsv(const Integral& inte){
		std::string str = "i,far,frr\r\n";
		for (int i = 0; i < 101; i++){
			str += utils::StrFormat("%d,%.10f,%.10f\r\n", i, inte.far_inte[i], inte.frr_inte[i]);
		}
		return str;
	}

	/*if read double value from file, may be loss precision*/
	static std::string FarfrrToStr(const FarFrr& farfrr){
		std::string str = utils::StrFormat("score = %.10f, err = %.10f \r\n", farfrr.score, farfrr.err);
		double farL[5] = { 0.0001, 0.00001, 0.000001, 0.0000001, 0 };
		for (int iL = 0; iL < 5; iL++){
			str += utils::StrFormat("far = %.10f, frr = %.10f, score = %.10f \r\n",
				farL[iL], farfrr.frrL[iL], farfrr.frrS[iL]);
		}
		return str;
	}

}//!namespace


#endif //!__FAR_FRR_H__