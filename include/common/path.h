#ifndef ___COMMON_PATH__H__
#define ___COMMON_PATH__H__

#ifdef WIN32
#include <windows.h>
#endif



namespace utils{
#ifdef WIN32
	static const std::string seprator = "\\";
#else
	static const std::string seprator = "/";
#endif

	static std::string get_tmp(){
#ifdef WIN32
		char buff[MAX_PATH] = {0};
		::GetTempPathA(MAX_PATH, buff);
		int len = strlen(buff);
		if (len >= 1){//remove dir
			buff[len - 1] = 0;
		}
		return std::string(buff);
#else
		return "/tmp";
#endif
	}

	//Append to path, return path.
	static std::string& append(std::string& path, const std::string& name){
		path.append(seprator);
		path.append(name);
		return path;
	}
};

#endif //!___COMMON_PATH__H__