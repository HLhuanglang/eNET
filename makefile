TARGET_SO=libeasynet.so
CXX=g++

SUB_DIRS=example\
			raw_examples\
			test\

ROOT_DIR=$(shell pwd)
OBJ_DIR=$(ROOT_DIR)/debug/obj
BIN_DIR=$(ROOT_DIR)/debug/bin
LIB_PATH=$(ROOT_DIR)/output/lib

export LIB_PATH

.PHONY: default
default: all

.PHONY: echo
echo:
	@echo $(SUB_DIRS)
	@echo $(ROOT_DIR)
	@echo $(OBJ_DIR)
	@echo $(BIN_DIR)

.PHONY: all
all:
	make raw_examples build test
	@echo "\n\n========[make all done, please enjoy easynet]========" 

.PHONY: build
build:
	mkdir -p output/lib output/include
	g++ -g -fPIC -shared ./net/*.cpp ./util/*.cpp -o ${TARGET_SO} -I./util -DDEBUG
	mv -u ${TARGET_SO} output/lib
	cp ./net/*.h output/include
	cp ./util/*.h output/include

.PHONY: test
test:
	make -C test

.PHONY: raw_examples
raw_examples:
	make -C raw_examples
	make -C raw_examples/libuv
	make -C raw_examples/http_parser

.PHONY: clean
clean:
	rm -rf output
	rm -rf bin