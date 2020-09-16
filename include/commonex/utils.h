//author : lyg
//date : 2020-9-15
//test : utils_thread.cpp
#ifndef _COMMONEX_UTILITY_H_
#define _COMMONEX_UTILITY_H_

#include <boost/thread.hpp>
#include <boost/atomic.hpp>

namespace utils  {
	class thread {
		BOOST_THREAD_NO_COPYABLE(thread)
	public:
		thread() BOOST_NOEXCEPT
			:_quit(false),
			_running(false)
		{}

		virtual ~thread() BOOST_NOEXCEPT {
		}

		/*
		* @brief Start thread
		* @return error
		*/
		void start() {
			boost::unique_lock<boost::mutex> lock(_mutex);

			if (_running) {
				return;
			}

			//release last thread data
			ensure_thread_over();

			_quit = false;

			_thread = boost::thread(boost::bind(&thread::work_thread, this));

			_cond.wait(lock, boost::bind(&thread::thread_is_start, this));
		}

		bool is_quit() const {
			boost::lock_guard<boost::mutex> lock(_mutex);
			return _quit;
		}

		/*
		* @brief Stop thread if used _quit variable
		* @ isWaiting Is waiting thread quit, default is true
		*/
		void quit(bool isWaiting = true) {
			_mutex.lock();
			_quit = true;
			_mutex.unlock();

			if (isWaiting) {
				ensure_thread_over();
			}
		}

		/* @brief Whether the thread is running */
		bool is_running() const {
			boost::lock_guard<boost::mutex> lock(_mutex);
			return _running;
		}

		void yield() {
			_thread.yield();
		}

		/* @brief This thread sleep milliseconds */
		static void msleep(int ms) {
			boost::this_thread::sleep_for(boost::chrono::milliseconds(ms));
		}

	protected:
		/* @brief Thread's run method. Must be implemented by derived classes.*/
		virtual void run() = 0;

	private:
		bool thread_is_start() {
			return _running;
		}

		void work_thread() {
			_mutex.lock();
			_running = true;
			_cond.notify_one();
			_mutex.unlock();

			try {
				run();
			}
			catch (...) {
				//avoid exception;
			}
		}

		void ensure_thread_over() BOOST_NOEXCEPT {
			if (_thread.joinable()) {
				_thread.join();
			}

			_running = false;
		}

	private:
		boost::thread _thread;
		boost::condition_variable _cond;
		mutable boost::mutex _mutex;
		volatile bool _quit;
		volatile bool _running;
	};
}//! namespace


#endif //! _COMMONEX_UTILITY_H_
