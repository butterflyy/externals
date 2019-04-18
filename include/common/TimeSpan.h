#ifndef TIME_SPAN_H
#define TIME_SPAN_H

#include <time.h>
#ifndef WIN32
#include <sys/time.h>
#endif

class TimeSpan{

public:
	TimeSpan(){
#ifdef WIN32
		_start = 0;
#else
		memset(&_start, 0, sizeof(struct timeval));
#endif
	}

	~TimeSpan(){

	}

	void start(){
#ifdef WIN32
		_start = clock(); //new time
#else
		gettimeofday(&_start, NULL);
#endif
	}

	int restart(){
		int duration = elapsed();
		start();
		return duration;
	}

	int elapsed(){
#ifdef WIN32
		int end = clock();//end time
		int duration = (end - _start);
#else
		struct timeval start = _start;
		struct timeval end;
		memset(&end, 0, sizeof(struct timeval));
		gettimeofday(&end, NULL);
		if (end.tv_usec < start.tv_usec){
			end.tv_usec += 1000;
			end.tv_sec = end.tv_sec - 1;
		}
		int duration = (end.tv_sec - start.tv_sec)*1000 + (end.tv_usec - start.tv_usec) / 1000;
#endif
		return duration;
	}

private:
#ifdef WIN32
	clock_t _start;
#else
	struct timeval _start;
#endif
};
#endif //!TIME_SPAN_H