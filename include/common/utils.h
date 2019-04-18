#ifndef _UTILITY_H_
#define _UTILITY_H_

#define SUPPORT_CPLUSPLUS11

#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>
#include <cstdarg>
#include <cstdlib>

#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#define _access access
#endif

#ifdef SUPPORT_CPLUSPLUS11
#include <thread>
#include <mutex>
#include <atomic>
#else
#include <unistd.h>
#include <pthread.h>

#define nullptr NULL
#define override
#endif



#define SAFE_DELETE(p) { do{ if (p){ delete p; p = NULL; } } while (0); }

#define SAFE_DELETE_ARRAY(p) { do{ if (p){ delete []p; p = NULL; } } while (0); }

#define UN_USED(x) { do{;} while (0); }


typedef unsigned char byte;

namespace utils  {
/*
* @brief Prevent copy constructor for class.
* @detail When using this class simply inherit it privately
*/
class NoCopy {
protected:
    NoCopy() {}
private:
    NoCopy(const NoCopy&);
    NoCopy& operator= (const NoCopy&);
};

/*
* @brief Make all default constructor private.
* @detail This prevents initializing class making it static unless an explicit constructor is declared.
*  When using this class simply inherit it privately
*/
class StaticClass {
private:
    StaticClass();
    StaticClass(const StaticClass&);
    StaticClass& operator=(const StaticClass&);
};


/** @brief Deletes memory safely and points to null */
#ifdef SUPPORT_CPLUSPLUS11
template<typename T>
static inline
typename std::enable_if<std::is_pointer<T*>::value, void>::type
safeDelete(T*& pointer) {
    if (pointer == nullptr) {
        return;
    }

    delete pointer;
    pointer = nullptr;
}

/** @brief Deletes memory safely and points to null */
template<typename T>
static inline
typename std::enable_if<std::is_pointer<T*>::value, void>::type
safeArrayDelete(T*& pointer) {
    if (pointer == nullptr) {
        return;
    }

    delete[] pointer;
    pointer = nullptr;
}

#endif


/*
* @brief Simple thread that using std::thread or pthread
*/


class Thread : NoCopy {
public:
    Thread() : _quit(false), _isRunning(false) {}
    virtual ~Thread() {}

    /*
    * @brief Start thread
    * @param isJoin run join or detach, default is detach
    * @return 0 succeeded, -1 thread already running, or other error code.
	* @problem thread is start last.
    */
    int start(bool isJoin = false) {
        if (_isRunning) {
            return -1;
        }

        _quit = false;
#ifdef SUPPORT_CPLUSPLUS11
        std::thread thd(&Thread::work_thread, this);

        if (isJoin) {
            thd.join();
        } else {
            thd.detach();
        }

#else
        int ret = pthread_create(&_thread_t, NULL, work_thread, (void*)this);

        if (ret != 0) {
            return -2;
        }

        if (isJoin) {
            void* threadResult = 0; // Dummy var.
            ret = pthread_join(_thread_t, &threadResult);

            if (ret != 0) {
                return -3;
            }
        } else {
            ret = pthread_detach(_thread_t);

            if (ret != 0) {
                return -4;
            }
        }

#endif
        return 0;
    }

    bool isQuit() const {
        return _quit;
    }

    /*
    * @brief Stop thread if used _quit variable
    * @ isWaiting Is waiting thread quit, default is true
    */
    void quit(bool isWaiting = true) {
        _quit = true;

        if (isWaiting) {
            while (_isRunning) {
                msleep(10);
            }
        }
    }

    /* @brief Whether the thread is running */
    bool isRunning() const {
        return _isRunning;
    }

    /* @brief This thread sleep milliseconds */
    static void msleep(int ms) {
#ifdef SUPPORT_CPLUSPLUS11
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
#else
        usleep(ms * 1000);
#endif
    }

protected:
    /* @brief Thread's run method. Must be implemented by derived classes.*/
    virtual void run() = 0;

private:
#ifdef SUPPORT_CPLUSPLUS11
    void work_thread() {
        _isRunning = true;
        run();
        _isRunning = false;
    }
#else
    static void* work_thread(void* param) {
        Thread* pThis = (Thread*)param;
        pThis->_isRunning = true;
        pThis->run();
        pThis->_isRunning = false;
    }
#endif
private:
#ifdef SUPPORT_CPLUSPLUS11
    /* @brief Control loop running function quit if used _quit variable to control.*/
    std::atomic<bool> _quit;

    std::atomic<bool> _isRunning;
#else
    volatile bool _quit;

    volatile bool _isRunning;

    pthread_t _thread_t;
#endif
};


/*
* @brief Mutex that using std::mutex or pthread
*/
#ifdef SUPPORT_CPLUSPLUS11
class Mutex : public std::mutex {
public:
    Mutex()
        : std::mutex() {
        // default construct
    }
#else
class Mutex {
public:
    Mutex() {
        // default construct
        pthread_mutex_init(&_thread_mutex_t, nullptr);
    }
    ~Mutex() {
        // clean up
        pthread_mutex_destroy(&_thread_mutex_t);
    }
    void lock() {
        // lock the mutex
        pthread_mutex_lock(&_thread_mutex_t);
    }

    bool try_lock() {
        // try to lock the mutex
        return (pthread_mutex_trylock(&_thread_mutex_t) == 0);
    }

    void unlock() {
        // unlock the mutex
        pthread_mutex_unlock(&_thread_mutex_t);
    }
private:
    pthread_mutex_t _thread_mutex_t;
#endif
private:
    Mutex(const Mutex&);
    Mutex& operator=(const Mutex&);
};

/*
* @brief Class with destructor that unlocks mutex
*/
template<class _Mutex>
class LockGuard {
public:
    explicit LockGuard(_Mutex& _Mtx)
        : _MyMutex(_Mtx) {
        // construct and lock
        _MyMutex.lock();
    }

    ~LockGuard() {
        // unlock
        _MyMutex.unlock();
    }

private:
    _Mutex& _MyMutex;

    LockGuard(const LockGuard&);
    LockGuard& operator=(const LockGuard&) ;
};

/*
* @brief Safely Buffer
*/
class Buffer {
public:
    Buffer()
        : _data(nullptr),
          _size(0) {

    }

    Buffer(const byte* data, size_t size) : _data(nullptr), _size(0) {
        if (!data || !size) {
            return;
        }

        allocBuffer(size);
        memcpy(_data, data, size);
    }

    Buffer(size_t size) : _data(nullptr), _size(0)  {
        allocBuffer(size);
    }

    Buffer(const Buffer& buffer) : _data(nullptr), _size(0)  {
        *this = buffer;
    }

    Buffer(const Buffer* pbuffer) : _data(nullptr), _size(0)  {
        if (!pbuffer) {
            return;
        }

        *this = *pbuffer;
    }

    ~Buffer() {
        releaseBuffer();
    }

    Buffer& assign(const byte* data, size_t size) {
        allocBuffer(size);

        if (data && size > 0) {
            memcpy(_data, data, size);
        }

        return *this;
    }

    byte* data() const {
        return _data;
    }

    size_t size() const {
        return _size;
    }

    size_t length() const {
        return size();
    }

    void resize(size_t size) {
        allocBuffer(size);
    }

    byte& operator[](size_t pos) const {
		assert(pos < _size);
        return _data[pos];
    }

    Buffer& operator=(const Buffer& right) {
        allocBuffer(right.size());

        if (!right.empty()) {
            memcpy(_data, right.data(), _size);
        }

        return *this;
    }

    bool empty() const {
        return (!_data && _size == 0);
    }

    void clear() {
        releaseBuffer();
    }

    bool operator==(const Buffer& right) const {
        if (_size == right.size()) {
            for (size_t i = 0; i < _size; i++) {
                if ((*this)[i] != right[i]) {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

private:
    void allocBuffer(size_t size) {
        releaseBuffer();
        _size = size;

        if (size) {
            _data = new byte[_size];

            if (!_data) {
                throw std::bad_alloc();
            }

            memset(_data, 0, _size);
        }
    }

    void releaseBuffer() {
        SAFE_DELETE_ARRAY(_data);
        _size = 0;
    }

private:
    byte* _data;
    size_t _size;
};

/** @brief Convert string to wstring */
static std::wstring s2ws(const std::string& s) {
    setlocale(LC_ALL, "chs");
    size_t _dsize = s.size() + 1;
    wchar_t* _dest = new wchar_t[_dsize];
    wmemset(_dest, 0, _dsize);
    const char* _source = s.c_str();
    mbstowcs(_dest, _source, _dsize);
    std::wstring result = _dest;
    delete[] _dest;
    setlocale(LC_ALL, "C");
    return result;
}

/** @brief Convert wstring to string */
static std::string ws2s(const std::wstring& ws) {
    std::string cur_locale = setlocale(LC_ALL, nullptr);
    setlocale(LC_ALL, "chs");
    size_t _dsize = 2 * ws.size() + 1;
    char* _dest = new char[_dsize];
    memset(_dest, 0, _dsize);
    const wchar_t* _source = ws.c_str();
    wcstombs(_dest, _source, _dsize);
    std::string result = _dest;
    delete[] _dest;
    setlocale(LC_ALL, cur_locale.c_str());
    return result;
}

/*
* @brief Check File exist
* @param path file path
* @return true or false
*/
static bool CheckFileExist(const std::string& path) {
	if ((_access(path.c_str(), 0)) != -1){
		return true;
	}
	return false;
}

/*
* @brief Get file size
* @param path file path
* @return -1 failed, others succeeded
*/
static std::streamsize GetFileSize(const std::string& path) {
    std::ifstream file(path.c_str(), std::ios_base::binary);

    if (!file.good()) {
        return -1;
    }

    file.seekg(0, std::ios_base::end);
    std::streamsize size = file.tellg();
    file.close();
    return size;
}

/*
* @brief Read file data to binary buffer
* @param path file path
* @pbuff point to binary buffer
* @return -1 failed, others file size succeeded
*/
static int ReadFile(const std::string& path, byte* buff, int len) {
	FILE* f = fopen(path.c_str(), "rb");
	if (!f)
		return -1;
	size_t size = fread(buff, 1, len, f);
	fclose(f);
	return (int)size;
}

/*
* @brief Read file data to binary buffer
* @param path file path
* @pbuff point to binary buffer
* @return -1 failed, others file size succeeded
* @note Allocation memory in function, Must release memory outside
*/
static int ReadFile(const std::string& path, byte** pbuff) {
    std::ifstream file(path.c_str(), std::ios_base::binary);

    if (!file.good()) {
        return -1;
    }

    file.seekg(0, std::ios_base::end);
    size_t size = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios_base::beg);
    *pbuff = new unsigned char[size];
    file.read(reinterpret_cast<char*>(*pbuff), size);
    file.close();
    return (int)file.gcount();
}

/*
* @brief Write buffer data fo file,Any contents that existed in the file before it is open are discarded
* @param path file path
* @param buff buffer data pointer
* @param len buffer data length
* @return -1 failed, others succeeded
*/
static int WriteFile(const std::string& path, const byte* buff, int len) {
    assert(buff);
    std::ofstream file(path.c_str(), std::ios_base::binary | std::ios_base::trunc);

    if (!file.good()) {
        return -1;
    }

    file.write(reinterpret_cast<const char*>(buff), len);
    file.close();
    return (int)len;
}

/*
* @brief String format
* @param format format
* @return format string
*/
static std::string StrFormat(const char* format, ...) {
    assert(format);
    char buff[1024] = { 0 };
    va_list va;
    va_start(va, format);
    int len = vsnprintf(buff, 1024 - 1, format, va);
    va_end(va);

    if (len == -1) {
        assert(false);
    }

    return buff;
}

/*
* @brief byte data to hex string
* @param byte the ptr of byte data
* @param len the length of byte data
* @param uppercase whether uppercase
* @return hex string
*/
static std::string HexFormat(const byte* byte, int len, bool uppercase = true) {
    assert(byte);
    std::string hex;

    for (int i = 0; i < len; i++) {
        char buff[3] = { 0 };

        if (uppercase) {
            sprintf(buff, "%02X", byte[i]);
        } else {
            sprintf(buff, "%02x", byte[i]);
        }

        hex += buff;
    }

    return hex;
}


/*
* @brief read lines from file
* @param path file path
* @param lines file lines
* @return -1 failed, others is file lines
*/
static int ReadLines(const std::string& path, std::vector<std::string>& lines) {
	FILE* f = fopen(path.c_str(), "rb");
	if (!f) return -1;

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
		lines.push_back(s);
		len += l;
	}
	fclose(f);
	return len;
}

/*
* @brief write lines to file
* @param path file path
* @param lines file lines
* @return -1 failed, others succeeded
*/
static int WriteLines(const std::string& path, const std::vector<std::string>& lines){
	std::string str;
	for (std::vector<std::string>::const_iterator it = lines.begin();
		it != lines.end();
		++it){
		str += *it;
		str += "\r\n";
	}

	return WriteFile(path, (byte*)str.c_str(), (int)str.size());
}

/*
* @brief spit sring
* @param src source string
* @param separator spit separator string
* @param dests return string
* @return spit success string size
*/
static int Spit(const std::string& src, const std::string& separator, std::vector<std::string>& dests){
	if (src.empty() || separator.empty())
		return 0;

	int splen = (int)separator.size();

	int last = 0;

	while (true){
		int index = (int)src.find(separator, last);
		if (index == -1){
			dests.push_back(src.substr(last));
			break;
		}
		else{
			dests.push_back(src.substr(last, index - last));
			last = index + splen;
		}
	}

	return (int)dests.size();
}

}//! namespace

#endif //! _UTILITY_H_