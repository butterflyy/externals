﻿// utils_signal.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <gtest/gtest.h>
#include <commonex/utils.h> //for msleep
#include <commonex/convert_sync.h>



TEST(Test, sync1) {
	utils::convert_sync<std::string, std::string> sync;

	std::string recv;
	//1
	{
		ASSERT_EQ(sync.no_id, sync.wait_once("1", &recv, 100));

		ASSERT_TRUE(sync.add("1"));
		ASSERT_TRUE(sync.add("2"));

		ASSERT_FALSE(sync.add("2"));

		ASSERT_EQ(sync.timeout, sync.wait_once("1", &recv, 100));

		ASSERT_TRUE(sync.add("1"));
		std::thread thd([&]() {
			ASSERT_FALSE(sync.set("3", "test data"));
			ASSERT_TRUE(sync.set("1", "test data"));
		});

		//wait_once thread run
		utils::thread::msleep(100);

		ASSERT_EQ(sync.no_timeout, sync.wait_once("1", &recv, 100));
		ASSERT_EQ(recv, "test data");

		ASSERT_EQ(sync.no_id, sync.wait_once("1", &recv, 100));

		ASSERT_EQ(sync.no_id, sync.wait_once("3", &recv, 100));
		thd.join();
	}

	{
		std::thread thd([&]() {
			ASSERT_FALSE(sync.set("3", "test data1"));
			ASSERT_FALSE(sync.set("1", "test data1"));
		});

		//wait_once thread run
		utils::thread::msleep(100);

		ASSERT_EQ(sync.no_id, sync.wait_once("1", &recv, 100));
		ASSERT_EQ(sync.no_id, sync.wait_once("3", &recv, 100));
		thd.join();
	}


	{
		ASSERT_TRUE(sync.add("3"));
		std::thread thd([&]() {
			//wait_once thread run
			utils::thread::msleep(100);
			ASSERT_TRUE(sync.set("3", "test data2"));
			ASSERT_FALSE(sync.set("1", "test data2"));
		});


		ASSERT_EQ(sync.no_id, sync.wait_once("1", &recv, 1));
		ASSERT_EQ(sync.no_timeout, sync.wait_once("3", &recv, 200));
		ASSERT_EQ(recv, "test data2");
		thd.join();
	}

	{
		ASSERT_TRUE(sync.add("3"));
		std::thread thd([&]() {
			//wait_once thread run
			utils::thread::msleep(300);
			ASSERT_FALSE(sync.set("3", "test data2"));
			ASSERT_FALSE(sync.set("1", "test data2"));
		});


		ASSERT_EQ(sync.no_id, sync.wait_once("1", &recv, 1));
		ASSERT_EQ(sync.timeout, sync.wait_once("3", &recv, 200));
		thd.join();
	}
}