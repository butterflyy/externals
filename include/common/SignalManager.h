#ifndef __SIGNAL_MANAGER_H__
#define __SIGNAL_MANAGER_H__

#include <map>
#include <common/Signal_.h>


class SignalManager{
public:
	typedef unsigned long SID;

	typedef std::map<SID, Signal*> SignalMap;

	SignalManager(){
	}

	~SignalManager(){
		for (SignalMap::const_iterator it = _eventMap.begin();
			it != _eventMap.end();
			++it){
			delete it->second;
		}
	}

	int CreateSignal(SID id){
		if (find(id)){
			assert(false);
			return false;
		}
		
		//create event
		Signal* pEvent = new Signal();
		_eventMap[id] = pEvent;
		
		return 0;
	}

	int CloseSignal(SID id){
		if (!find(id)){
			assert(false);
			return false;
		}

		SAFE_DELETE(_eventMap[id]);
		return 0;
	}

	int SetSignal(SID id){
		if (!find(id)){
			assert(false);
			return false;
		}

		return _eventMap[id]->SetSignal();
	}

	int ResetSignal(SID id){
		if (!find(id)){
			assert(false);
			return false;
		}

		return _eventMap[id]->ResetSignal();
	}

	bool IsWait(SID id){
		if (!find(id)){
			assert(false);
			return false;
		}
		return _eventMap[id]->IsWait();
	}

	int WaitSignal(SID id, unsigned long msec = 0xFFFFFFFF /*INFINITE*/){
		if (!find(id)){
			assert(false);
			return false;
		}

		return _eventMap[id]->WaitSignal(msec);
	}

private:
	bool find(SID id){
		SignalMap::const_iterator it = _eventMap.find(id);
		if (it == _eventMap.end()){
			return false;
		}
		return true;
	}
private:
	SignalMap _eventMap;
};

#endif //!__SIGNAL_MANAGER_H__
