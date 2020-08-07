#ifndef __OBJECT_LIST_H__
#define __OBJECT_LIST_H__

#include <list>
#include <common/utils.h> //for mutex

template<typename T>
class ObjectList{
public:
	typedef std::list<item> List;
	typedef T item_type;

	void Add(const item_type& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		_items.push_back(item);
	}

	void Add(const item_type&& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		_items.push_back(std::move(item));
	}

	void Remove(const item_type& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);

		for (List::iterator it = _items.begin();
			it != _items.end();){
			if (*it == item){
				if (std::is_pointer<item_type>::value){
					delete (*it);
				}
				it = _items.erase(it);
			}
			else{
				it++;
			}
		}
	}

	bool Has(const item_type& item){
		utils::LockGuard<utils::Mutex> lock(_mutex);
		for (List::const_iterator it = _items.begin();
			it != _items.end();
			++it){
			if (*this == item){
				return true;
			}
		}
		return false;
	}

	void Clear(){
		utils::LockGuard<utils::Mutex> lock(_mutex);

		if (std::is_pointer<item_type>::value){
			for (List::const_iterator it = _items.begin();
				it != _items.end();
				++it){
				delete (*it);
			}
		}
		_items.clear();
	}

private:
	List _items;
	utils::Mutex _mutex;
};


#endif //!__OBJECT_LIST_H__
