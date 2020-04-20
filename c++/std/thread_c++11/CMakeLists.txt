cmake_minimum_required(VERSION 3.2)
project(thread)

add_executable(
	${PROJECT_NAME}
	${PROJECT_NAME}.cpp
	)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
