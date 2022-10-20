/* ====================================================================
 * Copyright (c) 2021 qingteng.  All rights reserved.
 *
 * Author: yonggang.liu@qingteng.cn
 *
 * ====================================================================
 */

#include <cache/eventdatacache.h>
#include <common/utils.h>
#include <gtest/gtest.h>

namespace Cache {
using EventDataCache =
    Cache::BasicEventDataCache<std::string,
                               std::pair<std::string, std::string>>;
// not need event notify, item is <table_name, table_data>
using NormalDataCache =
    Cache::BasicEventDataCache<std::string, std::string, false>;

class EventDataCacheTests : public testing::Test {};

TEST_F(EventDataCacheTests, AddGet) {
    EventDataCache cache;

    std::string data("{\"pid\":\"1111\", \"uid\":\"44444\"}");
    cache.Add("1", std::make_pair<>("process_events", data), data.size());
    ASSERT_FALSE(data.empty());
    cache.Add("2", std::make_pair<>("rpm", "{}"), data.size());

    {
        std::pair<std::string, std::string> outdata;
        bool has = cache.Get("1", outdata);
        ASSERT_TRUE(has);
        ASSERT_EQ(std::get<0>(outdata), "process_events");
        ASSERT_EQ(std::get<1>(outdata), data);
    }

    // get again
    {
        std::pair<std::string, std::string> outdata;
        bool has = cache.Get("1", outdata);
        ASSERT_TRUE(has);
        ASSERT_EQ(std::get<0>(outdata), "process_events");
        ASSERT_EQ(std::get<1>(outdata), data);
    }

    {
        std::pair<std::string, std::string> outdata;
        bool has = cache.Get("3", outdata);
        ASSERT_FALSE(has);
        ASSERT_TRUE(std::get<0>(outdata).empty());
        ASSERT_TRUE(std::get<1>(outdata).empty());
    }

    cache.Add("5", std::make_pair<>("process_events2", std::move(data)),
              data.size());
    ASSERT_TRUE(data.empty());
    {
        std::pair<std::string, std::string> outdata;
        bool has = cache.Get("5", outdata);
        ASSERT_TRUE(has);
        ASSERT_EQ(std::get<0>(outdata), "process_events2");
        ASSERT_EQ(std::get<1>(outdata),
                  "{\"pid\":\"1111\", \"uid\":\"44444\"}");
    }

    // add agent
    {
        std::string data("new data");
        cache.Add("5", std::make_pair<>("process_events2", data), data.size());
        std::pair<std::string, std::string> outdata;
        bool has = cache.Get("5", outdata);
        ASSERT_TRUE(has);
        ASSERT_EQ(std::get<0>(outdata), "process_events2");
        ASSERT_EQ(std::get<1>(outdata), data);
    }
}

TEST_F(EventDataCacheTests, GetOriginData) {
    StringDataCache cache;

    std::string str1("test string data");
    const char* addr = str1.c_str();
    cache.Add("1", std::move(str1), str1.capacity());

    // 2 thread get origin data
    std::thread thd1([&]() {
        StringDataCache::CacheDataPtr data;
        bool has = cache.Get("1", data);
        ASSERT_TRUE(has);

        // waiting data clear
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // check is origin str1 data
        ASSERT_TRUE(cache.Empty());
        ASSERT_EQ(data->eventData.c_str(), addr);
    });

    std::thread thd2([&]() {
        StringDataCache::CacheDataPtr data;
        bool has = cache.Get("1", data);
        ASSERT_TRUE(has);

        // waiting data clear
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        // check is origin str1 data
        ASSERT_TRUE(cache.Empty());
        ASSERT_EQ(data->eventData.c_str(), addr);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cache.Clear();
    thd1.join();
    thd2.join();
}

TEST_F(EventDataCacheTests, RemoveClear) {
    NormalDataCache cache;

    std::string data("test data");

    cache.Add("1", data, data.size());
    cache.Add("2", data, data.size());
    cache.Add("3", data, data.size());
    cache.Add("4", data, data.size());

    cache.Remove("3");
    ASSERT_FALSE(cache.Has("3"));
    ASSERT_TRUE(cache.Has("4"));

    cache.Clear();
    ASSERT_FALSE(cache.Has("1"));
    ASSERT_FALSE(cache.Has("2"));
    ASSERT_FALSE(cache.Has("4"));
}

TEST_F(EventDataCacheTests, EventException) {
    NormalDataCache cache;

    std::list<std::pair<EventDataCache::EventID, clock_t>> ids;
    ASSERT_THROW(cache.WaitAll(ids, 10), std::logic_error);
}

TEST_F(EventDataCacheTests, QueueData) {
    StringDataCache cache;

    std::string str("test string data");

    {
        cache.Add("1", str, str.capacity());
        cache.Add("2", str, str.capacity());
        auto queckCheck = cache._queueCheck;

        ASSERT_EQ(queckCheck.size(), 2);
        std::string q1 = queckCheck.front().first;
        queckCheck.pop_front();

        std::string q2 = queckCheck.front().first;
        ASSERT_EQ(q1, "2");
        ASSERT_EQ(q2, "1");

        cache.Clear();
    }

    {
        cache.Add("1", str, str.capacity(), 310);
        cache.Add("2", str, str.capacity(), 300);
        auto queckCheck = cache._queueCheck;

        ASSERT_EQ(queckCheck.size(), 2);
        std::string q1 = queckCheck.front().first;
        queckCheck.pop_front();

        std::string q2 = queckCheck.front().first;
        ASSERT_EQ(q1, "1");
        ASSERT_EQ(q2, "2");

        cache.Clear();
    }
}

TEST_F(EventDataCacheTests, Caption) {
    EventDataCache cache(100);

    std::string data;
    data.assign(60, 'a');

    cache.Add("1", std::make_pair<>("process_events", data), data.size());
    cache.Add("2", std::make_pair<>("process_events2", data), data.size());
    cache.Add("3", std::make_pair<>("process_events2", data), data.size());
    cache.Add("4", std::make_pair<>("process_events2", data), data.size());

    ASSERT_FALSE(cache.Has("1"));
    ASSERT_FALSE(cache.Has("2"));
    ASSERT_TRUE(cache.Has("3"));
    ASSERT_TRUE(cache.Has("4"));
}

TEST_F(EventDataCacheTests, Expiry) {
    EventDataCache cache(EventDataCache::CAPTION_SIZE, 3);  // 3 second

    std::string data("test data");

    cache.Add("0", std::make_pair<>("process_events", data), data.size(), 10);

    cache.Add("1", std::make_pair<>("process_events", data), data.size());
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cache.Add("1", std::make_pair<>("process_events", data), data.size());
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cache.Add("2", std::make_pair<>("process_events2", data), data.size());
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cache.Add("3", std::make_pair<>("process_events2", data), data.size());
    std::this_thread::sleep_for(std::chrono::seconds(2));

    cache.Add("4", std::make_pair<>("process_events2", data), data.size());

    ASSERT_TRUE(cache.Has("0"));
    ASSERT_FALSE(cache.Has("1"));
    ASSERT_FALSE(cache.Has("2"));
    ASSERT_TRUE(cache.Has("3"));
    ASSERT_TRUE(cache.Has("4"));
}

TEST_F(EventDataCacheTests, WaitAll) {
    EventDataCache cache;

    std::thread thd([&]() {
        std::list<std::pair<EventDataCache::EventID, clock_t>> ids;
        EventDataCache::status status = cache.WaitAll(ids, 10);
        ASSERT_EQ(status, EventDataCache::status::timeout);

        status = cache.WaitAll(ids, 100);
        ASSERT_EQ(status, EventDataCache::status::no_timeout);
        ASSERT_EQ(ids.size(), 1);
        ASSERT_EQ(ids.front().first, "1");
        ASSERT_NE(ids.front().second, 0);
        ids.pop_front();

        status = cache.WaitAll(ids, 200);
        ASSERT_EQ(status, EventDataCache::status::no_timeout);
        ASSERT_EQ(ids.size(), 1);
        ASSERT_EQ(ids.front().first, "2");
        ASSERT_NE(ids.front().second, 0);
        ids.pop_front();
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::string data("test json data string");

    cache.Add("1", std::make_pair<>("process_events", data), data.size());

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    cache.Add("2", std::make_pair<>("process_events2", data), data.size());

    ASSERT_TRUE(cache.Has("1"));
    ASSERT_TRUE(cache.Has("2"));

    thd.join();

    std::list<std::pair<EventDataCache::EventID, clock_t>> ids;
    EventDataCache::status status = cache.WaitAll(ids, 10);
    ASSERT_EQ(status, EventDataCache::status::timeout);

    cache.Add("5", std::make_pair<>("process_events2", data), data.size());
    status = cache.WaitAll(ids, 10);
    ASSERT_EQ(status, EventDataCache::status::no_timeout);
    ASSERT_EQ(ids.size(), 1);
}

class QueueDataCacheTests : public testing::Test {};

TEST_F(QueueDataCacheTests, AddSync) {
    using StringQueue = QueueDataCache<std::string>;
    StringQueue cache;

    std::string str("test1");
    cache.Add(str);

    std::string data;
    StringQueue::status status = cache.WaitOne(data, 100);
    ASSERT_EQ(status, StringQueue::status::no_timeout);
    ASSERT_EQ(data, str);
}

TEST_F(QueueDataCacheTests, AddAsync1) {
    using StringQueue = QueueDataCache<std::string>;
    StringQueue cache;

    std::string str("test1");

    std::thread thd([&]() {
        std::string data;
        StringQueue::status status = cache.WaitOne(data);
        ASSERT_EQ(status, StringQueue::status::no_timeout);
        ASSERT_EQ(data, str);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.Add(str);
    thd.join();
}

TEST_F(QueueDataCacheTests, AddAsync2) {
    using StringQueue = QueueDataCache<std::string>;
    StringQueue cache;

    std::string str("test1");

    std::thread thd([&]() {
        std::string data;
        StringQueue::status status = cache.WaitOne(data, 5);
        ASSERT_EQ(status, StringQueue::status::timeout);

        status = cache.WaitOne(data, 20);
        ASSERT_EQ(status, StringQueue::status::no_timeout);

        ASSERT_EQ(data, str);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.Add(str);
    thd.join();
}

TEST_F(QueueDataCacheTests, AddAsync3) {
    using StringQueue = QueueDataCache<std::string>;
    StringQueue cache;

    const int kSize = 100;
    std::vector<std::string> strs;
    for (int i = 0; i < kSize; i++) {
        strs.emplace_back("teststring" + std::to_string(i));
    }

    std::thread thd([&]() {
        std::vector<std::string> strdatas;
        while (true) {
            std::string data;
            StringQueue::status status = cache.WaitOne(data, 100);
            if (status == StringQueue::status::timeout) break;
            strdatas.emplace_back(std::move(data));
        }

        // compare data
        ASSERT_EQ(strs.size(), strdatas.size());
        for (size_t i = 0; i < strs.size(); i++) {
            ASSERT_EQ(strs[i], strdatas[i]);
        }
    });

    for (const auto& one : strs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        cache.Add(one);
    }

    thd.join();
}

TEST_F(QueueDataCacheTests, Move) {
    using StringQueue = QueueDataCache<std::string>;
    StringQueue cache;

    std::string str("discrepancies between Markdown processors");
    std::string strbk = str;

    const char* addr = str.c_str();

    std::thread thd([&]() {
        std::string data;
        StringQueue::status status = cache.WaitOne(data);
        ASSERT_EQ(status, StringQueue::status::no_timeout);
        ASSERT_EQ(data, strbk);
        ASSERT_EQ(data.c_str(), addr);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.Add(std::move(str));
    ASSERT_TRUE(str.empty());
    thd.join();
}

class FixQueueDataCacheTests : public testing::Test {};

TEST_F(FixQueueDataCacheTests, AddSync) {
    using StringQueue = FixQueueDataCache<std::string, 10>;
    StringQueue cache;

    std::string str("test1");
    cache.Add(str);

    std::string data;
    StringQueue::status status = cache.WaitOne(data, 100);
    ASSERT_EQ(status, StringQueue::status::no_timeout);
    ASSERT_EQ(data, str);
}

TEST_F(FixQueueDataCacheTests, AddAsync1) {
    using StringQueue = FixQueueDataCache<std::string, 10>;
    StringQueue cache;

    std::string str("test1");

    std::thread thd([&]() {
        std::string data;
        StringQueue::status status = cache.WaitOne(data);
        ASSERT_EQ(status, StringQueue::status::no_timeout);
        ASSERT_EQ(data, str);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.Add(str);
    thd.join();
}

TEST_F(FixQueueDataCacheTests, AddAsync2) {
    using StringQueue = FixQueueDataCache<std::string, 10>;
    StringQueue cache;

    std::string str("test1");

    std::thread thd([&]() {
        std::string data;
        StringQueue::status status = cache.WaitOne(data, 5);
        ASSERT_EQ(status, StringQueue::status::timeout);

        status = cache.WaitOne(data, 10);
        ASSERT_EQ(status, StringQueue::status::no_timeout);

        ASSERT_EQ(data, str);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.Add(str);
    thd.join();
}

TEST_F(FixQueueDataCacheTests, AddAsync3) {
    using StringQueue = FixQueueDataCache<std::string, 10>;
    StringQueue cache;

    const int kSize = 100;
    std::vector<std::string> strs;
    for (int i = 0; i < kSize; i++) {
        strs.emplace_back("teststring" + std::to_string(i));
    }

    std::thread thd([&]() {
        std::vector<std::string> strdatas;
        while (true) {
            std::string data;
            StringQueue::status status = cache.WaitOne(data, 100);
            if (status == StringQueue::status::timeout) break;
            strdatas.emplace_back(std::move(data));
        }

        // compare data
        ASSERT_EQ(strs.size(), strdatas.size());
        for (size_t i = 0; i < strs.size(); i++) {
            ASSERT_EQ(strs[i], strdatas[i]);
        }
    });

    for (const auto& one : strs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        cache.Add(one);
    }

    thd.join();
}

TEST_F(FixQueueDataCacheTests, FixData) {
    using StringQueue = FixQueueDataCache<std::string, 3>;
    StringQueue cache;

    const int kSize = 5;
    std::vector<std::string> strs;
    for (int i = 1; i <= kSize; i++) {
        strs.emplace_back("teststring" + std::to_string(i));
    }

    for (const auto& one : strs) {
        cache.Add(one);
    }

    std::string data;
    StringQueue::status status = cache.WaitOne(data, 100);
    ASSERT_EQ(status, StringQueue::status::no_timeout);
    ASSERT_EQ(data, "teststring3");

    status = cache.WaitOne(data, 100);
    ASSERT_EQ(status, StringQueue::status::no_timeout);
    ASSERT_EQ(data, "teststring4");

    status = cache.WaitOne(data, 100);
    ASSERT_EQ(status, StringQueue::status::no_timeout);
    ASSERT_EQ(data, "teststring5");

    status = cache.WaitOne(data, 100);
    ASSERT_EQ(status, StringQueue::status::timeout);

    // wait any
    for (const auto& one : strs) {
        cache.Add(one);
    }
    std::list<std::string> datas;
    status = cache.WaitAny(datas, 100);
    ASSERT_EQ(status, StringQueue::status::no_timeout);
    ASSERT_EQ(datas.size(), 3);

    ASSERT_EQ("teststring3", datas.front());
    datas.pop_front();
    ASSERT_EQ("teststring4", datas.front());
    datas.pop_front();
    ASSERT_EQ("teststring5", datas.front());
    datas.pop_front();
    ASSERT_TRUE(datas.empty());
}

TEST_F(FixQueueDataCacheTests, FixData2) {
    using StringQueue = FixQueueDataCache<size_t, 8192>;
    StringQueue cache;

    const size_t kSize = 10000;
    std::vector<size_t> sizes;
    for (size_t i = 1; i <= kSize; i++) {
        sizes.emplace_back(i);
    }

    for (const auto& one : sizes) {
        cache.Add(one);
    }

    std::vector<size_t> sizesget;

    std::thread thd([&]() {
        while (true) {
            size_t data;
            StringQueue::status status = cache.WaitOne(data, 100);
            if (status == StringQueue::status::timeout) {
                if (sizesget.size() >= 300)
                    break;
                else
                    ASSERT_TRUE(false);
            } else {
                sizesget.emplace_back(data);
            }
        }
    });

    thd.join();

    ASSERT_EQ(sizesget.size(), cache.Capacity());

    ASSERT_EQ(sizesget[sizesget.size() - 1], 10000);

    size_t last = sizesget[0];
    for (size_t i = 1; i < sizesget.size(); i++) {
        ASSERT_GT(sizesget[i], last);
        last = sizesget[i];
    }
}

TEST_F(FixQueueDataCacheTests, FixData3) {
    using StringQueue = FixQueueDataCache<size_t, 8192>;
    StringQueue cache;

    const size_t kSize = 10000;
    std::vector<size_t> sizes;
    for (size_t i = 1; i <= kSize; i++) {
        sizes.emplace_back(i);
    }

    for (const auto& one : sizes) {
        cache.Add(one);
    }

    std::vector<size_t> sizesget;

    std::thread thd([&]() {
        while (true) {
            std::list<size_t> datas;
            StringQueue::status status = cache.WaitAny(datas, 100);
            if (status == StringQueue::status::timeout) {
                if (sizesget.size() >= 300)
                    break;
                else
                    ASSERT_TRUE(false);
            } else {
                while (!datas.empty()) {
                    sizesget.emplace_back(datas.front());
                    datas.pop_front();
                }
            }
        }
    });

    thd.join();

    ASSERT_EQ(sizesget.size(), cache.Capacity());

    ASSERT_EQ(sizesget[sizesget.size() - 1], 10000);

    size_t last = sizesget[0];
    for (size_t i = 1; i < sizesget.size(); i++) {
        ASSERT_GT(sizesget[i], last);
        last = sizesget[i];
    }
}

TEST_F(FixQueueDataCacheTests, Move) {
    using StringQueue = FixQueueDataCache<std::string, 10>;
    StringQueue cache;

    std::string str("discrepancies between Markdown processors");
    std::string strbk = str;

    const char* addr = str.c_str();

    std::thread thd([&]() {
        std::string data;
        StringQueue::status status = cache.WaitOne(data);
        ASSERT_EQ(status, StringQueue::status::no_timeout);
        ASSERT_EQ(data, strbk);
        ASSERT_EQ(data.c_str(), addr);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    cache.Add(std::move(str));
    ASSERT_TRUE(str.empty());
    thd.join();
}

TEST_F(FixQueueDataCacheTests, MultiThread) {
    using StringQueue = FixQueueDataCache<std::string, 10000>;
    StringQueue cache;

    std::atomic_bool quit(false);

    int datacount(0);
    std::vector<std::thread> thds;

    for (int i = 0; i < 4; i++) {
        thds.emplace_back([&]() {
            for (int i = 0; i < 5000; i++) {
                cache.Add("test data");

                {
                    std::string data;
                    StringQueue::status status = cache.WaitOne(data, 100);
                    if (status == StringQueue::status::no_timeout) {
                        datacount++;
                    }
                }

                {
                    std::string data;
                    StringQueue::status status = cache.WaitOne(data, 100);
                    if (status == StringQueue::status::no_timeout) {
                        datacount++;
                    }
                }
            }
        });
    }

    for (auto& one : thds) {
        one.join();
    }
}

}  // namespace Cache
