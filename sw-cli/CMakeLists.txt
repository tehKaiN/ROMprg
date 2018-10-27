cmake_minimum_required(VERSION 3.7)

project(romprg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601")
# set(LINKER_FLAGS "")

# Project files
file(GLOB_RECURSE sources src/*.h src/*.hpp src/*.c src/*.cpp)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(romprg ${sources})
set_target_properties(romprg PROPERTIES LINKER_LANGUAGE CXX)

#wjwwood-serial
find_package(serial REQUIRED)
include_directories(${serial_INCLUDE_DIRS})
target_link_libraries(romprg serial)

#fmt
find_package(fmt REQUIRED)
include_directories(${fmt_INCLUDE_DIRS})
target_link_libraries(romprg fmt::fmt)

# Linking - custom flags
# target_link_libraries(app ${LINKER_FLAGS})
