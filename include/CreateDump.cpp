#include "CreateDump.h"
#include <Windows.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

CreateDump DUMP;

CreateDump* CreateDump::_pThis = NULL;

CreateDump::CreateDump(){
	_pThis = this;
	_errTitle = "Error";
	_errMsg = "Program has an exception and needs to be closed!";
}

CreateDump::~CreateDump(){

}

void CreateDump::DumpFile(const std::string& dumpFileName){
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	char buff[MAX_PATH];
	sprintf_s(buff, MAX_PATH, "%04d-%02d-%02d-%02d-%02d-%02d",
		tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	_strDumpFile = std::string(buff);
	if (!dumpFileName.empty()){
		_strDumpFile += "-";
		_strDumpFile += dumpFileName;
	}
	_strDumpFile += ".dmp";

	OutputDebugStringA(_strDumpFile.c_str());

	SetUnhandledExceptionFilter(UnhandleExceptionFilter);
}

long _stdcall CreateDump::UnhandleExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo){
	HANDLE hFile = CreateFileA(_pThis->_strDumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE){
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = ExceptionInfo;
		ExInfo.ClientPointers = FALSE;

		BOOL bOk = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		CloseHandle(hFile);

		if (!bOk){
			DWORD dw = GetLastError();
			OutputDebugStringA("MiniDumpWriteDump Failed!");
			//write dump file errror
			return EXCEPTION_CONTINUE_SEARCH;
		}
		else{
			::MessageBoxA(NULL, _pThis->_errMsg.c_str(), _pThis->_errTitle.c_str(), MB_ICONERROR);
			return EXCEPTION_EXECUTE_HANDLER;
		}
	}
	else{
		OutputDebugStringA("Create Dump File Failed!");
		return EXCEPTION_EXECUTE_HANDLER;
	}
}