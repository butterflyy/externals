#include "TimeLimit.h"
#include <common\utils.h>
#include <common\rc4.h>
#include <shlobj.h>
#include <stdio.h>
#pragma comment(lib, "shell32.lib")

#pragma warning(disable:4996)

#define LIMIT_VERSION              0x01

#define KEY                        "F014672E-F0E6-3C55-9E7F-279F5791B128"
#define FILENAME1                  "system.m1"
#define FILENAME2                  "windows.k2"

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;

typedef unsigned char      byte;

#ifdef _WIN32
typedef __int64            int64;
typedef unsigned __int64   uint64;
#else
typedef ulong long         int64;
typedef unsigned long long uint64;
#endif

#pragma pack (push, 1)
struct limit_time{
	uint32 year;
	uint32 mon;
	uint32 day;
	uint32 hour;
	uint32 min;
	uint32 sec;
};

struct limit_data{
	byte noused1[20];
	uint16 version;
	int64 t;
	limit_time tm;
	byte noused2[20];
	uint32 limit;
	byte noused3[20];
	uint16 crc;
};
#pragma pack (pop)


uint16 crc16_ccitt(const byte* a_szBufuer, int a_sBufferLen)
{
	unsigned short usCRC = 0;
	for (short j = 0; j < a_sBufferLen; j++)
	{
		unsigned char* pucPtr = (unsigned char*)&usCRC;
		*(pucPtr + 1) = *(pucPtr + 1) ^ *a_szBufuer++;
		for (short i = 0; i <= 7; i++)
		{
			if (usCRC & ((short)0x8000))
			{
				usCRC = usCRC << 1;
				usCRC = usCRC ^ ((unsigned short)0x8005);
			}
			else
				usCRC = usCRC << 1;
		}
	}
	return (usCRC);
}

void encrpyt_rc4(unsigned char* pInStream, int nInLen, char* pKey, int nKeyLen, unsigned char* pOutStream){
	struct rc4_state s;
	memset(&s, 0, sizeof(struct rc4_state));
	rc4_setup(&s, (unsigned char*)pKey, nKeyLen);
	rc4_crypt(&s, pInStream, nInLen, pOutStream);
}

TimeLimit::TimeLimit(int totalTimes)
:_totalTimes(totalTimes),
_size1(37),
_size2(sizeof(limit_data) - 37),
_data(new limit_data)
{
}


TimeLimit::~TimeLimit()
{
	SAFE_DELETE(_data);
}


int TimeLimit::CreateData(){
	for (int i = 0; i < 20; i++){
		_data->noused1[i] = i * 12;
	}

	_data->version = LIMIT_VERSION;

	time_t t;
	time(&t);
	struct tm *lt = localtime(&t);
	_data->t = t;
	_data->tm.year = lt->tm_yday;
	_data->tm.mon = lt->tm_mon;
	_data->tm.day = lt->tm_mday;

	for (int i = 0; i < 20; i++){
		_data->noused1[i] = i * 29;
	}

	_data->limit = _totalTimes;

	for (int i = 0; i < 20; i++){
		_data->noused1[i] = i * 77;
	}

	//set crc
	_data->crc = crc16_ccitt((byte*)_data, sizeof(limit_data)-sizeof(uint16));

	//encrypt file
	utils::Buffer en_data(sizeof(limit_data));
	encrpyt_rc4((byte*)_data, en_data.size(), KEY, strlen(KEY), en_data.data());

	//save file
	if (_size1 != utils::WriteFile(_path1, en_data.data(), _size1)){
		return -1;
	}

	if (_size2 != utils::WriteFile(_path2, en_data.data() + _size1, _size2)){
		return -1;
	}

	return 0;
}

int TimeLimit::CheckIntergrity(){
	char szWindows[MAX_PATH] = { 0 };
	char szAppData[MAX_PATH] = { 0 };
	if (!SHGetSpecialFolderPathA(NULL, szWindows, CSIDL_WINDOWS, FALSE)){
		return -2;
	}
	_path1 = szWindows;

	if (!SHGetSpecialFolderPathA(NULL, szAppData, CSIDL_COMMON_APPDATA, FALSE)){
		return -2;
	}
	_path2 = szAppData;
	

#if 0
	_path1 = "d:\\Temp";
#endif

	_path1 += "\\";
	_path1 += FILENAME1;
	_path2 += "\\";
	_path2 += FILENAME2;

	if (!utils::CheckFileExist(_path1)
		&& !utils::CheckFileExist(_path2)){//no file
		return 1;
	}

	if (_size1 != utils::GetFileSize(_path1)
		|| _size2 != utils::GetFileSize(_path2)){
		return -1;
	}

	//read data
	utils::Buffer data(sizeof(limit_data));
	assert(_size1 + _size2 == sizeof(limit_data));

	if (_size1 != utils::ReadFile(_path1, data.data(), _size1)){
		return -1;
	}
	if (_size2 != utils::ReadFile(_path2, data.data() + _size1, _size2)){
		return -1;
	}

	//decrypt data
	encrpyt_rc4(data.data(), data.size(), KEY, strlen(KEY), (byte*)_data);

	//check data crc
	uint16 crc = crc16_ccitt((byte*)_data, sizeof(limit_data) - sizeof(uint16));
	if (crc != _data->crc){
		return -1;
	}

	//check limit version
	if (_data->version < LIMIT_VERSION){
		return 2;
	}
	
	return 0;
}

bool TimeLimit::IsValid(int& leftTimes){
	if (_data->limit <= 0){
		return false;
	}

	//check time is change old
	time_t t;
	time(&t);
	struct tm *lt = localtime(&t);
	if (_data->t > t){
		//check time
		return false;
	}
	_data->t = t;

	//check if is different date
	if (_data->tm.year != lt->tm_yday ||
		_data->tm.mon != lt->tm_mon ||
		_data->tm.day != lt->tm_mday){
		_data->tm.year = lt->tm_yday;
		_data->tm.mon = lt->tm_mon;
		_data->tm.day = lt->tm_mday;
		_data->limit--;
	}

	leftTimes = _data->limit;

	//update crc
	_data->crc = crc16_ccitt((byte*)_data, sizeof(limit_data)-sizeof(uint16));

	//encrypt file
	utils::Buffer en_data(sizeof(limit_data));
	encrpyt_rc4((byte*)_data, en_data.size(), KEY, strlen(KEY), en_data.data());

	//save file
	if (_size1 != utils::WriteFile(_path1, en_data.data(), _size1)){
		return false;
	}

	if (_size2 != utils::WriteFile(_path2, en_data.data() + _size1, _size2)){
		return false;
	}

	return true;
}

bool TimeLimit::EasyCheckValid(int& leftTimes, std::string& err){
	int ret = CheckIntergrity();
	if (ret == 1){
		ret = CreateData();
		if (ret < 0){
			err = "create new data error";
			return false;
		}
		leftTimes = _totalTimes;
	}
	else if (ret == 0){//ok
		if (!IsValid(leftTimes)){
			err = "probation is up";
			return false;
		}
	}
	else if (ret == 2){
		ret = CreateData();
		if (ret < 0){
			err = "create update data error";
			return false;
		}
		leftTimes = _totalTimes;
	}
	else if (ret == -1){
		err = "file intergrity";
		return false;
	}
	else if (ret == -2){
		err = "other error";
		return false;
	}
	else{
		assert(false);
		return false;
	}

	return true;
}
