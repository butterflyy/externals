#include "AnalyseThread.h"
#include "ReadThread.h"
#include <common\message_.h>

namespace farfrr
{
	AnalyseThread::AnalyseThread(ReadThread* readThread, int threadIndex)
		: _readThread(readThread),
		_threadIndex(threadIndex){
	}


	AnalyseThread::~AnalyseThread()
	{
		Thread::quit();
	}


	void AnalyseThread::handle(Data* data){
		for (int i = 0; i < data->lines.size(); i++){
			std::string line = data->lines[i];
			std::vector<std::string> dests;
			int size = utils::Spit(line, " ", dests);
			if (dests.size() != 5){
				utils::ShowMessageG("score line error");
				return;
			}

			if (dests[0] == dests[1]){//same tmpl
				continue;
			}

			int score = utils::Stoi(dests[2].c_str());
			int angle = utils::Stoi(dests[3].c_str());
			int valnm = utils::Stoi(dests[4].c_str());

			bool issame = _trueMap.find(MakeKey(dests[0], dests[1])) != _trueMap.end();

			if (issame){//same file
				_result.score.scoreSame[score]++;
				if (score < 30){
					_result.samenoMatchs.push_back(line);
				}
			}
			else{//diff
				_result.score.scoreDiff[score]++;
				if (score > 34){
					_result.diffMatchs.push_back(line);
				}
			}
			_result.count++;

			if (valnm > 35){//valnm score
				if (issame){//same file
					_result.scoreValnm.scoreSame[score]++;
				}
				else{//diff
					_result.scoreValnm.scoreDiff[score]++;
				}

				_result.countValnm++;
			}
		}
	}


	void AnalyseThread::run(){
		_result.clear();

		while (!Thread::isQuit()){
			if (_readThread->IsEmpty()){
				Thread::msleep(10);
				continue;
			}
			Data* data = _readThread->PopTask();

			int task_size = _readThread->TaskSize();
			utils::OutputDebug("Thread<%d> 队列中待处理记录数 = %d \n", _threadIndex, task_size);


			handle(data);
			utils::OutputDebug("Thread<%d> 处理数据大小 = %d \n", _threadIndex, data->lines.size());

			SAFE_DELETE(data);
		}
	}

}