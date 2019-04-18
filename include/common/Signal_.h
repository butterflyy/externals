#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include <cassert>

#ifdef WIN32
#include <Windows.h>
#else
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#endif


class Signal{
public:
	Signal()
	:_bWaiting(false){
#ifdef WIN32
		_handle = CreateEvent(NULL, TRUE, FALSE, NULL);
		assert(_handle);
#else
		int ret = pthread_cond_init(&_cond, NULL);
		assert(ret == 0);
		ret = pthread_mutex_init(&_mutex, NULL);
		assert(ret == 0);
#endif	
	}

	~Signal(){
#ifdef WIN32
		CloseHandle(_handle);
		_handle = NULL;
#else
		int ret = pthread_cond_destroy(&_cond);
		assert(ret == 0);
		ret = pthread_mutex_destroy(&_mutex);
		assert(ret == 0);
#endif
	}

	//0 success  -1 error
	int SetSignal(){
#ifdef WIN32
		if(!SetEvent(_handle))
			return -1;
#else
		int ret = pthread_mutex_lock(&_mutex);
		if(ret != 0)
			return -1;
		int con_ret = pthread_cond_signal(&_cond);
		ret = pthread_mutex_unlock(&_mutex);
		if(ret != 0)
			return -1;	

		if (con_ret != 0)
			return -1;
#endif
		return 0;
	}

	//0 success  -1 error
	int ResetSignal(){
#ifdef WIN32
		if(!ResetEvent(_handle))
			return -1;
#else
		;
#endif
		return 0;
	}

	bool IsWait(){
		return _bWaiting;
	}

	//0 success 1 timeout -1 error
	//on linux platform, waiting sec.
	int WaitSignal(unsigned long msec = 0xFFFFFFFF /*INFINITE*/){
		_bWaiting = true;
#ifdef WIN32
		DWORD dwRet = WaitForSingleObject(_handle, msec);
		_bWaiting = false;

		if (WAIT_OBJECT_0 == dwRet){
			return 0;
		}
		else if (WAIT_FAILED == dwRet){
			assert(false);
			return -1;
		}
		else if (WAIT_TIMEOUT == dwRet){
			return 1;
		}
		else{
			assert(false);
			return -1;
		}
#else
		int ret = pthread_mutex_lock(&_mutex);
		if(ret != 0)
			return -1;

		if(msec == 0xFFFFFFFF){
			int con_ret = pthread_cond_wait(&_cond, 
				&_mutex);
			_bWaiting = false;

			ret = pthread_mutex_unlock(&_mutex);
			if(ret != 0)
				return -1;

			if(con_ret != 0)
				return -1;
		}
		else{
			struct timespec ts;
			struct timeval tp;
			ret = gettimeofday(&tp, NULL);
			if(ret != 0)
				return -1;
			//Convert form timeval to timespec
			ts.tv_sec = tp.tv_sec;
			ts.tv_nsec = tp.tv_usec * 1000;
			ts.tv_sec += msec / 1000;

			int con_ret = pthread_cond_timedwait(&_cond, 
				&_mutex, &ts);
			_bWaiting = false;

			ret = pthread_mutex_unlock(&_mutex);
			if(ret != 0)
				return -1;

			if(con_ret == ETIMEDOUT){
				return 1;
			}
			else if(con_ret != 0)
				return -1;
		}

		ret = pthread_mutex_unlock(&_mutex);
		if(ret != 0)
			return -1;
#endif
		return 0;
	}

private:

#ifdef WIN32
	HANDLE _handle;
#else
	pthread_cond_t _cond;
	pthread_mutex_t _mutex;
#endif
	volatile bool _bWaiting;
};

#endif //!__SIGNAL_H__
