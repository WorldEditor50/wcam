cmake_minimum_required(VERSION 3.5)

project(wcam LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# src
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CAMERA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/camera)
set(LIBRARIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty)

file(GLOB SRC_FILES
    ${SRC_DIR}/*.h
    ${SRC_DIR}/*.hpp
    ${SRC_DIR}/*.cpp
    ${SRC_DIR}/*.ui)
# camera
file(GLOB CAMERA_FILES
    ${CAMERA_DIR}/*.h
    ${CAMERA_DIR}/*.hpp
    ${CAMERA_DIR}/*.cpp)
list(APPEND SRC_FILES ${CAMERA_FILES})
# libyuv
include_directories(${LIBRARIES_DIR}/libyuv/include)
include_directories(${LIBRARIES_DIR}/libjpeg/include)
set(LIBYUV_LIBS
    ${LIBRARIES_DIR}/libyuv/lib/yuv.lib
    ${LIBRARIES_DIR}/libjpeg/lib/jpeg-static.lib)
add_executable(wcam ${SRC_FILES})
target_compile_definitions(wcam PRIVATE _UNICODE UNICODE)
target_link_libraries(wcam PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${LIBYUV_LIBS})
