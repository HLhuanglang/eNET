# 设置最低cmake版本,3.6-Morden CMake
cmake_minimum_required(VERSION 3.6)

# ---------------------------------------------------------------------------------------
# Start EasyNet project
# ---------------------------------------------------------------------------------------
# 项目名称-EasyNet
project(EasyNet VERSION 1.0.0
  DESCRIPTION "A cpp11 network library"
  LANGUAGES CXX C)

# 导入GunDirs
include(GNUInstallDirs)

# ---------------------------------------------------------------------------------------
# Compiler config
# ---------------------------------------------------------------------------------------
# 指定cpp11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED true)

# 生成compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 判断编译类型
if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
  add_definitions("-DEASYNET_DEBUG")
endif()

# ---------------------------------------------------------------------------------------
# config thrid_party
# ---------------------------------------------------------------------------------------
find_package(GTest 1.12.1 EXACT REQUIRED)
if (GTest_FOUND)
  message(STATUS "GTest found")
endif()

find_package(spdlog 1.13.0 EXACT REQUIRED)
if (spdlog_FOUND)
  message(STATUS "spdlog found")
endif()

# ---------------------------------------------------------------------------------------
# EasyNet库编译
# ---------------------------------------------------------------------------------------
set(EASYNET_LIB_NAME "easynet")
option(BUILD_WITH_HTTP "是否编译http协议模块" OFF)
if(BUILD_WITH_HTTP)
    file(GLOB_RECURSE HTTP_SOURCES "protocol/http/*.cpp")
    list(APPEND EASYNET_SOURCES ${HTTP_SOURCES})
endif()
file(GLOB_RECURSE BASE_SOURCES "${CMAKE_SOURCE_DIR}/easy_net/base/*.cpp")
file(GLOB_RECURSE NET_SOURCES "${CMAKE_SOURCE_DIR}/easy_net/net/*.cpp")
list(APPEND EASYNET_SOURCES ${BASE_SOURCES} ${NET_SOURCES})  
add_library(${EASYNET_LIB_NAME} STATIC ${EASYNET_SOURCES})
target_include_directories(${EASYNET_LIB_NAME}
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net/base
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net/net
  PUBLIC $<IF:$<BOOL:${BUILD_WITH_HTTP}>,${CMAKE_SOURCE_DIR}/easy_net/protocol/http,>)
target_link_libraries(${EASYNET_LIB_NAME} spdlog::spdlog)

# ---------------------------------------------------------------------------------------
# EasyNet库单元测试
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE EASYNET_UNITTEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")
add_executable(EasyNetUnitTest ${EASYNET_UNITTEST_SOURCES})
target_include_directories(EasyNetUnitTest 
  PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(EasyNetUnitTest ${EASYNET_LIB_NAME})
target_link_libraries(EasyNetUnitTest GTest::gtest GTest::gtest_main)


# ---------------------------------------------------------------------------------------
# EasyNet使用案例
# ---------------------------------------------------------------------------------------
option(BUILD_WITH_EXAMPLES "是否编译examples" OFF)
if (BUILD_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

# ---------------------------------------------------------------------------------------
# compile raw_examples
# ---------------------------------------------------------------------------------------
option(BUILD_WITH_RAWEXAMPLES "是否编译raw_examples" OFF)
if (BUILD_WITH_RAWEXAMPLES)
  add_subdirectory(raw_examples)
endif()