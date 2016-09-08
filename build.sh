#!/bin/sh

DEBUG=1
BUILD=1
INSTALL=0
CLEAN=0
PARAM=

for opt in $@
do
  cmd=`echo $opt | tr [a-z] [A-Z]`
  case "$cmd" in 
    ALL)
      INSTALL=1
      BUILD=1
      CLEAN=1
      ;;

    BUILD)
      INSTALL=0
      BUILD=1
      CLEAN=0
      ;;

    REBUILD)
      INSTALL=0
      BUILD=1
      CLEAN=1
      ;;

    CLEAN)
      INSTALL=0
      BUILD=0
      CLEAN=1
      ;;

     RELEASE)
       DEBUG=0
       ;;

     DEBUG)
       DEBUG=1
       ;;

     *)
       PARAM="$PARAM $opt "
       ;;

  esac
done

SCRIPT_NAME=`readlink -f $0`
FOLDER=`dirname ${SCRIPT_NAME}`
echo $FOLDER

if [ $DEBUG -eq 1 ]; then
  DTYPE="-DCMAKE_BUILD_TYPE=Debug"
  BUILD_TYPE="Debug"
else
  DTYPE="-DCMAKE_BUILD_TYPE=Release"
  BUILD_TYPE="Release"
fi

if [ "`uname -p`" == "x86" ]; then
  BUILD_TYPE="${BUILD_TYPE}32"
else
  BUILD_TYPE="${BUILD_TYPE}64"
fi

BUILD_FOLDER=${FOLDER}/build/${BUILD_TYPE}
if [ $CLEAN -eq 1 ]; then
  echo "Clean the build...."
  if [ -d ${BUILD_FOLDER} ];then
    echo "rm -rf ${BUILD_FOLDER}"
    rm -rf ${BUILD_FOLDER}
  fi
  if [ -d ${FOLDER}/install/${BUILD_TYPE} ]; then
    echo "rm -rf ${FOLDER}/install/${BUILD_TYPE}"
    rm -rf ${FOLDER}/install/${BUILD_TYPE}
  fi
fi

if [ $BUILD -eq 0 -a $INSTALL -eq 0 ]; then
  exit 0
fi

if [ ! -d ${BUILD_FOLDER} ]; then
  echo "mkdir -p ${BUILD_FOLDER}"
  mkdir -p ${BUILD_FOLDER}
fi

cd ${BUILD_FOLDER}
echo "[COMMAND]: cmake $DTYPE `dirname ${SCRIPT_NAME}`"
cmake $DTYPE -DPROJ_BUILD_DIR="$FOLDER" `dirname ${SCRIPT_NAME}`
echo "Build folder is ${BUILD_FOLDER}"

if [ $BUILD -eq 1 ]; then
  echo "[COMMAND]: make $PARAM"
  make $PARAM
  if [ $? -ne 0 ];then
    exit -1
  fi
fi
  
if [ $INSTALL -eq 1 ];then
  echo "[COMMAND]: make install"
  make install
  if [ $? -ne 0 ];then
    exit -1
  fi
fi

cd -
