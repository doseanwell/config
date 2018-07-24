# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
project (test_option)

#告知当前使用的是交叉编译方式，必须配置
#SET(CMAKE_SYSTEM_NAME Linux)
#指定C交叉编译器,必须配置
#或交叉编译器使用绝对地址
#SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
#指定C++交叉编译器
#SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")

#不一定需要设置
#指定交叉编译环境安装目录...
SET(CMAKE_FIND_ROOT_PATH "...")
#从来不在指定目录下查找工具程序
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#只在指定目录下查找库文件
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#只在指定目录下查找头文件
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

include_directories("${CMAKE_SOURCE_DIR}/../src")

link_directories("${CMAKE_SOURCE_DIR}/../src")
ADD_DEFINITIONS(
 -O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)

SET(PROJECT_SRCS 
	./test.c 
	../src/option.c)
# 指定生成目标
add_executable(test_option ${PROJECT_SRCS})

#target_link_libraries(test_option option)#链接库有先后顺序问题
