# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.8)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)
aux_source_directory(${PROJECT_BINARY_DIR}/lib LIB_SRCS)

# 换个地方保存生成的目标二进制文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

LINK_DIRECTORIES("${PROJECT_BINARY_DIR}/lib")

# 指定生成目标
add_executable(lucia ${DIR_SRCS})

# message(STATUS "${PROJECT_BINARY_DIR}/lib")
TARGET_LINK_LIBRARIES(lucia log4cpp cia_datapoll cia_net cia_parser cia_signal cia_tools cia_log cia_operation cia_proc cia_threadpoll)
