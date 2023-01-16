#!/bin/bash
##设置编译工具链,只在当前的终端生效，通过env可以看到具体的值

print_help() {
cat <<EOF
Use: source toolchain.sh cmd [args]
Cmd:
    export CROSS_COMPILE
    unset
Eg:
    source ./toolchain.sh export x86_64-linux-gnu
    source ./toolchain.sh unset

EOF
}

main() {
    if [ $# -lt 1 ]; then
        print_help
        return
    fi
    COMMAND=$1

    if [ $COMMAND = "export" ]; then
        if [ $# -lt 2 ]; then
            print_help
            return
        fi
        CROSS_COMPILE=$2
        if [ ${CROSS_COMPILE:${#CROSS_COMPILE}-1:1} != "-" ]; then
            CROSS_COMPILE=${CROSS_COMPILE}-
        fi
        echo "CROSS_COMPILE=$CROSS_COMPILE"

        ##========设置编译工具
        export CROSS_COMPILE=${CROSS_COMPILE}
        export CC=${CROSS_COMPILE}gcc
        export CXX=${CROSS_COMPILE}g++
        export AR=${CROSS_COMPILE}ar
        export AS=${CROSS_COMPILE}as
        export LD=${CROSS_COMPILE}ld
        export STRIP=${CROSS_COMPILE}strip
        export RANLIB=${CROSS_COMPILE}ranlib
        export NM=${CROSS_COMPILE}nm

        ##========获取交叉编译信息
        #编译机器
        HOST_OS=`uname -s`
        HOST_ARCH=`uname -m`
        #运行机器
        TARGET_PLATFORM=`$CC -v 2>&1 | grep Target | sed 's/Target: //'`
        TARGET_ARCH=`echo $TARGET_PLATFORM | awk -F'-' '{print $1}'`
        case $TARGET_PLATFORM in
            *mingw*) TARGET_OS=Windows ;;
            *android*) TARGET_OS=Android ;;
            *darwin*) TARGET_OS=Darwin ;;
            *) TARGET_OS=Linux ;;
        esac
        
        #========设置系统环境变量值
        export EASYNET_HOST_OS=$HOST_OS
        export EASYNET_HOST_ARCH=$HOST_ARCH
        export EASYNET_TARGET_OS=$TARGET_OS
        export EASYNET_TARGET_ARCH=$TARGET_ARCH
    elif [ $COMMAND = "unset" ]; then
        unset  CROSS_COMPILE
        unset  CC
        unset  CXX
        unset  AR
        unset  AS
        unset  LD
        unset  STRIP
        unset  RANLIB
        unset  NM
        unset  EASYNET_HOST_OS
        unset  EASYNET_HOST_ARCH
        unset  EASYNET_TARGET_OS
        unset  EASYNET_TARGET_ARCH
    else
        print_help
        return
    fi
}

main $@
echo "CC                    = $CC"
echo "CXX                   = $CXX"
echo "AR                    = $AR"
echo "AS                    = $AS"
echo "LD                    = $LD"
echo "STRIP                 = $STRIP"
echo "RANLIB                = $RANLIB"
echo "NM                    = $NM"
echo "EASYNET_HOST_OS       = $EASYNET_HOST_OS"
echo "EASYNET_HOST_ARCH     = $EASYNET_HOST_ARCH"
echo "EASYNET_TARGET_OS     = $EASYNET_TARGET_OS"
echo "EASYNET_TARGET_ARCH   = $EASYNET_TARGET_ARCH"