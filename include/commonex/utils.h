//author : lyg
//date : 2020-9-15
//test : utils_thread.cpp
#ifndef _COMMONEX_UTILITY_H_
#define _COMMONEX_UTILITY_H_

#include <config.h>
#ifdef USE_BOOST
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#else
#include <thread>
#include <condition_variable>
#endif

namespace utils  {
	class thread {
#ifdef USE_BOOST
		BOOST_THREAD_NO_COPYABLE(thread)
#else
#endif
	public:
		thread() noexcept
			:_quit(false),
			_running(false)
		{}

		virtual ~thread() noexcept {
		}

		/*
		* @brief Start thread
		* @return error
		*/
		void start() {
#ifdef USE_BOOST
			boost::unique_lock<boost::mutex> lock(_mutex);

			if (_running) {
				return;
			}

			//release last thread data
			ensure_thread_over();

			_quit = false;

			_thread = boost::thread(boost::bind(&thread::work_thread, this));

			_cond.wait(lock, boost::bind(&thread::thread_is_start, this));
#else
			std::unique_lock<std::mutex> lock(_mutex);

			if (_running) {
				return;
			}

			//release last thread data
			ensure_thread_over();

			_quit = false;

			_thread = std::thread(&thread::work_thread, this);

			_cond.wait(lock, std::bind(&thread::thread_is_start, this));
#endif
		}

		bool is_quit() const {
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			lock_guard<mutex> lock(_mutex);
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
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			lock_guard<mutex> lock(_mutex);
			return _running;
		}

		void yield() {
#ifdef USE_BOOST
			_thread.yield();
#else
			msleep(1);
#endif

		}

		/* @brief This thread sleep milliseconds */
		static void msleep(int ms) {
#ifdef USE_BOOST
			using namespace boost;
#else
			using namespace std;
#endif

			this_thread::sleep_for(chrono::milliseconds(ms));
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

		void ensure_thread_over() noexcept {
			if (_thread.joinable()) {
				_thread.join();
			}

			_running = false;
		}

	private:
#ifdef USE_BOOST
		boost::thread _thread;
		boost::condition_variable _cond;
		mutable boost::mutex _mutex;
#else
		std::thread _thread;
		std::condition_variable _cond;
		mutable std::mutex _mutex;
#endif
		volatile bool _quit;
		volatile bool _running;
	};
}//! namespace


#endif //! _COMMONEX_UTILITY_H_
