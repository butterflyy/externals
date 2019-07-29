#pragma once
#include <string>

struct limit_data;
class TimeLimit
{
public:
	TimeLimit(int totalTimes = 20);
	~TimeLimit();

	int TotalTimes() const;

	//0 ok, -1 create data failed
	int CreateData();

	//0 ok
	//1 first no file need create data
	//2 limit version is old, need update
	//-1 no intergrity
	//-2 other error
	int CheckIntergrity();

	//true valid; false no valid
	bool IsValid(int& leftTimes);

	//easy check  valid
	bool EasyCheckValid(int& leftTimes, std::string& err);
private:
	int _totalTimes;
	std::string _path1;
	std::string _path2;
	int _size1;
	int _size2;
	limit_data* _data;
};

inline int TimeLimit::TotalTimes() const{
	return _totalTimes;
}

