# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.8)

# 项目信息
project (lucia)

# 指定std=11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 添加子目录
INCLUDE_DIRECTORIES( headers tools net  logs signal threadpoll datapoll operation http-parser-master)

# TARGET_LINK_LIBRARIES()
subdirs(tools net logs signal threadpoll  proc datapoll operation http-parser-master app)