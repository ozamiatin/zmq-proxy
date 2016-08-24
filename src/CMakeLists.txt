# zmq-proxy Cmake build description.
# ==================================
#
# CMake should setup zmq libraries / headers and other dependencies
# automatically.
#

cmake_minimum_required(VERSION 2.8.1)

set(CMAKE_VERBOSE_MAKEFILE on)
enable_testing()

add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Wall -Wextra -O3 -pthread")
project(zmq_proxy)


include(ExternalProject)


ExternalProject_Add(zeromq
    PREFIX 3rd-party
    GIT_REPOSITORY "https://github.com/zeromq/libzmq"
    BUILD_COMMAND make
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install
    LOG_CONFIGURE ON
    LOG_INSTALL ON)
ExternalProject_Get_Property(zeromq binary_dir)
message(STATUS "ZEROMQ INSTALL DIR: " ${binary_dir}/install/include)
include_directories(${binary_dir}/install/include)
link_directories(${binary_dir}/install/lib)


ExternalProject_Add(cpp-redis
	PREFIX 3rd-party
    GIT_REPOSITORY "https://github.com/ozamiatin/cpp_redis.git"
    BUILD_COMMAND make
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install
    LOG_CONFIGURE ON
    LOG_INSTALL ON)
ExternalProject_Get_Property(cpp-redis binary_dir)
message(STATUS "CPP-REDIS INSTALL DIR: " ${binary_dir}/install/include)
include_directories(${binary_dir}/install/include)
link_directories(${binary_dir}/install/lib)


ExternalProject_Add(cppzmq
    PREFIX 3rd-party
    GIT_REPOSITORY "https://github.com/zeromq/cppzmq.git"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON)
ExternalProject_Get_Property(cppzmq source_dir)
set(INCLUDE_DIR_CPPZMQ ${source_dir})


set(Boost_USE_STATIC_LIBS ON)
add_definitions(-DBOOST_LOG_DYN_LINK -DBOOST_LOG_NO_THREADS)
find_package(Boost COMPONENTS program_options log REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${INCLUDE_DIR_CPPZMQ})
include_directories(${INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(HEADER_FILES "common/matchmaker.h"
                 "common/names.h")

set(SOURCE_FILES "common/configuration.cpp"
                 "common/matchmaker.cpp"
                 "common/matchmaker_redis.cpp"
                 "common/utils.cpp"
                 "proxy/centralproxy.cpp"
                 "proxy/localproxy.cpp"
                 "proxy/main.cpp")

add_executable(zmq-proxy ${HEADER_FILES} ${SOURCE_FILES})

add_dependencies(zmq-proxy zeromq
                           cpp-redis
                           cppzmq)

target_link_libraries(zmq-proxy pthread
                                zmq-static
                                ${Boost_LIBRARIES}
                                boost_program_options
                                boost_system
                                boost_log
                                cpp_redis)
