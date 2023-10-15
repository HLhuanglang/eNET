BIN_PATH=${OUTPUT_BIN_DIR}/epoll
TARGET_BIN=epoll_svr
TARGET_CLI=epoll_cli

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS} epoll_svr.cpp -o  ${BIN_PATH}/${TARGET_BIN} ${LDFLAGES}
	${CXX} ${CXXFLAGS} epoll_cli.cpp -o  ${BIN_PATH}/${TARGET_CLI} ${LDFLAGES}