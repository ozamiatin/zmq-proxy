# zmq-proxy Cmake build description.
# ==================================
#
# CMake should find the zmq libraries / headers automatically if they are
# installed system-wide. If CMake cannot find them not, or you would like to
# use custom built ones, set these variables:
#
#   - ZEROMQ_LIB_DIR to the directory where libzmq / libzmq-shared is located
#
#   - ZEROMQ_INCLUDE_DIR to the directory where zmq.h is located
#

cmake_minimum_required(VERSION 2.8.1)
enable_testing()

add_compile_options(-std=c++11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -O2")

set(INCLUDE_DIR "~/3rd-party/cppzmq"
                "~/3rd-party/easyloggingpp/src"
                "~/3rd-party/cpp_redis/includes")


project(zmq_proxy)

find_package(Boost COMPONENTS program_options REQUIRED)

ExternalProject_Add(zeromq
    GIT_REPOSITORY "https://github.com/zeromq/libzmq"
    DOWNLOAD_DIR "3rd-party"
    BUILD_COMMAND "make -j"
    INSTALL_DIR "/usr/local/lib"
    INSTALL_COMMAND "make install && sudo ldconfig"
    CMAKE_ARGS "-DCMAKE_BUILD_TYPE=Release")


include_directories(${Boost_INCLUDE_DIRS})
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
target_link_libraries(zmq-proxy zmq ${Boost_LIBRARIES} boost_program_options boost_system cpp_redis)
