# echo
# - echosvr
add_executable(echosvr echo/echosvr.cpp)
target_include_directories(echosvr PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(echosvr ${EASYNET_LIB_NAME})

# - echocli
add_executable(echocli echo/echocli.cpp)
target_include_directories(echocli PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(echocli ${EASYNET_LIB_NAME})

# timer
add_executable(timer timer/timer.cpp)
target_include_directories(timer PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(timer ${EASYNET_LIB_NAME})

# http
# -httpsvr
add_executable(httpsvr http/httpsvr.cpp)
target_include_directories(httpsvr PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(httpsvr ${EASYNET_LIB_NAME})

# tcp
# - tcpsvr
add_executable(tcpsvr tcp/tcpsvr.cpp)
target_include_directories(tcpsvr PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(tcpsvr ${EASYNET_LIB_NAME})

# - tcpcli
add_executable(tcpcli tcp/tcpcli.cpp)
target_include_directories(tcpcli PUBLIC ${CMAKE_SOURCE_DIR}/easy_net)
target_link_libraries(tcpcli ${EASYNET_LIB_NAME})
