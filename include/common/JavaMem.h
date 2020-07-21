#ifndef ___JAVA_MEM__H__
#define ___JAVA_MEM__H__

#include <common/utils.h>

#include <common/message_.h>

namespace JavaMem{
	typedef void* MemPtr;

	typedef unsigned short     uint16;
	typedef unsigned int       uint32;

	static MemPtr New(int len){
		return new byte[len];
	}

	static void Delete(MemPtr mem){
		byte* ptr = (byte*)mem;
		
		utils::safeArrayDelete(ptr);
	}


	static bool Set(MemPtr mem, int offset, int data, int len){
		if (offset < 0 || len < 0) return false;
		byte* ptr = (byte*)mem;
		if (!ptr) return false;

		switch (len)
		{
		case 1:
			*((byte*)(ptr + offset)) = data;
			return true;
		case 2:
			*((uint16*)(ptr + offset)) = data;
			return true;
		case 4:
			*((uint32*)(ptr + offset)) = data;
			return true;
		default:
			return false;
		}
	}

	static bool Set(MemPtr mem, int offset, const byte* pdata, int len){
		if (offset < 0 || len < 0) return false;
		byte* ptr = (byte*)mem;
		if (!ptr) return false;

		if (!pdata) return false;

		memcpy(ptr + offset, pdata, len);
		return true;
	}

	static bool Get(MemPtr mem, int offset, int* pdata, int len){
		if (offset < 0 || len < 0) return false;
		byte* ptr = (byte*)mem;
		if (!ptr) return false;

		if (!pdata) return false;
		switch (len)
		{
		case 1:
			*pdata = *((byte*)(ptr + offset));
			return true;
		case 2:
			*pdata = *((uint16*)(ptr + offset));
			return true;
		case 4:
			*pdata = *((uint32*)(ptr + offset));
			return true;
		default:
			return false;
		}
	}

	static bool Get(MemPtr mem, int offset, byte* pdata, int len){
		if (offset < 0 || len < 0) return false;
		byte* ptr = (byte*)mem;
		if (!ptr) return false;

		if (!pdata) return false;

		memcpy(pdata, ptr + offset, len);
		return true;
	}
};


#endif //!___JAVA_MEM__H__