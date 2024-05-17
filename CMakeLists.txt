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

find_package(QT NAMES Qt6 Qt5 COMPONENTS Xml Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Xml Widgets REQUIRED)

# src
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CAMERA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/camera)
set(IMPROC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/improcess)
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
# improcess
file(GLOB IMPROC_FILES
    ${IMPROC_DIR}/*.h
    ${IMPROC_DIR}/*.hpp
    ${IMPROC_DIR}/*.cpp)
list(APPEND SRC_FILES ${IMPROC_FILES})
# libyuv
include_directories(${LIBRARIES_DIR}/libyuv/include)
include_directories(${LIBRARIES_DIR}/libjpeg/include)
set(LIBYUV_LIBS
    ${LIBRARIES_DIR}/libyuv/lib/yuv.lib
    ${LIBRARIES_DIR}/libjpeg/lib/jpeg-static.lib)
# opencv
set(OpenCV_DIR "D:/home/3rdparty/opencv48/x64/vc16/lib")
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message("FOUND opencv")
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()
# ncnn
set(NCNN_DIR "D:/home/3rdparty/ncnn")
include_directories(${NCNN_DIR}/include)
set(NCNN_STATIC ${NCNN_DIR}/lib/ncnn.lib -fopenmp)

add_executable(wcam ${SRC_FILES})
target_compile_definitions(wcam PRIVATE _UNICODE UNICODE)
target_link_libraries(wcam PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Xml
    ${LIBYUV_LIBS}
    ${OpenCV_LIBS}
    ${NCNN_STATIC})
