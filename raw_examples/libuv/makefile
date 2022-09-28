SRC=$(wildcard *.cpp *.c)

BIN_PATH=${OUTPUT_BIN_DIR}/libuv
TARGET_BIN=libuv_echo

LDFLAGES+= -I/opt/libuv/include -L/opt/libuv/lib -luv

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS} ${SRC} -o ${BIN_PATH}/${TARGET_BIN} ${LDFLAGES}