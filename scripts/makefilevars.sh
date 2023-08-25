#!/bin/bash

# 项目根目录
ROOT_DIR=$(pwd)

# 编译生成目录
BUILD_EASYNET_DIR=${ROOT_DIR}/build/easy_net
BUILD_BIN_DIR=${ROOT_DIR}/build/bin
BUILD_OBJS_DIR=${ROOT_DIR}/build/objs

# 头文件
NET_PUB_HEADERS=$(find ${ROOT_DIR}/easy_net/inc -name "*.h")
HTTP_PUB_HEADERS=$(find ${ROOT_DIR}/protocol/http -name "*.h")
MQTT_PUB_HEADERS=$(find ${ROOT_DIR}/protocol/mqtt -name "*.h")

echo "# 项目根目录"
echo "ROOT_DIR=${ROOT_DIR}"
echo
echo "# 编译生成目录"
echo "BUILD_EASYNET_DIR=${BUILD_EASYNET_DIR}"
echo "BUILD_BIN_DIR=${BUILD_BIN_DIR}"
echo "BUILD_OBJS_DIR=${BUILD_OBJS_DIR}"
echo

echo "# 头文件"
echo
echo "NET_PUB_HEADERS = \\"
for header in ${NET_PUB_HEADERS}; do
    echo -e "\t${header} \\"
done

echo
echo "HTTP_PUB_HEADERS = \\"
for header in ${HTTP_PUB_HEADERS}; do
    echo -e "\t${header} \\"
done

echo
echo "MQTT_PUB_HEADERS = \\"
for header in ${MQTT_PUB_HEADERS}; do
    echo -e "\t${header} \\"
done