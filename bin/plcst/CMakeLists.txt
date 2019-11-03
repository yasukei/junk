cmake_minimum_required(VERSION 3.2)

# -------------------------------------------------------------------
# DownloadProject
# -------------------------------------------------------------------
configure_file(CMakeLists.txt.in DownloadProject/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/DownloadProject" )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/DownloadProject" )

# -------------------------------------------------------------------
# gtest by DownloadProject
# -------------------------------------------------------------------
include("${CMAKE_BINARY_DIR}/DownloadProject-src/DownloadProject.cmake")
download_project(
	PROJ gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG master
	"${UPDATE_DISCONNECTED_IF_AVAILABLE}"
	)
# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${gtest_SOURCE_DIR}" "${gtest_BINARY_DIR}")

# -------------------------------------------------------------------
# main
# -------------------------------------------------------------------
project(liboreore)

include(CTest)
enable_testing()

add_compile_options(-Wall -Wextra -O0 -g --coverage)

#add_subdirectory(src)
#add_subdirectory(test)

add_executable(
	interpreter_test
	interpreter_test.cpp
   	interpreter.cpp
	)

target_link_libraries(
	interpreter_test
	gtest
	gmock
	gtest_main
	gcov
	)

add_test(
	NAME interpreter_test
	COMMAND $<TARGET_FILE:interpreter_test>
	)

