SUB_DIRS=socket_practice\
			http_parser\
			libuv\

OUTPUT_BIN_DIR:=${OUTPUT_BIN_DIR}/raw_examples

.PHONY:default
default:build

.PHONY:build
build:
	@for dir in ${SUB_DIRS};do ${MAKE} -C $${dir}; done