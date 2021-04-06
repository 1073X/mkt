cmake_minimum_required (VERSION 3.19)
project("${CMAKE_PROJECT_NAME}")
include("$ENV{TOOLCHAIN_PATH}/CMakeLists.txt.in")

add_lib("libmkt" "source/lib/mkt")
add_lib("libmktstub" "source/lib/stub")
add_lib("libmktsvc" "source/lib/svc")
add_unittest(libmkt libmktsvc libmktstub
    ${LIB_REF_STUB} ${LIB_REF}
    ${LIB_SVC} ${LIB_JOB} ${LIB_CMD} ${LIB_ASP}
    ${LIB_NET} ${LIB_SHM} ${LIB_CFG} ${LIB_LOG}
    ${LIB_COM}
)
