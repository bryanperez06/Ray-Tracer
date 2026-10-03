# Dependencies are pulled automatically via vcpkg manifest mode.
# Build with:
#   VCPKG_ROOT=/path/to/vcpkg cmake --preset default
#   cmake --build build
cmake_minimum_required(VERSION 3.20)
project(par-raytrace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include all optimizations
set(CMAKE_BUILD_TYPE Release)

# export the compile_commands.json file to the build directory for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# glm (header-only, ships a CMake config package via vcpkg)
find_package(glm CONFIG REQUIRED)

# add the TIRA library (Git submodule of this project)
add_subdirectory(tira)

# Find everything needed to run ImGui using OpenGL with GLFW3
find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# Windows only exposes OpenGL 1.1 in its headers and in opengl32.dll, so
# anything newer has to be loaded at runtime. glad does that; see cpp/opengl.h.
find_package(glad CONFIG REQUIRED)

add_executable(raytrace
                            cpp/pathtrace.cpp
                            cpp/display.cpp
                            cpp/imgui_manager.cpp
)

# tira_images carries the tira and stb include roots, so neither is repeated
# here. cpp/stb_image_impl.cpp is gone with it: the tira_stb target inside
# tira/ is the same two #defines, compiled once.
target_include_directories(raytrace PRIVATE
          ${CMAKE_SOURCE_DIR}
)

target_link_libraries(raytrace PRIVATE
                                        tira_images
                                        glm::glm
                                        imgui::imgui
                                        glfw
                                        glad::glad
                                        OpenGL::GL
)

# xtensor::use_xsimd (linked above) and defines XTENSOR_USE_XSIMD, but
# this is only useful if the compiler generates wide vector instructions.
# Delete this line and the target system doesn't support them
target_compile_options(raytrace PRIVATE ${MIE_ARCH_FLAGS})
