# 设置最低cmake版本,3.6-Morden CMake
cmake_minimum_required(VERSION 3.6)

# ---------------------------------------------------------------------------------------
# Start EasyNet project
# ---------------------------------------------------------------------------------------
# 项目名称-EasyNet
project(
  EasyNet
  VERSION 1.0.0
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
if(GTest_FOUND)
  message(STATUS "GTest found")
endif()

find_package(spdlog 1.13.0 EXACT REQUIRED)
if(spdlog_FOUND)
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
target_include_directories(
  ${EASYNET_LIB_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/easy_net/base>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/easy_net/net>
    $<BUILD_INTERFACE:$<IF:$<BOOL:${BUILD_WITH_HTTP}>,${CMAKE_SOURCE_DIR}/easy_net/protocol/http,>>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(${EASYNET_LIB_NAME} spdlog::spdlog)

# ---------------------------------------------------------------------------------------
# EasyNet库安装
# ---------------------------------------------------------------------------------------
# 安装头文件和库 
# --头文件
set(easynet_public_headers
    easy_net/net/tcp_server.h
    easy_net/net/connection_owner.h
    easy_net/net/poll_poller.h
    easy_net/net/tcp_connection.h
    easy_net/net/timer_timewheel.h
    easy_net/net/io_event.h
    easy_net/net/timer.h
    easy_net/net/poller.h
    easy_net/net/notify.h
    easy_net/net/connector.h
    easy_net/net/acceptor.h
    easy_net/net/timer_list.h
    easy_net/net/server_thread.h
    easy_net/net/tcp_client.h
    easy_net/net/timer_miniheap.h
    easy_net/net/event_loop.h
    easy_net/net/def.h
    easy_net/net/epoll_poller.h
    easy_net/base/socket_opt.h
    easy_net/base/buffer.h
    easy_net/base/log.h
    easy_net/base/copyable.h
    easy_net/base/inet_addr.h
    easy_net/base/non_copyable.h
    easy_net/base/thread.h
    easy_net/base/sigleton.h)
install(FILES ${easynet_public_headers}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/easynet")
# 这种模式,需要项目所有的头文件放到include目录下 
# install( DIRECTORY
# ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION
# ${CMAKE_INSTALL_INCLUDEDIR}/easynet )

# --库和可执行文件
install(
  TARGETS easynet
  EXPORT EasyNetTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 动态库安装路径
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # 静态库安装路径
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}) # 可执行文件安装路径

# 安装cmake配置文件
set(project_config_in "${CMAKE_CURRENT_LIST_DIR}/cmake/easynetConfig.cmake.in")
set(project_config_out "${CMAKE_CURRENT_BINARY_DIR}/easynetConfig.cmake")
set(config_targets_file "easynetConfigTargets.cmake")
set(version_config_file "${CMAKE_CURRENT_BINARY_DIR}/easynetConfigVersion.cmake")
set(export_dest_dir "${CMAKE_INSTALL_LIBDIR}/cmake/easynet")

# --根据配置模板，生成并安装包配置文件easynetConfig.cmake
include(CMakePackageConfigHelpers)
configure_package_config_file("${project_config_in}" "${project_config_out}"
                              INSTALL_DESTINATION ${export_dest_dir})
install(FILES "${project_config_out}" DESTINATION ${export_dest_dir}) 

# --生成并安装easynetConfigVersion.cmake
write_basic_package_version_file("${version_config_file}"
                                 COMPATIBILITY SameMajorVersion)
install(FILES "${version_config_file}" DESTINATION ${export_dest_dir})

# --生成并安装easynetConfigTargets.cmake
export(TARGETS easynet
       FILE "${CMAKE_CURRENT_BINARY_DIR}/${config_targets_file}")
install(
  EXPORT EasyNetTargets
  FILE ${config_targets_file}
  DESTINATION ${export_dest_dir})

# ---------------------------------------------------------------------------------------
# EasyNet库单元测试
# ---------------------------------------------------------------------------------------
file(GLOB_RECURSE EASYNET_UNITTEST_SOURCES "${CMAKE_SOURCE_DIR}/test/*.cpp")
add_executable(EasyNetUnitTest ${EASYNET_UNITTEST_SOURCES})
target_include_directories(EasyNetUnitTest PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(EasyNetUnitTest ${EASYNET_LIB_NAME})
target_link_libraries(EasyNetUnitTest GTest::gtest GTest::gtest_main)

# ---------------------------------------------------------------------------------------
# EasyNet使用案例
# ---------------------------------------------------------------------------------------
option(BUILD_WITH_EXAMPLES "是否编译examples" OFF)
if(BUILD_WITH_EXAMPLES)
  add_subdirectory(examples)
endif()

# ---------------------------------------------------------------------------------------
# compile raw_examples
# ---------------------------------------------------------------------------------------
option(BUILD_WITH_RAWEXAMPLES "是否编译raw_examples" OFF)
if(BUILD_WITH_RAWEXAMPLES)
  add_subdirectory(raw_examples)
endif()
