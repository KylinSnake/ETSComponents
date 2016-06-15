#!/bin/sh

SCRIPT_NAME=`readlink -f $0`
FOLDER=`dirname ${SCRIPT_NAME}`/../
echo $FOLDER
if [ -d ${FOLDER}/build ];then
  rm -rf ${FOLDER}/build
fi
mkdir -p ${FOLDER}/build
cd ${FOLDER}/build
cmake `dirname ${SCRIPT_NAME}`
if [ $? -eq 0 ];then
  make -j 2
fi

if [ $? -eq 0 ];then
  make install
fi

cd -
