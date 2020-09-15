# 公共库

提供一些公共功能的函数，例如线程，事件，图片处理等，所有其他的项目都会应用这个公共库

# 特点

###  head only
    除了小部分windows特有实现
### 跨平台
### C0X支持
	common 在不支持C11的linux平台上面，也可以使用，WINDOWS平台需要C11支持
	commonex 依赖boost，支持C0X

# 说明

主要包括 common 和 commoneex， commonex依赖boost

## common 
	提供基本上能用到的很多基本功能，线程，事件，文件读写，字符串转换，base64，
	ini读写，bmp文件读写，编码转换等。

## commonex
	commonex的重构升级版本，完全的跨平台，支持C0X，依赖boost，更加完善的测试用例，保证稳定性。
	测试用例是在 test 下面，使用gtest。
	
	目前实现了 
	signal.h：使用condition_variable，mutex实现, 但是可以查询等待状态，wait延迟后，set也支持。
	thread.h: 使用thread，condition_variable，mutex实现，方便类直接继承使用，可以查看线程的状态，并且提供退出循环遍历控制。

