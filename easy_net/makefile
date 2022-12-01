
#libeasynet.so--->.o文件---->.cpp文件
#当某个.cpp变化时,需要重新编译成.o,然后再重新生成.so

SRC_DIR=${ROOT_DIR}/easy_net
SRC_NET_DIR=${SRC_DIR}/net
SRC_UTIL_DIR=${SRC_DIR}/util
SRC_HTTP_DIR=${SRC_DIR}/protocol/http

OBJS=$(patsubst ${SRC_NET_DIR}/%.cpp, ${OUTPUT_OBJ_DIR}/%.o, $(wildcard ${SRC_NET_DIR}/*.cpp))
OBJS+=$(patsubst ${SRC_UTIL_DIR}/%.cpp, ${OUTPUT_OBJ_DIR}/%.o, $(wildcard ${SRC_UTIL_DIR}/*.cpp))
OBJS+=$(patsubst ${SRC_HTTP_DIR}/%.cpp, ${OUTPUT_OBJ_DIR}/%.o, $(wildcard ${SRC_HTTP_DIR}/*.cpp))

INC=net/*.h \
	util/*h \
	protocol/http/*.h\


CXXFLAGS+= -fpic -shared -Wall
LDFLAGES+= -I./net -I./util -I./protocol/http


.PHONY:default
default:${OUTPUT_LIB_PATH}/${TARGET_SO}
	@cp ${INC} ${OUTPUT_INC_PATH}

 
${OUTPUT_LIB_PATH}/${TARGET_SO}:${OBJS}
	${CXX} ${CXXFLAGS} $^ -o $@ ${LDFLAGES}

${OUTPUT_OBJ_DIR}/%.o:${SRC_NET_DIR}/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@ ${LDFLAGES}


${OUTPUT_OBJ_DIR}/%.o:${SRC_UTIL_DIR}/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@ ${LDFLAGES}


${OUTPUT_OBJ_DIR}/%.o:${SRC_HTTP_DIR}/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@ ${LDFLAGES}