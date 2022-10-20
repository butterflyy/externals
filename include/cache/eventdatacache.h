/* ====================================================================
 * Copyright (c) 2021 qingteng.  All rights reserved.
 *
 * Author: yonggang.liu@qingteng.cn
 *
 * ====================================================================
 */

#pragma once

#include <time.h>
#include <string.h>
#ifndef _WIN32
#include <sys/time.h>
#endif

#include <unordered_map>
#include <list>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cassert>
#include <array>
#include <climits>

#define FRIEND_TEST(test_case_name, test_name) \
    friend class test_case_name##_##test_name##_Test

namespace Cache {
#ifdef _WIN32
class TimeSpan {
 public:
    TimeSpan() { _start = 0; }

    ~TimeSpan() {}

    void start() { _start = clock(); }

    int restart() {
        int duration = elapsed();
        start();
        return duration;
    }

    // millisecond
    int elapsed() { return (clock() - _start); }

 private:
    clock_t _start;
};
#else
class TimeSpan {
 public:
    TimeSpan() { memset(&_start, 0, sizeof(struct timeval)); }

    ~TimeSpan() {}

    void start() { gettimeofday(&_start, nullptr); }

    int restart() {
        int duration = elapsed();
        start();
        return duration;
    }

    // millisecond
    int elapsed() {
        struct timeval start = _start;
        struct timeval end;
        memset(&end, 0, sizeof(struct timeval));
        gettimeofday(&end, nullptr);
        int duration = int(end.tv_sec - start.tv_sec) * 1000 +
                       int(end.tv_usec - start.tv_usec) / 1000;
        return duration;
    }

 private:
    struct timeval _start;
};
#endif
template <typename _EI, typename _ED, bool _NeedEvent = true>
class BasicEventDataCache {
 public:
    typedef _EI EventID;
    typedef _ED EventData;

    typedef typename std::cv_status status;

    // Add some addition item
    // clock_t is only for timestamp return, because clock() is only cpu spec
    // time inner used timeval instead.
    //  Cache expiry algorithm
    //  start : the timespan
    //  expiry : expiry time
    //	now : now time
    //	live time = expiry - (now - begin)
    //	QueryCheck is the sort live time, first is the largest.
    //	then if check expiry, only need to check the QueryCheck last.
    struct CacheData {
        EventData eventData;
        std::size_t eventSize;  // event caption size
        std::size_t expiryTime;
        clock_t timestamp;
        TimeSpan timespan;
    };

    typedef std::shared_ptr<CacheData> CacheDataPtr;
    typedef std::pair<EventID, CacheDataPtr> CheckData;

    typedef std::unordered_map<EventID, CacheDataPtr> MapData;
    typedef std::list<CheckData> QueueCheck;
    typedef std::list<std::pair<EventID, clock_t>> ListID;

    static const std::size_t CAPTION_SIZE = 10 * 1024 * 1024;
    // 10 MB, only event data size, not all need caption size

    static const std::size_t EXPIRY_TIME = 600;
    // default expiry time, 600 second

    explicit BasicEventDataCache(std::size_t captionSize = CAPTION_SIZE,
                                 std::size_t expiryTime = EXPIRY_TIME)
        : _captionSize(captionSize), _currentSize(0), _expiryTime(expiryTime) {}

    ~BasicEventDataCache() {}

    // insert event element.
    void Add(const EventID& id, const EventData& data, std::size_t size) {
        return emplace(id, data, size, _expiryTime);
    }

    // insert by moving into event element.
    void Add(const EventID& id, EventData&& data, std::size_t size) {
        return emplace(id, std::move(data), size, _expiryTime);
    }

    // insert event element.
    void Add(const EventID& id, const EventData& data, std::size_t size,
             std::size_t expiry) {
        return emplace(id, data, size, expiry);
    }

    // insert by moving into event element.
    void Add(const EventID& id, EventData&& data, std::size_t size,
             std::size_t expiry) {
        return emplace(id, std::move(data), size, expiry);
    }

    bool Has(const EventID& id) const {
        std::lock_guard<std::mutex> lock(_mutex);
        return (_datas.find(id) != _datas.end());
    }

    // Get copy cache data
    bool Get(const EventID& id, EventData& data) const {
        clock_t timeSpan;
        return Get(id, data, timeSpan);
    }

    // Get copy cache data
    bool Get(const EventID& id, EventData& data, clock_t& timestamp) const {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _datas.find(id);
        if (it == _datas.end()) {
            return false;
        }

        data = it->second->eventData;
        timestamp = it->second->timestamp;

        return true;
    }

    // Get original cache data
    bool Get(const EventID& id, CacheDataPtr& cache) const {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = _datas.find(id);
        if (it == _datas.end()) {
            return false;
        }

        cache = it->second;

        return true;
    }

    void Remove(const EventID& id) {
        std::lock_guard<std::mutex> lock(_mutex);
        // check if has this item
        if (_datas.find(id) != _datas.end()) {
            _datas.erase(id);
            // has this item, need remove id
            for (auto it = _queueCheck.begin(); it != _queueCheck.end(); ++it) {
                if (it->first == id) {
                    _queueCheck.erase(it);
                    break;
                }
            }
        }
    }

    bool Empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _datas.empty();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(_mutex);
        _datas.clear();
        _currentSize = 0;
        _queueCheck.clear();
        _eventIds.clear();
    }

    // wait to get specified id event.
    status Wait(const EventID& id,
                unsigned long msec = ULONG_MAX /*INFINITE*/) {
        assert(!"Do not has unit test");
        if (!_NeedEvent) {
            throw std::logic_error("Disabled event");
        }

        TimeSpan span;
        span.start();

        while (span.elapsed() < msec) {
            if (WaitAny(msec) == status::timeout) {
                return status::timeout;
            }

            if (Has(id)) {
                return status::no_timeout;
            } else {
                continue;
            }
        }
        return status::timeout;
    }

    status WaitAny(unsigned long msec = ULONG_MAX /*INFINITE*/) {
        assert(!"Do not has unit test");
        if (!_NeedEvent) {
            throw std::logic_error("Disabled event");
        }

        std::unique_lock<std::mutex> lock(_waitMutex);
        std::cv_status status(std::cv_status::no_timeout);
        if (msec == ULONG_MAX) {
            _waitCond.wait(lock);
        } else {
            status = _waitCond.wait_for(lock, std::chrono::milliseconds(msec));
        }

        return status;
    }

    // wait event has add, then return all event id.
    // only one thread can used, because swap all add event id.
    status WaitAll(ListID& ids, unsigned long msec = ULONG_MAX /*INFINITE*/) {
        if (!_NeedEvent) {
            throw std::logic_error("Disabled event");
        }

        ids.clear();
        _mutex.lock();
        if (_eventIds.empty()) {
            _mutex.unlock();

            // wait add event
            {
                std::unique_lock<std::mutex> lock(_waitMutex);
                std::cv_status status(std::cv_status::no_timeout);
                if (msec == ULONG_MAX) {
                    _waitCond.wait(lock);
                } else {
                    status = _waitCond.wait_for(
                        lock, std::chrono::milliseconds(msec));
                }

                if (status == std::cv_status::timeout) {
                    return status;
                }
            }

            // get data
            _mutex.lock();
            ids.swap(_eventIds);
            _mutex.unlock();
        } else {
            ids.swap(_eventIds);
            _mutex.unlock();
        }

        return std::cv_status::no_timeout;
    }

 private:
    template <typename T>
    void emplace(const EventID& id, T&& data, std::size_t size,
                 std::size_t expiry) {
        std::shared_ptr<CacheData> cache(new CacheData);
        cache->eventData = std::move(data);
        cache->eventSize = size;
        cache->expiryTime = expiry;
        cache->timestamp = clock();
        cache->timespan.start();

        // add data
        {
            std::lock_guard<std::mutex> lock(_mutex);
            // check if need remove item
            check();
            // check if has this item
            if (_datas.find(id) != _datas.end()) {
                // has this item, need remove id
                for (auto it = _queueCheck.begin(); it != _queueCheck.end();
                     ++it) {
                    if (it->first == id) {
                        _queueCheck.erase(it);
                        break;
                    }
                }
            }

            _datas[id] = cache;
            _currentSize += cache->eventSize;

            // Add latest expiry item to first.
            // front is newest item.
            int new_live_time = expiry;
            auto it = _queueCheck.begin();
            for (; it != _queueCheck.end(); ++it) {
                int live_time = it->second->expiryTime -
                                it->second->timespan.elapsed() / 1000;
                if (new_live_time >= live_time) {
                    break;
                }
            }
            _queueCheck.insert(it, std::make_pair<>(id, cache));

            if (_NeedEvent) {
                _eventIds.push_back(std::make_pair<>(id, cache->timestamp));
            }
        }

        if (_NeedEvent) {
            // notify data is coming.
            _waitCond.notify_all();
        }
    }

    // check is full and clear full data or expiry time data.
    void check() {
        // first check expiry time
        while (true) {
            if (_queueCheck.empty()) break;

            int expiry = _queueCheck.back().second->timespan.elapsed() / 1000;
            if (expiry <
                static_cast<int>(_queueCheck.back().second->expiryTime))
                break;

            // remove data
            _currentSize -= _queueCheck.back().second->eventSize;
            _datas.erase(_queueCheck.back().first);
            _queueCheck.pop_back();
        }

        // second check caption size
        while (!_queueCheck.empty() && _currentSize >= _captionSize) {
            _currentSize -= _queueCheck.back().second->eventSize;
            _datas.erase(_queueCheck.back().first);
            _queueCheck.pop_back();
        }
    }

    BasicEventDataCache(const BasicEventDataCache&);
    const BasicEventDataCache& operator=(const BasicEventDataCache&);

    FRIEND_TEST(EventDataCacheTests, QueueData);  // for test
 private:
    MapData _datas;
    mutable std::mutex _mutex;

    const std::size_t _captionSize;
    std::size_t _currentSize;

    const std::size_t _expiryTime;

    // for fast check event data is full or expiry.
    // reverse push data, because need to insert item
    QueueCheck _queueCheck;

    // for fast get wait event id data list
    ListID _eventIds;

    // for wait get event id
    std::condition_variable _waitCond;
    std::mutex _waitMutex;
};

template <typename _T>
class QueueDataCache {
 public:
    typedef _T Type;

    typedef typename std::cv_status status;

    void Add(const Type& data) { return emplace(data); }

    // insert by moving into data element.
    void Add(Type&& data) { return emplace(std::move(data)); }

    status WaitOne(Type& data, unsigned long msec = ULONG_MAX /*INFINITE*/) {
        std::unique_lock<std::mutex> lock(mutex_);

        std::cv_status status(std::cv_status::no_timeout);

        if (datas_.empty()) {
            if (msec == ULONG_MAX) {
                cond_.wait(lock);
            } else {
                status = cond_.wait_for(lock, std::chrono::milliseconds(msec));
            }
        }

        if (status == std::cv_status::no_timeout) {
            std::swap(data, datas_.front());
            datas_.pop();
        }

        return status;
    }

 private:
    template <typename T>
    void emplace(T&& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        datas_.push(std::move(data));
        cond_.notify_all();
    }

 private:
    std::queue<Type> datas_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

template <typename _T, std::size_t _Cap>
class FixQueueDataCache {
 public:
    typedef _T Type;

    typedef typename std::cv_status status;

    void Add(const Type& data) { return emplace(data); }

    // insert by moving into data element.
    void Add(Type&& data) { return emplace(std::move(data)); }

    status WaitOne(Type& data, unsigned long msec = ULONG_MAX /*INFINITE*/) {
        std::unique_lock<std::mutex> lock(mutex_);

        std::cv_status status(std::cv_status::no_timeout);

        if (datas_.empty()) {
            if (msec == ULONG_MAX) {
                cond_.wait(lock);
            } else {
                status = cond_.wait_for(lock, std::chrono::milliseconds(msec));
            }
        }

        if (status == std::cv_status::no_timeout) {
            assert(!datas_.empty());
            std::swap(data, datas_.front());
            datas_.pop_front();
        }

        return status;
    }

    status WaitAny(std::list<Type>& datas,
                   unsigned long msec = ULONG_MAX /*INFINITE*/) {
        std::unique_lock<std::mutex> lock(mutex_);

        std::cv_status status(std::cv_status::no_timeout);

        if (datas_.empty()) {
            if (msec == ULONG_MAX) {
                cond_.wait(lock);
            } else {
                status = cond_.wait_for(lock, std::chrono::milliseconds(msec));
            }
        }

        if (status == std::cv_status::no_timeout) {
            datas.swap(datas_);
        }

        return status;
    }

    std::size_t Size() {
        std::lock_guard<std::mutex> lock(mutex_);
        return datas_.size();
    }

    std::size_t Capacity() const { return _Cap; }

 private:
    template <typename T>
    void emplace(T&& data) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (datas_.size() >= _Cap) {
            // drop data
            datas_.pop_front();
        }
        datas_.push_back(std::move(data));
        cond_.notify_one();
    }

 private:
    std::list<Type> datas_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

// Buffer class used to storage byte buffer data.
class Buffer {
 public:
    typedef unsigned char byte;
    Buffer() : _data(nullptr), _size(0) {}

    Buffer(const byte* data, size_t size) : _data(nullptr), _size(0) {
        if (!data || !size) {
            return;
        }

        allocBuffer(size);
        memcpy(_data, data, size);
    }

    Buffer(size_t size) : _data(nullptr), _size(0) { allocBuffer(size); }

    Buffer(const Buffer& buffer) : _data(nullptr), _size(0) { *this = buffer; }

    Buffer(Buffer&& buffer) : _data(nullptr), _size(0) { swap(buffer); }

    Buffer(const Buffer* pbuffer) : _data(nullptr), _size(0) {
        if (!pbuffer) {
            return;
        }

        *this = *pbuffer;
    }

    ~Buffer() { releaseBuffer(); }

    Buffer& assign(const byte* data, size_t size) {
        allocBuffer(size);

        if (data && size > 0) {
            memcpy(_data, data, size);
        }

        return *this;
    }

    byte* data() const { return _data; }

    size_t size() const { return _size; }

    size_t length() const { return size(); }

    void resize(size_t size) { allocBuffer(size); }

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

    Buffer& operator=(Buffer&& right) {
        swap(right);

        return *this;
    }

    bool empty() const { return (!_data && _size == 0); }

    void clear() { releaseBuffer(); }

    bool operator==(const Buffer& right) const {
        if (_size == right.size()) {
            return (memcmp(_data, right.data(), _size) == 0);
        }

        return false;
    }

    bool operator!=(const Buffer& right) const { return !(*this == right); }

    void swap(Buffer& right) {
        if (this == &right) {
            return;
        }

        std::swap(this->_data, right._data);
        std::swap(this->_size, right._size);
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
        if (_data) {
            delete _data;
            _data = nullptr;
        }
        _size = 0;
    }

 private:
    byte* _data;
    size_t _size;
};

// Fix byte date cache used std::array
template <std::size_t _Nm>
using FixByteDataCache =
    BasicEventDataCache<std::string, std::array<unsigned char, _Nm>>;

// Byte buffer data cache
using ByteDataCache = BasicEventDataCache<std::string, Buffer>;

// String buffer data cache
using StringDataCache = BasicEventDataCache<std::string, std::string>;
};  // namespace Cache
