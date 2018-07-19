# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)
project (qbox10_cmr)

#告知当前使用的是交叉编译方式，必须配置
SET(CMAKE_SYSTEM_NAME Linux)
#指定C交叉编译器,必须配置
#或交叉编译器使用绝对地址
SET(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
#指定C++交叉编译器
SET(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")

#不一定需要设置
#指定交叉编译环境安装目录...
SET(CMAKE_FIND_ROOT_PATH "...")
#从来不在指定目录下查找工具程序
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#只在指定目录下查找库文件
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#只在指定目录下查找头文件
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#set(ROOT_DIR $(shell pwd)) CMAKE_CURRENT_SOURCE_DIR
include_directories("${CMAKE_SOURCE_DIR}/application/ui_bl/inc"
	"${CMAKE_SOURCE_DIR}/application/business_layer/inc"
	"${CMAKE_SOURCE_DIR}/application/persist_layer/inc"
	"${CMAKE_SOURCE_DIR}/application/ui_layer/inc"
	"${CMAKE_SOURCE_DIR}/lib/nmea/include"
	"${CMAKE_SOURCE_DIR}/lib/openssl/include"
	"${CMAKE_SOURCE_DIR}/lib/reg/include"
	"${CMAKE_SOURCE_DIR}/lib/iconv/include"
	"${CMAKE_SOURCE_DIR}/lib/alsa/include"
	"${CMAKE_SOURCE_DIR}/lib/bluez/include"
	"${CMAKE_SOURCE_DIR}/application/ui_bl/inc/websocket")

link_directories("${CMAKE_SOURCE_DIR}/application/ui_bl/src" 
	"${CMAKE_SOURCE_DIR}/application/business_layer/src"
	"${CMAKE_SOURCE_DIR}/application/persist_layer/src" 
	"${CMAKE_SOURCE_DIR}/application/ui_layer/src" 
	"${CMAKE_SOURCE_DIR}/lib/nmea/lib" 
	"${CMAKE_SOURCE_DIR}/lib/openssl/lib" 
	"${CMAKE_SOURCE_DIR}/lib/reg/lib" 
	"${CMAKE_SOURCE_DIR}/lib/iconv/lib"
	"${CMAKE_SOURCE_DIR}/lib/alsa/lib"
	"${CMAKE_SOURCE_DIR}/application/ui_bl/src/websocket")
ADD_DEFINITIONS(
 -O3 -g -W -Wall
 -Wunused-variable -Wunused-parameter -Wunused-function -Wunused
 -Wno-deprecated -Woverloaded-virtual -Wwrite-strings
 -D__WUR= -D_REENTRANT -D_FILE_OFFSET_BITS=64 -DTIXML_USE_STL
)

# 添加 math 子目录
add_subdirectory(application)
#add_subdirectory(bin)
# 指定生成目标
add_executable(qbox10_cmr ./application/main/src/qbox10_cmr_main.c)

target_link_libraries(qbox10_cmr business ui persist ui_bl login_webs_a)#链接库有先后顺序问题
target_link_libraries(qbox10_cmr pthread m rt nmea ssl crypto dl reg iconv asound)

install (TARGETS qbox10_cmr 
		RUNTIME DESTINATION ${CMAKE_SOURCE_DIR}/install)
#install (TARGETS business ARCHIVE DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/bluetooth DESTINATION ${CMAKE_SOURCE_DIR}/install)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_SOURCE_DIR}/install)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/lib/openssl/lib/ DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/lib/iconv/lib/ DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/lib/nmea/lib/ DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)
install (DIRECTORY ${CMAKE_SOURCE_DIR}/lib/reg/lib/ DESTINATION ${CMAKE_SOURCE_DIR}/install/lib)
