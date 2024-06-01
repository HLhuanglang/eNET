# async_http
add_executable(async_http async_http/async_http.cpp)
target_link_libraries(async_http pthread)

# cpp-httplib
add_executable(cpp-httplib cpp-httplib/httplib_client_test.cpp)
target_link_libraries(cpp-httplib pthread)

# epoll
add_executable(epoll_cli epoll/epoll_cli.cpp)
target_link_libraries(epoll_cli pthread)
add_executable(epoll_svr epoll/epoll_svr.cpp)
target_link_libraries(epoll_svr pthread)

# eventfd
add_executable(eventfd eventfd/eventfd_demo.cpp)
target_link_libraries(eventfd pthread)

# http_parser
add_executable(http_parser http_parser/http_parser_demo.cpp http_parser/http_parser.c)
target_link_libraries(http_parser pthread)

# noblock_connect
add_executable(noblocking_connect noblocking_connect/noblocking_connect.cpp)
target_link_libraries(noblocking_connect pthread)

# pipe_notifier
add_executable(pipe_notifier pipe_notifier/pipe.cpp pipe_notifier/notifier.cpp)
target_link_libraries(pipe_notifier pthread)

# reuseport
add_executable(reuseport reuseport/server.cpp)
target_link_libraries(reuseport pthread)

# self-pipe-trick
add_executable(self-pipe-trick self-pipe-trick/self-pipe-trick.cpp)
target_link_libraries(self-pipe-trick pthread)

# signalfd
add_executable(signalfd signalfd/signalfd_demo.cpp)
target_link_libraries(signalfd pthread)

# thread_pool
add_executable(thread_pool thread_pool/main.cpp)
target_link_libraries(thread_pool pthread)

# timer
add_executable(timer_demo timer/timer_demo.cpp)
target_link_libraries(timer_demo pthread)

# timerfd
add_executable(timerfd timerfd/timerfd_demo.cpp)
target_link_libraries(timerfd pthread)

# unix_domain
add_executable(unix_domain_cli unix_domain/client.cpp)
target_link_libraries(unix_domain_cli pthread)
add_executable(unix_domain_svr unix_domain/server.cpp)
target_link_libraries(unix_domain_svr pthread)

# c-ares
if(c-ares_FOUND)
    add_executable(c-ares c-ares/main.cpp)
    target_link_libraries(c-ares pthread c-ares::cares_static)
    target_link_directories(c-ares PRIVATE "${c-ares_INCLUDE_DIR}")
endif()