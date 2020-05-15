
# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.12)
project(c++17)

set(CMAKE_CXX_STANDARD 17)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 添加 base 子目录
add_subdirectory(base)

include_directories (${PROJECT_SOURCE_DIR}/base)

# 指定生成目标
add_executable(c++17 ${DIR_SRCS})


# 添加链接库
target_link_libraries(c++17 base)


