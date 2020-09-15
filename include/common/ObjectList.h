//author : lyg
//data : 2020-8

#ifndef __OBJECT_LIST_H__
#define __OBJECT_LIST_H__

#include <list>
#include <common/utils.h> //for mutex

template<typename T>
struct DefaultDeleter{
	typedef T item_type;

	void operator()(const item_type& item){
		//do nothing
	}
};

template<typename T>
struct DefaultDeleter<T*>{
	typedef T* item_type;

	void operator()(const item_type& item){
		if (item)
			delete item;
	}
};

template<typename T>
class ObjectList{
public:
	typedef std::list<T> List;
	typedef T item_type;

	void Add(const item_type& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		_items.push_back(item);
	}

#ifdef ENABLE_C11

	void Add(const item_type&& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		_items.push_back(std::move(item));
	}

#endif

	template<typename Deleter>
	void Remove(const item_type& item, Deleter d){
		utils::LockGuard<utils::Mutex> lock(_mutex);

		for (typename List::iterator it = _items.begin();
			it != _items.end();){
			if (*it == item){
				d(*it);
				it = _items.erase(it);
			}
			else{
				it++;
			}
		}
	}

	//use default deleter
	void Remove(const item_type& item){
		Remove(item, DefaultDeleter<item_type>());
	}

	bool Has(const item_type& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		for (typename List::const_iterator it = _items.begin();
			it != _items.end();
			++it){
			if (*this == item){
				return true;
			}
		}
		return false;
	}

	template<typename Deleter>
	void Clear(Deleter d){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		for (typename List::iterator it = _items.begin();
			it != _items.end();
			++it){
			d(*it);
		}

		_items.clear();
	}

	//use default deleter
	void Clear(){
		Clear(DefaultDeleter<item_type>());
	}

	item_type& operator[](size_t index){
		if (index >= _items.size()){
			throw std::out_of_range("index big than size");
		}
		assert(index < _items.size());
		int i = 0;
		for (typename List::iterator it = _items.begin();
			it != _items.end();
			++it, ++i){
			if (i == index){
				return *it;
			}
		}
		assert(false);
		return *(_items.begin());
	}

	void Lock(){
		_mutex.lock();
	}

	//return raw data, must need lock to handle data
	List& Data(){
		return _items;
	}

	void Unlock(){
		_mutex.unlock();
	}
private:
	List _items;
	utils::Mutex _mutex;
};


#endif //!__OBJECT_LIST_H__
