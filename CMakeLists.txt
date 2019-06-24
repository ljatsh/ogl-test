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

# Tutorial 3
add_executable(tutorial03_shader_uniform
    src/03_shader_uniform.cpp
)
target_link_libraries(tutorial03_shader_uniform
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial03_shader_uniform WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial03_shader_uniform/")

# Tutorial 4
add_executable(tutorial04_shader_vertex_attribute
    src/04_shader_vertex_attribute.cpp
)
target_link_libraries(tutorial04_shader_vertex_attribute
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial04_shader_vertex_attribute WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial04_shader_vertex_attribute/")

# Tutorial 5
add_executable(tutorial05_matrix
    src/05_matrix.cpp
)
target_link_libraries(tutorial05_matrix
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial05_matrix WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial05_matrix/")

# Tutorial 6
add_executable(tutorial06_colored_cube
    src/06_colored_cube.cpp
)
target_link_libraries(tutorial06_colored_cube
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial06_colored_cube WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial06_colored_cube/")

# Tutorial 7
add_executable(tutorial07_primitives
    src/07_primitives.cpp
)
target_link_libraries(tutorial07_primitives
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial07_primitives WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial07_primitives/")

# Tutorial 8
add_executable(tutorial08_texture_mapping
    src/08_texture_mapping.cpp
)
target_link_libraries(tutorial08_texture_mapping
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial08_texture_mapping WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial08_texture_mapping/")

# Tutorial 9
add_executable(tutorial09_textured_cube
    src/09_textured_cube.cpp
)
target_link_libraries(tutorial09_textured_cube
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial09_textured_cube WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial09_textured_cube/")

# Tutorial 10
add_executable(tutorial10_input_control
    src/10_input_control.cpp
)
target_link_libraries(tutorial10_input_control
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial10_input_control WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial10_input_control/")

# Tutorial 11
add_executable(tutorial11_blender_model
    src/11_blender_model.cpp
)
target_link_libraries(tutorial11_blender_model
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial11_blender_model WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial11_blender_model/")

# Tutorial 12
add_executable(tutorial12_vbo_index
    src/12_vbo_index.cpp
)
target_link_libraries(tutorial12_vbo_index
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial12_vbo_index WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial12_vbo_index/")

# Tutorial 13
add_executable(tutorial13_transparency
    src/13_transparency.cpp
)
target_link_libraries(tutorial13_transparency
    ${ALL_LIBS}
    common
)
# Xcode and Visual working directories
create_target_launcher(tutorial13_transparency WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build/tutorial13_transparency/")

if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
    add_custom_command(
        TARGET tutorial01_first_window POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/tutorial01_first_window${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/tutorial01_first_window/"
    )
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )
    add_custom_command(
        TARGET common POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shader" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/shader"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/texture" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/texture"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/model" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/model"
    )
endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

