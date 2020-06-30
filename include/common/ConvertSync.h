/*!
 * \class ConvertSync
 *
 * \brief conver async event to sync event 
 *
 * \author Butterfly
 * \date  2019-9-29
 */
#ifndef __CONVER_SYNC_H__
#define  __CONVER_SYNC_H__

#include <map>
#include <common/utils.h>
#include <common/Signal_.h>

template<typename SID, typename Data>
class ConvertSync{
public:
	struct Param
	{
		Signal* pSignal;
		Data data;
	};

	typedef std::map<SID, Param> SignalMap;

	ConvertSync(){
	}

	~ConvertSync(){
		for (typename SignalMap::const_iterator it = _eventMap.begin();
			it != _eventMap.end();
			++it){
			delete it->second.pSignal;
		}
	}

	//data out return waiting data
	//return 0 success, 1 timeout, -1 error, -2 already has id
	int WaitEvent(SID id, Data* data, int timeout = -1){
		_mutex.lock();
		if (find(id)){
			assert(false);
			_mutex.unlock();
			return -2;
		}

		//create event
		Signal* pSignal= new Signal();

		Param param;
		param.pSignal = pSignal;
		_eventMap[id] = param;

		_mutex.unlock();

		int ret = pSignal->WaitSignal(timeout == -1 ? 0xFFFFFFFF : timeout);

		_mutex.lock();
		if (ret == 0){//success
			*data = _eventMap[id].data;
		}

		//clear event
		SAFE_DELETE(pSignal);
		_eventMap.erase(id);
		_mutex.unlock();

		return ret;
	}

	//data set return waiting data
	//return 0 success, -1 error,  -2 do not has id
	int SetEvent(SID id, Data data){
		_mutex.lock();
		if (!find(id)){
			_mutex.unlock();
			return -2;
		}

		_eventMap[id].data = data;
		int ret = _eventMap[id].pSignal->SetSignal();
		_mutex.unlock();
		
		return ret;
	}

private:
	bool find(SID id){
		typename SignalMap::const_iterator it = _eventMap.find(id);
		if (it == _eventMap.end()){
			return false;
		}
		return true;
	}
private:
	SignalMap _eventMap;
	utils::Mutex _mutex;
};


#endif //!__CONVER_SYNC_EVENT_H__
