BIN_PATH=${OUTPUT_BIN_DIR}/test
TARGET_BIN=$(patsubst %.cpp, % ,$(wildcard *.cpp))

CXXFLAGS+= -I${OUTPUT_INC_PATH} -L${OUTPUT_LIB_PATH} 
LDFLAGES+= -Wl,-rpath=${OUTPUT_LIB_PATH} -leasynet


$(shell mkdir -p ${BIN_PATH})

.PHONY:default
default:test

.PHONY: test
test:${TARGET_BIN}

%:%.cpp
	${CXX} ${CXXFLAGS} $< -o ${BIN_PATH}/$@ ${LDFLAGES}