# 设置最低cmake版本,3.6-Morden CMake
cmake_minimum_required(VERSION 3.6)

# 项目名称-EasyNet
project(EasyNet VERSION 1.0.0
  DESCRIPTION "A cpp11 network library"
  LANGUAGES CXX C)

# 指定cpp11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# 生成compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 导入GunDirs
include(GNUInstallDirs)

# 编译gtest库
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/gtest)
enable_testing()

# 编译spdlog库
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/spdlog)

# EasyNet库编译
set(EASYNET_LIB_NAME "easynet")
file(GLOB_RECURSE EASYNET_SOURCES "${CMAKE_SOURCE_DIR}/easy_net/*.cpp")
add_library(${EASYNET_LIB_NAME} STATIC ${EASYNET_SOURCES})
target_include_directories(${EASYNET_LIB_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net/base
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net/net
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net/protocol
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net/util)
target_link_libraries(${EASYNET_LIB_NAME} PRIVATE spdlog::spdlog)

# raw_examples编译
add_subdirectory(raw_examples)