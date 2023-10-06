cmake_minimum_required(VERSION 3.26)
project(Clip2Image-gui)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig)

pkg_check_modules(LIBS REQUIRED opencv4)

add_executable(clip2image WIN32 clip2image-gui.cpp)

target_link_libraries(clip2image  ${LIBS_LIBRARIES})
target_include_directories(clip2image PRIVATE ${LIBS_INCLUDE_DIRS})
target_compile_options(clip2image PUBLIC ${LIBS_CFLAGS})
