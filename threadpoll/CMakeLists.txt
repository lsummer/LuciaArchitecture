# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.8)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 换个地方保存生成的目标二进制文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CIA_OUTPUT_NAME cia_threadpoll)
# 指定生成目标
#add_library(${CIA_OUTPUT_NAME} SHARED ${DIR_SRCS})
add_library(${CIA_OUTPUT_NAME}_static STATIC ${DIR_SRCS})

set_target_properties(${CIA_OUTPUT_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#set_target_properties(${CIA_OUTPUT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(${CIA_OUTPUT_NAME}_static PROPERTIES OUTPUT_NAME  ${CIA_OUTPUT_NAME})
#set_target_properties(${CIA_OUTPUT_NAME} PROPERTIES VERSION 1.0 SOVERSION 1)
