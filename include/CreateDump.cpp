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


LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(
	LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
	return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
	HMODULE hKernel32 = LoadLibraryA("kernel32.dll");
	if (hKernel32 == NULL) return FALSE;
	void *pOrgEntry = GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
	if (pOrgEntry == NULL) return FALSE;
	unsigned char newJump[100];
	DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
	dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
	void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
	DWORD dwNewEntryAddr = (DWORD)pNewFunc;
	DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

	newJump[0] = 0xE9;  // JMP absolute
	memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
	SIZE_T bytesWritten;
	BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
		pOrgEntry, newJump, sizeof(pNewFunc)+1, &bytesWritten);
	return bRet;
}

void CreateDump::DumpFile(const std::string& dumpFileName){
	_dumpFileName = dumpFileName;

	SetUnhandledExceptionFilter(UnhandleExceptionFilter);

#ifdef _M_IX86
	if (!PreventSetUnhandledExceptionFilter()){
		OutputDebugStringA("PreventSetUnhandledExceptionFilter false");
	}
#endif
}

long _stdcall CreateDump::UnhandleExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo){
	OutputDebugStringA("UnhandleExceptionFilter");

	SYSTEMTIME tm;
	GetLocalTime(&tm);
	char buff[MAX_PATH];
	sprintf_s(buff, MAX_PATH, "%04d-%02d-%02d-%02d-%02d-%02d",
		tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond);
	std::string strDumpFile = std::string(buff);
	if (!_pThis->_dumpFileName.empty()){
		strDumpFile += "-";
		strDumpFile += _pThis->_dumpFileName;
	}
	strDumpFile += ".dmp";

	HANDLE hFile = CreateFileA(strDumpFile.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,
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