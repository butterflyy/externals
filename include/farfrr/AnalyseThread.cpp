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

			int score = utils::Stoi(dests[2].c_str());
			int angle = utils::Stoi(dests[3].c_str());
			int valnm = utils::Stoi(dests[4].c_str());

			bool issame = false;

			std::pair<ReadThread::TrueMap::const_iterator, ReadThread::TrueMap::const_iterator>
				trueResult = _readThread->_trueMap.equal_range(dests[0]);
			for (ReadThread::TrueMap::const_iterator it = trueResult.first; it != trueResult.second; ++it){
				if (it->second == dests[1]){
					issame = true;
					break;
				}
			}


			utils::LockGuard<utils::Mutex> lock(_readThread->_resultMutex);

			if (issame){//same file
				_readThread->_result.score.scoreSame[score]++;
				if (score < 30){
					_readThread->_result.samenoMatchs.push_back(line);
				}
			}
			else{//diff
				_readThread->_result.score.scoreDiff[score]++;
				if (score > 34){
					_readThread->_result.diffMatchs.push_back(line);
				}
			}
			_readThread->_result.count++;

			if (valnm > 35){//valnm score
				if (issame){//same file
					_readThread->_result.scoreValnm.scoreSame[score]++;
				}
				else{//diff
					_readThread->_result.scoreValnm.scoreDiff[score]++;
				}

				_readThread->_result.countValnm++;
			}
		}
	}


	void AnalyseThread::run(){
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