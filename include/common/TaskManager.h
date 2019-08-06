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

	bool IsEmptySafe(){
		_mutex.lock();
		bool isEmpty = _tasks.empty();
		_mutex.unlock();

		return isEmpty;
	}

	int TaskSize(){
		_mutex.lock();
		int size = _tasks.size();
		_mutex.unlock();
		return size;
	}

private:
	std::queue<T> _tasks;
	utils::Mutex _mutex;
};

#endif //!__TASK_MANAGER_H__
