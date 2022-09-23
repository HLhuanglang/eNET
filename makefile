##通用的编译器选项
CXX= g++
CXXFLAGS= -std=c++11 -g
LDFLAGES= -lpthread -DDEBUG

SUB_DIRS=easy_net\
			example\
			raw_examples\
			test\

ROOT_DIR=$(shell pwd)
OUTPUT_BIN_DIR=$(ROOT_DIR)/debug/bin
OUTPUT_LIB_PATH=$(ROOT_DIR)/output/lib
OUTPUT_INC_PATH=${ROOT_DIR}/output/include

TARGET_SO=libeasynet.so

$(shell mkdir -p ${OUTPUT_BIN_DIR})
$(shell mkdir -p ${OUTPUT_LIB_PATH})
$(shell mkdir -p ${OUTPUT_INC_PATH})

## 导出给子makefile使用
export ROOT_DIR OUTPUT_BIN_DIR OUTPUT_LIB_PATH OUTPUT_INC_PATH CXX CXXFLAGS LDFLAGES TARGET_SO

.PHONY: default
default: all

##关于socket系列接口使用,以及一些开源的网络库使用案例
.PHONY: raw_examples
raw_examples:
	${MAKE} -C raw_examples

.PHONY: all
all: 
	@for dir in ${SUB_DIRS}; do ${MAKE} -C $${dir}; done
	@echo "\n\n========[make all done, please enjoy easynet]========" 

.PHONY: easy_net
easy_net:
	${MAKE} -C easy_net

.PHONY:example
example:easy_net
	${MAKE} -C example

.PHONY:protocol
protocol: easy_net
	${MAKE} -C protocol

.PHONY: test
test: easy_net
	${MAKE} -C test

.PHONY: clean
clean:
	rm -rf debug
	rm -rf output
	find easy_net -name "*.o"  | xargs rm -f