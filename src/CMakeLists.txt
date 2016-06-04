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

cmake_minimum_required(VERSION 3.2.0)
enable_testing()

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

set(ZEROMQCPP_INCLUDE_DIR "~/3rd-party/cppzmq"
                          "~/3rd-party/easyloggingpp/src")


project(zmq_proxy)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${ZEROMQCPP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)


set(SOURCE_FILES "proxy/centralproxy.cpp"
                 "proxy/localproxy.cpp"
                 "proxy/main.cpp")


add_executable(zmq-proxy ${SOURCE_FILES})
target_link_libraries(zmq-proxy zmq)
