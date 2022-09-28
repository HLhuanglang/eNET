
#libeasynet.so--->.o文件---->.cpp文件
#当某个.cpp变化时,需要重新编译成.o,然后再重新生成.so

SRC_CPP=$(wildcard net/*.cpp util/*.cpp protocol/http/*.cpp)
SRC_C =$(wildcard protocol/http/*.c)
OBJ_CPP=$(patsubst %.cpp, %.o, ${SRC_CPP})
OBJ_C=$(patsubst %.c, %.o, ${SRC_C})

INC=net/*.h \
	util/*h \
	protocol/http/*.h\


CXXFLAGS+= -fpic -shared
LDFLAGES+= -I./net -I./util -I./protocol/http

.PHONY:default
default:${OUTPUT_LIB_PATH}/${TARGET_SO}
	@cp ${INC} ${OUTPUT_INC_PATH}

 
${OUTPUT_LIB_PATH}/${TARGET_SO}:${OBJ_CPP} ${OBJ_C}
	${CXX} ${CXXFLAGS} ${OBJ_CPP} ${OBJ_C} -o ${OUTPUT_LIB_PATH}/${TARGET_SO} ${LDFLAGES}

$(OBJ_CPP):%.o:%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@ ${LDFLAGES}

$(OBJ_C):%.o:%.c
	${CXX} ${CXXFLAGS} -c $< -o $@ ${LDFLAGES}
