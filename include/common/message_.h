#ifndef ___MESSAGE__H__
#define ___MESSAGE__H__

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

namespace utils{
	//max output buff 1024byte
	inline void OutputDebug(const char* format, ...){
		assert(format);
		char buff[1024] = { 0 };
		va_list va;
		va_start(va, format);
		int len = vsnprintf(buff, 1024 - 1, format, va);
		va_end(va);

		if (len == -1) {
			assert(false);
		}
#ifdef WIN32
		OutputDebugStringA(buff);
#endif
		printf("%s", buff);
	}

	//max output buff 1024byte
	inline void OutputDebugLn(const char* format, ...){
		assert(format);
		char buff[1024] = { 0 };
		va_list va;
		va_start(va, format);
		int len = vsnprintf(buff, 1024 - 1, format, va);
		va_end(va);

		if (len == -1) {
			assert(false);
		}

		strcat(buff, "\r\n");
#ifdef WIN32
		OutputDebugStringA(buff);
#endif
		printf("%s", buff);
	}

	inline void OutputDebug(const std::string& str){
#ifdef WIN32
		OutputDebugStringA(str.c_str());
#endif
		printf("%s", str.c_str());
	}

	inline void OutputDebugLn(const std::string& str){
		std::string buff = str;
		buff.append("\r\n");
#ifdef WIN32
		OutputDebugStringA(buff.c_str());
#endif
		printf("%s", buff.c_str());
	}

	inline void ShowMessageG(const std::string& msg){
#ifdef WIN32
		::MessageBoxA(NULL, msg.c_str(), "message", MB_OK);
#else
		printf("%s", msg.c_str());
#endif
	}
}


#ifdef _MSC_VER
#pragma warning(pop)
#endif


#endif //!___MESSAGE__H__