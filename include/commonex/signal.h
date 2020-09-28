//author : lyg
//date : 2020-9-15
//test : utils_signal.cpp
#ifndef __COMMONEX_SIGNAL_H__
#define __COMMONEX_SIGNAL_H__

#include <config.h>
#ifdef USE_BOOST
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#else
#include <condition_variable>
#include <mutex>
#endif



namespace utils
{
	class signal {
	public:
#ifdef USE_BOOST
		typedef typename boost::cv_status status;
#else
		typedef typename std::cv_status status;
#endif

		signal()
			:_waiting(false),
			_set(false){
		}

		~signal() {
		}

		void set() {
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			lock_guard<mutex> lock(_mutex);
			_set = true;
			_cond.notify_all();
		}

		void reset() {
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			lock_guard<mutex> lock(_mutex);
			_set = false;
			_cond.notify_all();
		}

		bool is_wait() {
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			lock_guard<mutex> lock(_mutex);
			return _waiting;
		}

		//return status
		status wait(unsigned long msec = 0xFFFFFFFF /*INFINITE*/) {
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			unique_lock<mutex> lock(_mutex);
			if (_set) {//set is first at wait
				_set = false;
				return status::no_timeout;
			}

			_waiting = true;
			cv_status status(cv_status::no_timeout);
			if (msec == 0xFFFFFFFF) {
				_cond.wait(lock);
			}
			else {
				status = _cond.wait_for(lock, chrono::milliseconds(msec));
			}

			_waiting = false;

			if (status == cv_status::no_timeout) {
				_set = false;
			}

			return status;
		}

	private:
#ifdef USE_BOOST
		boost::condition_variable _cond;
		boost::mutex _mutex;
#else
		std::condition_variable _cond;
		std::mutex _mutex;
#endif
		volatile bool _waiting;
		volatile bool _set;
	};

}
#endif //!__COMMONEX_SIGNAL_H__
