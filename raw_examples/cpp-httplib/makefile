SRC=$(wildcard ./*cpp ./*c)

BIN_PATH=${OUTPUT_BIN_DIR}/cpp-httplib
TARGET_BIN=httplib_client

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS}  ${SRC} -o ${BIN_PATH}/${TARGET_BIN} ${LDFLAGES}