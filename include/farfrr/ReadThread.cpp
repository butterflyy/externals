#include "ReadThread.h"
#include "AnalyseThread.h"
#include <common\message_.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

namespace farfrr{

	ReadThread::ReadThread(){
		for (int i = 0; i < 8; i++){
			_analyseThreads.push_back(new AnalyseThread(this, i));
		}
	}


	ReadThread::~ReadThread()
	{
		for (int i = 0; i < _analyseThreads.size(); i++){
			delete _analyseThreads[i];
		}
	}

	void ReadThread::SetWork(const std::vector<std::string>& resultpaths, const std::string& truefile){
		_resultpaths = resultpaths;

		//init data
		_trueMap.clear();
		_result.count = 0;
		memset(&_result.score, 0, sizeof(farfrr::Score));
		_result.countValnm = 0;
		memset(&_result.scoreValnm, 0, sizeof(farfrr::Score));
		_result.diffMatchs.clear();
		_result.samenoMatchs.clear();
		
		//read true lines
		std::vector<std::string> true_lines;
		int size = utils::ReadLines(truefile, true_lines);
		if (size < 0){
			utils::ShowMessageG("read true file failed");
			return;
		}


		for (int i = 0; i < true_lines.size(); i++){
			std::vector<std::string> dests;
			int size = utils::Spit(true_lines[i], " ", dests);
			if (dests.size() != 2){
				utils::ShowMessageG("ture line error");
				return;
			}

			_trueMap.insert(std::pair<std::string, std::string>(dests[0], dests[1]));
		}

		Thread::start();

		for (int i = 0; i < _analyseThreads.size(); i++){
			_analyseThreads[i]->start();
		}
	}


	void ReadThread::WaitWorkFinished(){
		Thread::quit();

		Thread::msleep(10); //waiting get task

		for (int i = 0; i < _analyseThreads.size(); i++){
			_analyseThreads[i]->quit();
		}
	}


	void ReadThread::run(){
		for (int i = 0; i < _resultpaths.size(); i++){
			utils::OutputDebug("read result file, i = %d, path = %s", i, _resultpaths[i].c_str());
			FILE* f = fopen(_resultpaths[i].c_str(), "rb");
			if (!f){
				utils::ShowMessageG("open score file failed");
				return;
			}

			Data* data = nullptr;

			int len = 0;
			char line[1024];
			while (true){
				fgets(line, 1024, f);
				if (feof(f)) break;
				int l = (int)strlen(line);

				//check file
				if (l < 4) continue;

				int less(0);
				if (line[l - 1] == '\n'){
					less++;
				}
				if (line[l - 2] == '\r'){
					less++;
				}
				std::string s(line, l - less);
				//lines.push_back(s);

				if (!data){
					data = new Data;
				}
				data->lines.push_back(s);
				if (data->lines.size() >= 10000){ //add thread handing

					while (TaskSize() > 300){
						Thread::msleep(1000);
					}
					PushTask(data);
					data = nullptr;
				}

				len += l;
			}
			fclose(f);

			if (data){
				PushTask(data);
				data = nullptr;
			}
		}

	}

}

#ifdef _MSC_VER
#pragma warning(pop)
#endif