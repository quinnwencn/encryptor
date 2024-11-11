cmake_minimum_required(VERSION 3.18)

project(encryptor)

set(CMAKE_CXX_STANDARD 11)
set(GLOG_ROOT_DIR /usr)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

find_package(OpenSSL REQUIRED)
find_package(glog REQUIRED)

add_subdirectory(src)
