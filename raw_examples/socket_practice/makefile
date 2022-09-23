SUB_DIRS=epoll\
			eventfd\
			timerfd\

OUTPUT_BIN_DIR:=${OUTPUT_BIN_DIR}/socket_practice

.PHONY:default
default:build

.PHONY:build
build:
	@for dir in ${SUB_DIRS};do ${MAKE} -C $${dir}; done