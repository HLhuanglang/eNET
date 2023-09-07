BIN_PATH=${OUTPUT_BIN_DIR}/unix_domain

TARGET_SVR=unix_domain_svr
TARGET_CLI=unix_domain_cli

.PHONY:default
default:build

.PHONY:build
build:
	@mkdir -p ${BIN_PATH}
	${CXX} ${CXXFLAGS} server.cpp -o  ${BIN_PATH}/${TARGET_SVR} ${LDFLAGES}
	${CXX} ${CXXFLAGS} client.cpp -o  ${BIN_PATH}/${TARGET_CLI} ${LDFLAGES}