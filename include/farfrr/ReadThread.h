#pragma once
#include <common\TaskManager.h>
#include <map>
#include <farfrr\AnalyseData.h>


namespace farfrr{

	class AnalyseThread;
	class ReadThread : protected utils::Thread, public TaskManager<Data*>
	{
	public:
		friend class AnalyseThread;

		typedef std::multimap<std::string, std::string> TrueMap;

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
		TrueMap _trueMap;

		ResultData _result;
		utils::Mutex _resultMutex;
	};

}
