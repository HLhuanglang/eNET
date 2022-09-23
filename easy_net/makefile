
SRCS=$(wildcard net/*.cpp util/*.cpp)

INC=net/*.h \
	util/*h \


CXXFLAGS+= -fpic -shared
LDFLAGES+= -I./net -I./util

.PHONY:default
default:${OUTPUT_LIB_PATH}/${TARGET_SO}
	@cp ${INC} ${OUTPUT_INC_PATH}

 
${OUTPUT_LIB_PATH}/${TARGET_SO}:${SRCS}
	${CXX} ${CXXFLAGS} ${SRCS} -o ${OUTPUT_LIB_PATH}/${TARGET_SO} ${LDFLAGES}
