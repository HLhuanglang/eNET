SRC=buffer_test.cpp\
	event_loop_test.cpp\
	ini_test.cpp\
	subreactor_pool_test.cpp\
	tcp_server_test.cpp\
	util_test.cpp\

TARGET_BIN=$(patsubst %.cpp,%,$(SRC))

CXXFLAGS+= -I${OUTPUT_INC_PATH} -L${OUTPUT_LIB_PATH} 
LDFLAGES+= -Wl,-rpath=${OUTPUT_LIB_PATH} -leasynet

BIN_PATH=${OUTPUT_BIN_DIR}/test
$(shell mkdir -p ${BIN_PATH})

.PHONY:default
default:test

.PHONY: test
test:${TARGET_BIN}

%:%.cpp
	${CXX} ${CXXFLAGS} $< -o ${BIN_PATH}/$@ ${LDFLAGES}