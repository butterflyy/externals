// utils_signal.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <gtest/gtest.h>
#include <commonex/utils.h>
#include <commonex/signal.h>

TEST(Test, signal) {
	utils::signal sgl;
	
	//1
	std::cout << "test 1" << std::endl;
	{
		std::thread thd([&]() {
			ASSERT_EQ(sgl.wait(100), utils::signal::status::timeout);
		});

		utils::thread::msleep(1);
		ASSERT_TRUE(sgl.is_wait());
		utils::thread::msleep(110);
		ASSERT_FALSE(sgl.is_wait());
		thd.join();
	}


	//2
	std::cout << "test 2" << std::endl;
	{
		std::thread thd([&]() {
			utils::thread::msleep(1);//set first
			ASSERT_EQ(sgl.wait(), utils::signal::status::no_timeout);
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
		});

		sgl.set();
		thd.join();
	}

	//3
	std::cout << "test 3" << std::endl;
	{
		std::thread thd([&]() {
			ASSERT_EQ(sgl.wait(), utils::signal::status::no_timeout);
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
		});

		utils::thread::msleep(10);//wait first
		sgl.set();
		thd.join();
	}


	//4
	std::cout << "test 4" << std::endl;
	{
		sgl.set();
		std::thread thd([&]() {
			ASSERT_EQ(sgl.wait(), utils::signal::status::no_timeout);
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
		});
		thd.join();
	}



	//5
	std::cout << "test 5" << std::endl;
	{
		sgl.set();
		sgl.reset();
		std::thread thd([&]() {
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
		});
		thd.join();
	}


	//6
	std::cout << "test 6" << std::endl;
	{
		std::thread thd1([&]() {
			ASSERT_EQ(sgl.wait(2000), utils::signal::status::no_timeout);
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
		});

		std::thread thd2([&]() {

			ASSERT_EQ(sgl.wait(2000), utils::signal::status::no_timeout);
			ASSERT_EQ(sgl.wait(1), utils::signal::status::timeout);
		});

		utils::thread::msleep(100);
		sgl.set();

		thd1.join();
		thd2.join();
	}
}