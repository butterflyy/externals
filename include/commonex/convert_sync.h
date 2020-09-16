//author : lyg
//date : 2020-9-15
//test : utils_convert_sync.cpp
#ifndef __COMMONEX_CONVERT_SYNC_H__
#define __COMMONEX_CONVERT_SYNC_H__

#include <map>
#include <commonex/signal.h>

namespace utils {
	template<typename SID, typename Data>
	class convert_sync {
	public:
		enum status {
			no_timeout,
			timeout,
			no_id,
		};

		struct Param
		{
			signal* sig;
			Data data;
		};

		typedef std::map<SID, Param> SignalMap;

		convert_sync() {
		}

		~convert_sync() {
			boost::lock_guard<boost::mutex> lock(_mutex);
			for (typename SignalMap::const_iterator it = _eventMap.begin();
				it != _eventMap.end();
				++it) {
				delete it->second.sig;
			}
			_eventMap.clear();
		}

		bool add(SID id) {
			boost::lock_guard<boost::mutex> lock(_mutex);

			if (find(id)) {
				return false;
			}

			//create event
			signal* sig = new signal();

			Param param;
			param.sig = sig;

			_eventMap[id] = param;
			
			return true;
		}

		//data out return waiting data
		//return status
		//import : remove id when call.
		status wait_once(SID id, Data* data, unsigned long msec = 0xFFFFFFFF /*INFINITE*/) {
			boost::unique_lock<boost::mutex> lock(_mutex);

			if (!find(id)) {
				return no_id;
			}

			signal* sig = _eventMap[id].sig;
			lock.unlock();
			status ret = (status)sig->wait(msec);
			lock.lock();

			if (ret == status::no_timeout) {//success
				*data = _eventMap[id].data;
			}

			//clear event
			delete sig;
			_eventMap.erase(id);

			return ret;
		}

		//data set return waiting data
		//return status
		bool set(SID id, Data data) {
			boost::lock_guard<boost::mutex> lock(_mutex);

			if (!find(id)) {
				return false;
			}

			_eventMap[id].data = data;
			_eventMap[id].sig->set();

			return true;
		}

	private:
		bool find(SID id) {
			typename SignalMap::const_iterator it = _eventMap.find(id);
			if (it == _eventMap.end()) {
				return false;
			}
			return true;
		}
	private:
		SignalMap _eventMap;
		boost::mutex _mutex;
	};
}


#endif //!__COMMONEX_CONVERT_SYNC_H__
