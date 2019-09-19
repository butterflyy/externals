#pragma once
#include <hash_set>
#include <farfrr\AnalyseData.h>

namespace farfrr{
	class ReadThread;
	class AnalyseThread : public utils::Thread
	{
	public:
		typedef std::hash_set<std::string> TrueMap;

		AnalyseThread(ReadThread* readThread, int threadIndex);
		~AnalyseThread();

		void SetTrue(const TrueMap& trueMap){
			_trueMap = trueMap;
		}

		static std::string MakeKey(const std::string& id1, const std::string& id2){
			if (id1 >= id2){
				return id1 + " " + id2;
			}
			else{
				return id2 + " " + id1;
			}
		}

		const ResultData& GetResultData(){
			return _result;
		}

	private:
		void handle(Data* data);
		void run() override;
	private:
		ReadThread* _readThread;
		int _threadIndex;

		TrueMap _trueMap;

		ResultData _result;
	};
}

