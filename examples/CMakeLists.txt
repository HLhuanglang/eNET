# echosvr
add_executable(echosvr echo/echosvr.cpp)
target_include_directories(echosvr PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(echosvr ${EASYNET_LIB_NAME})

# timer
add_executable(timer timer/timer.cpp)
target_include_directories(timer PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(timer ${EASYNET_LIB_NAME})