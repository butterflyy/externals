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

	static void GetIntegral(const Score& score, Integral& inte){
		int64 countS = 0, countD = 0;
		for (int i = 0; i < 101; i++) {
			countS += score.scoreSame[i];
			countD += score.scoreDiff[i];
		}

		inte.frr_inte[0] = score.scoreSame[0] / (countS * 1.0);
		for (int i = 1; i < 101; i++) {
			inte.frr_inte[i] = inte.frr_inte[i - 1] + score.scoreSame[i] / (countS * 1.0);
		}

		inte.far_inte[100] = score.scoreDiff[100] / (countD * 1.0);
		for (int i = 99; i >= 0; i--) {
			inte.far_inte[i] = inte.far_inte[i + 1] + score.scoreDiff[i] / (countD * 1.0);
		}
	}

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
		std::string str;
		for (int i = 0; i < 101; i++){
			str += utils::StrFormat("%d %s %s\r\n", i, Int64ToStr(score.scoreSame[i]).c_str(), 
				Int64ToStr(score.scoreDiff[i]).c_str());
		}
		return str;
	}

	static std::string IntegralToStr(const Integral& inte){
		std::string str;
		for (int i = 0; i < 101; i++){
			str += utils::StrFormat("%d %f %f\r\n", i, inte.frr_inte[i], inte.far_inte[i]);
		}
		return str;
	}

	static std::string FarfrrToStr(const FarFrr& farfrr){
		std::string str = utils::StrFormat("score = %f, err = %.8f \r\n", farfrr.score, farfrr.err);
		double farL[5] = { 0.0001, 0.00001, 0.000001, 0.0000001, 0 };
		for (int iL = 0; iL < 5; iL++){
			str += utils::StrFormat("far = %.8f, frr = %.8f, score = %.8f \r\n",
				farL[iL], farfrr.frrL[iL], farfrr.frrS[iL]);
		}
		return str;
	}

}//!namespace


#endif //!__FAR_FRR_H__