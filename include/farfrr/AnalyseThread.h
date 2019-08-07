#pragma once

#include <farfrr\AnalyseData.h>

namespace farfrr{
	class ReadThread;
	class AnalyseThread : public utils::Thread
	{
	public:
		AnalyseThread(ReadThread* readThread, int threadIndex);
		~AnalyseThread();
	private:
		void handle(Data* data);
		void run() override;
	private:
		ReadThread* _readThread;
		int _threadIndex;
	};
}

