cmake_minimum_required(VERSION 3.2)
project(playOnStd)

include(CTest)
enable_testing()

add_executable(
	playOnStd
	thread.cpp
	)

target_compile_features(playOnStd PUBLIC cxx_std_20)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#add_subdirectory(../../opt/googletest googletest)

target_include_directories(
	playOnStd
	PUBLIC ${PROJECT_SOURCE_DIR}
	)

target_link_libraries(
	playOnStd
#	gmock_main
	)

add_test(
	NAME playOnStd
	COMMAND $<TARGET_FILE:playOnStd>
	)
