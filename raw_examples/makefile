SUB_DIRS=http_parser\
			epoll\
			eventfd\
			timerfd \
			async_http \
			timer\
			reuseport\
			noblocking_connect\
			pipe_notifier\
			thread_pool\
			unix_domain\
			signalfd\
			self-pipe-trick


#for debug
#SUB_DIRS = timer

CXX= g++
CXXFLAGS= -std=c++11 -g -DDEBUG
LDFLAGES= -lpthread


export OUTPUT_BIN_DIR CXX CXXFLAGS LDFLAGES

.PHONY:default
default:build

.PHONY:build
build:
	@for dir in ${SUB_DIRS};do ${MAKE} -C $${dir}; done