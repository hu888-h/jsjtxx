cmake_minimum_required(VERSION 3.16)
project(px_qt LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wno-unused-parameter -O2)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SRCS
    src/main.cpp
    src/jstmainwindow.cpp
    src/canvasview.cpp
    src/algo.c
)
set(HDRS
    include/jstmainwindow.h
    include/canvasview.h
)

# 对含 Q_OBJECT 的头文件显式跑 moc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS -I${CMAKE_SOURCE_DIR}/include)

qt6_wrap_cpp(MOC_SRCS ${HDRS})

add_executable(px ${SRCS} ${MOC_SRCS})
target_include_directories(px PRIVATE include)
target_link_libraries(px PRIVATE Qt6::Widgets m)
