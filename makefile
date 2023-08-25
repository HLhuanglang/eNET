#########总控makefile#########
##自选配置
include easy_net_compile_config.mk

##makeifle变量, 请手动执行./script/makefilevars.sh生成
include makefilevars.mk

##封装一些功能
MAKE  = make --no-print-directory
MAKEF = $(MAKE) -f makefile.in
MKDIR = -mkdir -p 2>/dev/null
CP    = -cp -r 2>/dev/null
RM    = -rm -r 2>/dev/null

#============================================================
# 通用编译步骤
#============================================================
.PHONY: default
default:
	@echo "Usage: make [all|easy_net|raw_examples|test|clean|install|uninstall]"

.PHONY: all
all: raw_examples easy_net test

.PHONY: premake
premake:
	@${MKDIR} ${BUILD_BIN_DIR}
	@${MKDIR} ${BUILD_EASYNET_DIR}
	@${MKDIR} ${BUILD_OBJS_DIR}


#============================================================
# easy_net库编译
#============================================================
#源码和头文件
EASYNET_SRC_DIRS= ${ROOT_DIR}/easy_net/base \
				  ${ROOT_DIR}/easy_net/util

EASYNET_INC_DIRS =${ROOT_DIR}/easy_net/base \
				  ${ROOT_DIR}/easy_net/util

#对外暴露的头文件
EASYNET_OUT_HEADERS= ${BASE_PUB_HEADERS} ${UTIL_PUB_HEADERS}

#是否添加http模块
ifeq ($(WITH_HTTP),yes)
	EASYNET_INC_DIRS += ${ROOT_DIR}/easy_net/protocol/http
	EASYNET_SRC_DIRS += ${ROOT_DIR}/easy_net/protocol/http
	EASYNET_OUT_HEADERS += ${HTTP_PUB_HEADERS}
endif

#是否添加mqtt模块
ifeq ($(WITH_MQTT),yes)
	EASYNET_INC_DIRS += ${ROOT_DIR}/easy_net/protocol/mqtt
	EASYNET_SRC_DIRS += ${ROOT_DIR}/easy_net/protocol/mqtt
	EASYNET_OUT_HEADERS += ${MQTT_PUB_HEADERS}
endif

#提取编译lib所需要的src
EASYNET_SRCS = $(foreach dir, $(EASYNET_SRC_DIRS), $(wildcard $(dir)/*.cpp))
EASYNET_OBJS = $(EASYNET_SRCS:${ROOT_DIR}/easy_net/%.cpp=$(BUILD_OBJS_DIR)/%.o)

.PHONY: easy_net
easy_net: premake
	@$(MKDIR) ${BUILD_EASYNET_DIR}/include/easy_net
	@$(MKDIR) ${BUILD_EASYNET_DIR}/lib
	@$(MAKEF) MODE=LIB \
		TARGET_NAME=$@ \
		LIB_TYPE="SHARED|STATIC" \
		BUILD_TYPE=DEBUG \
		DEFINES=DEBUG	\
		OUTDIR=${BUILD_EASYNET_DIR}/lib \
		SRCS="$(EASYNET_SRCS)" \
		OBJS="$(EASYNET_OBJS)" \
		INCDIRS="$(EASYNET_INC_DIRS)" \
		SRCS_DIR=${ROOT_DIR}/easy_net \
		OBJS_DIR=${BUILD_OBJS_DIR}
		
	@$(CP) $(EASYNET_OUT_HEADERS) ${BUILD_EASYNET_DIR}/include/easy_net


#============================================================
# easy_net单元测试
#============================================================
TEST_SRCDIRS = ${ROOT_DIR}/test/base_test\
			   ${ROOT_DIR}/test/util_test

#是否测试http模块
ifeq ($(WITH_HTTP),yes)
	TEST_SRCDIRS +=  ${ROOT_DIR}/test/http_test
endif

#是否测试mqtt模块
ifeq ($(WITH_MQTT),yes)
	TEST_SRCDIRS +=  ${ROOT_DIR}/test/mqtt_test
endif

#过滤所有的单元测试文件
TEST_SRCS = $(foreach dir, $(TEST_SRCDIRS), $(wildcard $(dir)/*.cpp))
TEST_TARGET = $(TEST_SRCS:${ROOT_DIR}/test/%.cpp=$(BUILD_BIN_DIR)/test/%)

.PHONY: test
test: easy_net
	@${MKDIR} ${BUILD_BIN_DIR}/test
	@$(MAKEF) MODE=EXE\
		DEP_INCDIRS=${BUILD_EASYNET_DIR}/include\
		DEP_LIBSDIRS=${BUILD_EASYNET_DIR}/lib\
		DEP_LIBS=easy_net\
		TEST_SRCS_DIR=${ROOT_DIR}/test\
		TEST_TARGET_DIR=${BUILD_BIN_DIR}/test\
		TEST_TARGET="$(TEST_TARGET)"\

#============================================================
# 编译raw_examples
#============================================================
.PHONY: raw_examples
raw_examples: premake
	@${MAKE} -C raw_examples OUTPUT_BIN_DIR=${BUILD_BIN_DIR}/raw_examples


#============================================================
# 安装与卸载easy_net
#============================================================
.PHONY: install
install:
	@${MKDIR} ${PREFIX}
	@${CP} ${BUILD_EASYNET_DIR}/* ${PREFIX}
	@echo "Install easy_net Done!"

.PHONY: uninstall
uninstall:
	@${RM} ${PREFIX}/*
	@echo "Uninstall easy_net Done!"

#============================================================
# 调试makefile
#============================================================
.PHONY: debug_mk
debug_mk:

#============================================================
# 清理编译环境
#============================================================
.PHONY: clean
clean:
	@find . -name "*.o" | xargs rm -rf
	@find . -name "*.d" | xargs rm -rf

.PHONY: clean_all
clean_all:clean
	@${RM} build
	@echo "Clean All done."
