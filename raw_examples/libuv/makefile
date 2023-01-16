SRC=$(wildcard *.cpp *.c)

BIN_PATH=${OUTPUT_BIN_DIR}/libuv
TARGET_BIN=libuv_echo

#编译前请先安装libuv,安装目录可自定义。
#该案例默认不编译
LDFLAGES+= -I/opt/libuv/include -L/opt/libuv/lib -luv

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS} ${SRC} -o ${BIN_PATH}/${TARGET_BIN} ${LDFLAGES}