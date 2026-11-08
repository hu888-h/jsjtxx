cmake_minimum_required(VERSION 3.16)
project(px_grahics LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GL   REQUIRED gl)
pkg_check_modules(X11  REQUIRED x11)
pkg_check_modules(GLX  REQUIRED glx)
pkg_check_modules(FT2  REQUIRED freetype2)

add_executable(px
    src/main.c src/app.c src/canvas.c src/text.c src/ui.c
    src/algo.c src/fill.c src/view.c)

target_include_directories(px PRIVATE include ${GL_INCLUDE_DIRS} ${X11_INCLUDE_DIRS} ${FT2_INCLUDE_DIRS})
target_link_libraries(px ${GL_LIBRARIES} ${X11_LIBRARIES} ${GLX_LIBRARIES} ${FT2_LIBRARIES} m)
