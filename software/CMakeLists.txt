cmake_minimum_required(VERSION 3.12)
include(lib/pico-sdk/external/pico_sdk_import.cmake)

project(VersaTerm C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

add_compile_options(-Wall)

include_directories(
	lib/PicoDVI/software/include
	lib/PicoVGA
	)

add_subdirectory(lib/PicoDVI/software/libdvi)
add_subdirectory(lib/PicoVGA)
add_subdirectory(src)
