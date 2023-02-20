cmake_minimum_required(VERSION 3.5)

project(gimc)

add_library(core_lib STATIC
	src/core_lib/calcs.cpp
	src/core_lib/config.cpp
	src/core_lib/parser.cpp
	src/core_lib/utils.cpp
)

target_include_directories(core_lib
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

add_executable(gimc
	src/gimc.cpp
)

target_link_libraries(gimc
    PRIVATE
		core_lib
)