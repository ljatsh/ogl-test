
cmake_minimum_required (VERSION 2.6)
project (ogl_test_under_sdl)

find_package(OpenGL REQUIRED)

add_subdirectory(external/sdl)
link_libraries(SDL2-static ${OPENGL_LIBRARY})

#https://stackoverflow.com/questions/27585896/how-to-use-framework-using-cmake

# 使用sdl创建一个空窗口
add_executable(01_primitive src/sdl_01_primitive.cpp)