#ifndef __TASK_MANAGER_H__
#define __TASK_MANAGER_H__

#include <queue>
#include <common/utils.h> //for mutex

template<typename T>
class TaskManager{
public:
	void PushTask(const T& task){
		_mutex.lock();
		_tasks.push(task);
		_mutex.unlock();
	}

	bool IsEmpty(){
		_mutex.lock();
		if (_tasks.empty()){
			_mutex.unlock();
			return true;
		}
		return false;
	}

	T PopTask(){
		T task = _tasks.front();
		_tasks.pop();
		_mutex.unlock();
		return task;
	}

private:
	std::queue<T> _tasks;
	utils::Mutex _mutex;
};

#endif //!__TASK_MANAGER_H__
