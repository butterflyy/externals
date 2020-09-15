// utils_signal.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <gtest/gtest.h>
#include <commonex/utils.h>

class MyThread : public utils::thread
{
public:
	void run() {
		_run();
	}

	std::function<void() > _run;
};

TEST(Test, thread) {
	//1
	{
		MyThread thd;
		thd.yield();

		thd._run = [&]() {
			while (!thd.is_quit()) {
				thd.yield();
				//std::cout << ".";
			}
		};

		thd.start();

		ASSERT_TRUE(thd.is_running());

		thd.quit();
		ASSERT_FALSE(thd.is_running());

		thd.start();
		ASSERT_EQ(thd.start(), thd.running);
		ASSERT_TRUE(thd.is_running());

		thd.quit();
		ASSERT_FALSE(thd.is_running());

		thd.quit();
	}

	//2
	std::cout << "defualt ~thread" << std::endl;
	{
		MyThread thd;
		thd.yield();

		thd._run = [&]() {
			thd.yield();
		};
	}

	//3
	std::cout << "defualt ~thread2" << std::endl;
	{
		MyThread thd;
		thd.yield();

		thd._run = [&]() {
		};
		thd.start();
	}
}