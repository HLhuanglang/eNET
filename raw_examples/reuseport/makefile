SRC=$(wildcard ./*.cpp)

BIN_PATH=${OUTPUT_BIN_DIR}/reuseport
TARGET_BIN=reuseport_demo

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS} ${SRC} -o  ${BIN_PATH}/${TARGET_BIN} ${LDFLAGES}