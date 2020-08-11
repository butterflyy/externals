//Config base class.
//author: lyg
#ifndef __CONFIG_BASE_H__
#define __CONFIG_BASE_H__

#include <sstream>
#include <common/utils.h>
#include <common/SimpleIni.h>

template<typename ConfigData>
class ConfigBase{
public:
	ConfigBase(){

	}
	virtual ~ConfigBase(){
		
	}

	template<typename KeyType>
	void AddItemString(const KeyType& item, 
		const std::string& section,
		const std::string& key,
		const KeyType& default_value){
		Item item_;
		item_.item = (void*)&item;
		if (typeid(KeyType) == typeid(std::string)){
			item_.type = String;
		}
		else if (typeid(KeyType) == typeid(long)){
			item_.type = Long;
		}
		else if (typeid(KeyType) == typeid(bool)){
			item_.type = Bool;
		}
		else if (typeid(KeyType) == typeid(double)){
			item_.type = Double;
		}
		else{
			assert(false);
		}
		item_.default_value = new KeyType(default_value);
		item_.section = section;
		item_.key = key;

		_items.push_back(item_);
	}

	virtual bool Init(const std::string& path){
		_path = path;
		if (utils::CheckFileExist(_path)){
			if (!load()){
				return false;
			}
		}
		else{
			set();
			if (!gen()){
				return false;
			}
		}

		return true;
	}

	virtual bool Save(){
		return gen();
	}

	//const value
	const ConfigData& Data() const{
		return _data;
	}

	//edit value
	ConfigData& Data_(){
		return _data;
	}

	//display config txt
	virtual std::string DisplayText() const{
		std::string str;
		str += utils::StrFormat("Config file : %s\r\n", _path.c_str());
		for (size_t i = 0; i < _items.size(); i++){
			switch (_items[i].type)
			{
			case String:
				str += utils::StrFormat("%s->%s : %s\r\n", _items[i].section.c_str(), _items[i].key.c_str(),
					((std::string*)(_items[i].item))->c_str());
				break;
			case Long:
				str += utils::StrFormat("%s->%s : %d\r\n", _items[i].section.c_str(), _items[i].key.c_str(),
					*((long*)(_items[i].item)));
				break;
			case Bool:
				str += utils::StrFormat("%s->%s : %s\r\n", _items[i].section.c_str(), _items[i].key.c_str(),
					*((bool*)(_items[i].item)) ? "true" : "false");
				break;
			case Double:
				str += utils::StrFormat("%s->%s : %f\r\n", _items[i].section.c_str(), _items[i].key.c_str(),
					*((double*)(_items[i].item)));
				break;
			default:
				assert(false);
				break;
			}
		}
		return str;
	}

	std::string errMsg() const{
		return _err;
	}

	static std::string MakePath(const std::string& dir, const std::string& name){
		std::ostringstream str;
		str << dir;
#ifdef WIN32
		str << "\\";
#else
		str << "/";
#endif
		str << name;
		return str.str();
	}
protected:
	enum Type{
		String,
		Long,
		Bool,
		Double,
	};

	struct Item{
		Type type;
		void* item;
		std::string section;
		std::string key;
		void* default_value;
	};

	virtual void set(){
		for (size_t i = 0; i < _items.size(); i++){
			switch (_items[i].type)
			{
			case String:
				*((std::string*)(_items[i].item)) = *((std::string*)_items[i].default_value);
				break;
			case Long:
				*((long*)(_items[i].item)) = *((long*)_items[i].default_value);
				break;
			case Bool:
				*((bool*)(_items[i].item)) = *((bool*)_items[i].default_value);
				break;
			case Double:
				*((double*)(_items[i].item)) = *((double*)_items[i].default_value);
				break;
			default:
				assert(false);
				break;
			}
		}
	}

	virtual bool load(){
		CSimpleIniA ini;
		SI_Error ret;

		ret = ini.LoadFile(_path.c_str());

		if (ret < 0){
			_err = utils::StrFormat("Load File Error, ret = %d", ret);
			return false;
		}

		for (size_t i = 0; i < _items.size(); i++){
			switch (_items[i].type)
			{
			case String:
				*((std::string*)(_items[i].item)) = ini.GetValue(_items[i].section.c_str(), _items[i].key.c_str(),
					((std::string*)_items[i].default_value)->c_str());
				break;
			case Long:
				*((long*)(_items[i].item)) = ini.GetLongValue(_items[i].section.c_str(), _items[i].key.c_str(),
					*((long*)_items[i].default_value));
				break;
			case Bool:
				*((bool*)(_items[i].item)) = ini.GetBoolValue(_items[i].section.c_str(), _items[i].key.c_str(),
					*((bool*)_items[i].default_value));
				break;
			case Double:
				*((double*)(_items[i].item)) = ini.GetDoubleValue(_items[i].section.c_str(), _items[i].key.c_str(),
					*((double*)_items[i].default_value));
				break;
			default:
				break;
			}
		}

		return true;
	}

	virtual bool gen(){
		CSimpleIniA ini;
		SI_Error ret;

		for (size_t i = 0; i < _items.size(); i++){
			switch (_items[i].type)
			{
			case String:
				ret = ini.SetValue(_items[i].section.c_str(), _items[i].key.c_str(), 
					((std::string*)(_items[i].item))->c_str());
				break;
			case Long:
				ret = ini.SetLongValue(_items[i].section.c_str(), _items[i].key.c_str(),
					*((long*)(_items[i].item)));
				break;
			case Bool:
				ret = ini.SetBoolValue(_items[i].section.c_str(), _items[i].key.c_str(),
					*((bool*)(_items[i].item)));
				break;
			case Double:
				ret = ini.SetDoubleValue(_items[i].section.c_str(), _items[i].key.c_str(),
					*((double*)(_items[i].item)));
				break;
			default:
				break;
			}
		}

		if (ret < 0){
			_err = utils::StrFormat("Set Ini Value Error, ret = %d", ret);
			return false;
		}

		ret = ini.SaveFile(_path.c_str());
		if (ret < 0){
			_err = utils::StrFormat("Save File Error, ret = %d", ret);
			return false;
		}
		
		return true;
	}


private:
	std::string _path;
	std::string _err;

	ConfigData _data;

	std::vector<Item> _items;
};


/*
Using example

Step 1:
Declare Config Data

struct Data{
	long mm;
	long xxx;
	std::string xx;
	std::string bb;
	double dd;
	std::string m;
	bool zzz;
};

Step 2:
Add Item and Default data.

SimpleConfig<Data>::instance().AddItemString<long>(SimpleConfig<Data>::instance().Data().mm, "ConfigD", "mm", 2222);
SimpleConfig<Data>::instance().AddItemString<long>(SimpleConfig<Data>::instance().Data().xxx, "ConfigD", "xxx", 333);
SimpleConfig<Data>::instance().AddItemString<std::string>(SimpleConfig<Data>::instance().Data().xx, "Config", "xxx", "Î¥·¨dd1");
SimpleConfig<Data>::instance().AddItemString<std::string>(SimpleConfig<Data>::instance().Data().bb, "Config", "bb", "Î¥·¨dd2");
SimpleConfig<Data>::instance().AddItemString<std::string>(SimpleConfig<Data>::instance().Data().m, "Config1", "mm", "Î¥·¨dd3");
SimpleConfig<Data>::instance().AddItemString<bool>(SimpleConfig<Data>::instance().Data().zzz, "ConfigD", "zzz", false);
SimpleConfig<Data>::instance().AddItemString<double>(SimpleConfig<Data>::instance().Data().dd, "ConfigD", "dd", 321.123);

Step 3:
Init Config.

if (!SimpleConfig<Data>::instance().Init("d:\\test.ini")){
	cout << SimpleConfig<Data>::instance().errMsg() << std::endl;
}

Step 4:
Using Config Data

long mm = SimpleConfig<Data>::instance().Data().mm;

*/
template<typename ConfigData>
class SimpleConfig : 
	public ConfigBase<ConfigData>
	,public utils::singleton<SimpleConfig<ConfigData>>
{
};
#endif //!__CONFIG_BASE_H__
