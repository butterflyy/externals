//author : lyg
//date : 2020-9-15
//test : utils_signal.cpp
#ifndef __COMMONEX_SIGNAL_H__
#define __COMMONEX_SIGNAL_H__

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>

namespace utils
{
	class signal {
	public:
		typedef typename boost::cv_status status;

		signal()
			:_waiting(false),
			_set(false){
		}

		~signal() {
		}

		void set() {
			boost::lock_guard<boost::mutex> lock(_mutex);
			_set = true;
			_cond.notify_all();
		}

		void reset() {
			boost::lock_guard<boost::mutex> lock(_mutex);
			_set = false;
			_cond.notify_all();
		}

		bool is_wait() {
			boost::lock_guard<boost::mutex> lock(_mutex);
			return _waiting;
		}

		//return status
		status wait(unsigned long msec = 0xFFFFFFFF /*INFINITE*/) {
			boost::unique_lock<boost::mutex> lock(_mutex);
			if (_set) {//set is first at wait
				_set = false;
				return status::no_timeout;
			}

			_waiting = true;
			boost::cv_status status(boost::cv_status::no_timeout);
			if (msec == 0xFFFFFFFF) {
				_cond.wait(lock);
			}
			else {
				status = _cond.wait_for(lock, boost::chrono::milliseconds(msec));
			}

			_waiting = false;

			if (status == boost::cv_status::no_timeout) {
				_set = false;
			}

			return status;
		}

	private:
		boost::condition_variable _cond;
		boost::mutex _mutex;
		volatile bool _waiting;
		volatile bool _set;
	};

}
#endif //!__COMMONEX_SIGNAL_H__
