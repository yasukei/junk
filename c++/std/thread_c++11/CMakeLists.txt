cmake_minimum_required(VERSION 3.2)
project(thread)

add_executable(
	${CMAKE_PROJECT_NAME}
	${CMAKE_PROJECT_NAME}.cpp
	)

target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_11)
