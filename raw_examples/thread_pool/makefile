SRC=$(wildcard ./*.cpp)

BIN_PATH=${OUTPUT_BIN_DIR}/thread_pool
TARGET_BIN=thread_pool_demo

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS} ${SRC} -o  ${BIN_PATH}/${TARGET_BIN} ${LDFLAGES}