# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (Tutorials)

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

#if(INCLUDE_DISTRIB)
#    add_subdirectory(distrib)
#endif(INCLUDE_DISTRIB)

include_directories(
    external/AntTweakBar-1.16/include/
    external/glfw-3.1.2/include/GLFW/
    external/glm-0.9.7.1/
    external/glew-1.13.0/include/
    external/assimp-3.0.1270/include/
    external/bullet-2.81-rev2613/src/
    common
)

set(ALL_LIBS
    ${OPENGL_LIBRARY}
    glfw
    GLEW_1130
)

add_definitions(
    -DTW_STATIC
    -DTW_NO_LIB_PRAGMA
    -DTW_NO_DIRECT3D
    -DGLEW_STATIC
    -D_CRT_SECURE_NO_WARNINGS
)

add_library(common
    common/helper.cpp
    common/shader.cpp
)

# Tutorial 1
add_executable(tutorial01_first_window 
    src/01_first_window.cpp
)
target_link_libraries(tutorial01_first_window
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
#set_target_properties(tutorial01_first_window PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial01_first_window/")
create_target_launcher(tutorial01_first_window WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial01_first_window/")

# Tutorial 2
add_executable(tutorial02_red_triangle
    src/02_red_triangle.cpp
)
target_link_libraries(tutorial02_red_triangle
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
#set_target_properties(tutorial02_red_triangle PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tutorial02_red_triangle/")
create_target_launcher(tutorial02_red_triangle WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial02_red_triangle/")


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
    add_custom_command(
        TARGET tutorial01_first_window POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tutorial01_first_window${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tutorial01_first_window/"
    )
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )
    add_custom_command(
        TARGET common POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shader" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/shader"
    )
endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

