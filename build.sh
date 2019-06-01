#!/bin/sh

DEBUG=1
BUILD=1
INSTALL=0
CLEAN=0
PARAM=
TEST=0
TEST_COV=0
CMAKE_PARAM=

for opt in $@
do
  cmd=`echo $opt | tr [a-z] [A-Z]`
  case "$cmd" in 
    ALL)
      INSTALL=1
      BUILD=1
      CLEAN=1
	  TEST=1
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
	   TEST=1
       ;;

     TEST)
       DEBUG=1
	   TEST=1
       ;;

     TEST_COVERAGE)
       DEBUG=1
	   TEST=1
	   TEST_COV=1
       ;;


     *)
       if [ ${opt:0:2} == '-D' ]; then
		   CMAKE_PARAM="$CMAKE_PARAM $opt"
	   else
		   PARAM="$PARAM $opt "
	   fi
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

TEST_COV_PARAM=

if [ $TEST_COV -eq 1 ];then
	TEST_COV_PARAM="-DTEST_COVERAGE=on"
fi

cd ${BUILD_FOLDER}
echo "[COMMAND]: cmake $DTYPE -DBUILD_SHARED_LIBS=on ${TEST_COV_PARAM} ${CMAKE_PARAM} `dirname ${SCRIPT_NAME}`"
cmake $DTYPE -DBUILD_SHARED_LIBS=on ${TEST_COV_PARAM} ${CMAKE_PARAM} `dirname ${SCRIPT_NAME}`
echo "Build folder is ${BUILD_FOLDER}"

if [ $BUILD -eq 1 ]; then
  echo "[COMMAND]: make $PARAM"
  make $PARAM
  if [ $? -ne 0 ];then
    exit -1
  fi
fi

if [ $TEST -eq 1 ]; then
	echo "[COMMAND]: make test"
	make CTEST_OUTPUT_ON_FAILURE=1 test
	if [ $? -ne 0 ];then
		exit -1
	fi
fi

if [ $TEST_COV -eq 1 ]; then
	echo "[COMMAND]: lcov -c -d ${BUILD_FOLDER} -o ${BUILD_FOLDER}/raw.info"
	lcov -c -d ${BUILD_FOLDER} -o ${BUILD_FOLDER}/raw.info
	if [ $? -ne 0 ];then
		exit -1
	fi
	echo "[COMMAND]: lcov -e ${BUILD_FOLDER}/raw.info \"*snake*\" -o ${BUILD_FOLDER}/filter_a.info"
	lcov -e ${BUILD_FOLDER}/raw.info "*snake*" -o ${BUILD_FOLDER}/filter_a.info
	if [ $? -ne 0 ];then
		exit -1
	fi
	echo "[COMMAND]: lcov -r ${BUILD_FOLDER}/filter_a.info \"*test*\" -o ${BUILD_FOLDER}/filter_b.info"
	lcov -r ${BUILD_FOLDER}/filter_a.info "*test*" -o ${BUILD_FOLDER}/filter_b.info
	if [ $? -ne 0 ];then
		exit -1
	fi
	mkdir -p ${BUILD_FOLDER}/../TestCoverage
	rm -rf ${BUILD_FOLDER}/../TestCoverage/*
	echo "[COMMAND]: genhtml -o ${BUILD_FOLDER}/../TestCoverage -t "snake" ${BUILD_FOLDER}/filter_b.info"
	genhtml -o ${BUILD_FOLDER}/../TestCoverage -t "snake" ${BUILD_FOLDER}/filter_b.info
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
