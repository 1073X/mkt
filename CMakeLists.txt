cmake_minimum_required (VERSION 3.19)
project("${CMAKE_PROJECT_NAME}")
include("$ENV{TOOLCHAIN_PATH}/CMakeLists.txt.in")

add_lib("libmkt" "source/lib")
add_unittest(libmkt ${LIB_REF_STUB} ${LIB_REF} ${LIB_SHM} ${LIB_CFG} ${LIB_LOG} ${LIB_COM})
