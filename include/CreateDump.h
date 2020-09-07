#ifndef __CREATE_DUMP_H__
#define __CREATE_DUMP_H__

#include <string>

struct _EXCEPTION_POINTERS;
class CreateDump
{
public:
	CreateDump();
	~CreateDump();

	void SetErrorBox(const std::string& title, const std::string& msg){
		_errTitle = title;
		_errMsg = msg;
	}

	void DumpFile(const std::string& dumpFileName);

private:
	static long _stdcall UnhandleExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo);

private:
	static CreateDump* _pThis;

	std::string _dumpFileName;
	std::string _errTitle;
	std::string _errMsg;
};


extern CreateDump DUMP;


#endif //!__CREATE_DUMP_H__



