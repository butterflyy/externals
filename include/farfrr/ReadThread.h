#pragma once
#include <common\TaskManager.h>
#include <farfrr\AnalyseData.h>


namespace farfrr{

	class AnalyseThread;
	class ReadThread : protected utils::Thread, public TaskManager<Data*>
	{
	public:
		friend class AnalyseThread;

		ReadThread();
		~ReadThread();

		void SetWork(const std::vector<std::string>& resultpaths, const std::string& truefile);

		void WaitWorkFinished();

		const ResultData& GetResultData(){
			return _result;
		}

	private:
		void run() override;

	private:
		std::vector<AnalyseThread*> _analyseThreads;

		std::vector<std::string> _resultpaths;

		ResultData _result;
	};

}
