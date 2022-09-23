
#libeasynet.so--->.o文件---->.cpp文件
#当某个.cpp变化时,需要重新编译成.o,然后再重新生成.so

SRCS=$(wildcard net/*.cpp util/*.cpp)
OBJS=$(patsubst %.cpp, %.o, ${SRCS})

INC=net/*.h \
	util/*h \


CXXFLAGS+= -fpic -shared
LDFLAGES+= -I./net -I./util

.PHONY:default
default:${OUTPUT_LIB_PATH}/${TARGET_SO}
	@cp ${INC} ${OUTPUT_INC_PATH}

 
${OUTPUT_LIB_PATH}/${TARGET_SO}:${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} -o ${OUTPUT_LIB_PATH}/${TARGET_SO} ${LDFLAGES}

$(OBJS):%.o:%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@ ${LDFLAGES}
