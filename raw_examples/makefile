SUB_DIRS=http_parser\
			cpp-httplib\
			epoll\
			eventfd\
			timerfd \
			async_http
			
CXX= g++
CXXFLAGS= -std=c++11 -g -DDEBUG
LDFLAGES= -lpthread


export OUTPUT_BIN_DIR CXX CXXFLAGS LDFLAGES

.PHONY:default
default:build

.PHONY:build
build:
	@for dir in ${SUB_DIRS};do ${MAKE} -C $${dir}; done