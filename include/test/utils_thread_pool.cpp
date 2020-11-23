// utils_signal.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <gtest/gtest.h>
#include <commonex/thread_pool.h>

TEST(Test, pool) {
	utils::thread_pool pool(8);
	
	
	{
		int tasksize = 1000;
		std::vector<std::future<int>> results;
		for (int i = 0; i < tasksize; i++) {
			results.emplace_back(pool.enqueue([i] {
				std::cout << "run " << i << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				return i;
			}));
		}

		for (int i = 0; i < tasksize; i++) {
			ASSERT_EQ(i, results[i].get());
			std::cout << "get over " << i << std::endl;
		}
	}


	{
		int tasksize = 100;
		std::vector<std::future<int>> results;
		for (int i = 0; i < tasksize; i++) {
			results.emplace_back(pool.enqueue([i] {
				std::cout << "run " << i << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				return i;
			}));
		}

		for (int i = 0; i < tasksize; i++) {
			ASSERT_EQ(i, results[i].get());
			std::cout << "get over " << i << std::endl;
		}
	}

}