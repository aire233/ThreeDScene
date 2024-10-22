# CMakeLists.txt
cmake_minimum_required(VERSION 3.3)
project(ThreeDScene)

set(CMAKE_CXX_STANDARD 11)

# 添加 include 目录
include_directories(include)
include_directories(include/imgui)

# 查找 GLFW
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
link_directories(${GLFW_LIBRARY_DIRS})
add_definitions(${GLFW_CFLAGS_OTHER})

# 查找 GLM
find_package(glm REQUIRED)
if(glm_FOUND)
    include_directories(${GLM_INCLUDE_DIRS})
endif()

# 手动列出源文件，包括 ImGui 和 GLAD
set(SOURCES
    src/main.cpp
    src/glad.c
    src/imgui/imgui.cpp
    src/imgui/imgui_draw.cpp
    src/imgui/imgui_tables.cpp
    src/imgui/imgui_widgets.cpp
    src/imgui/imgui_impl_glfw.cpp
    src/imgui/imgui_impl_opengl3.cpp
)

add_executable(ThreeDScene ${SOURCES})

# 链接库
target_link_libraries(ThreeDScene ${GLFW_LIBRARIES} dl GL)

add_custom_command(TARGET ThreeDScene POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:ThreeDScene>/shaders)

add_custom_command(TARGET ThreeDScene POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/scene.txt $<TARGET_FILE_DIR:ThreeDScene>/scene.txt)

add_custom_command(TARGET ThreeDScene POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/skybox $<TARGET_FILE_DIR:ThreeDScene>/skybox)