cmake_minimum_required(VERSION 3.2)

project(GUI_GO)
enable_language(C CXX)

#find_package(Torch REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

option(BUILD_VISUALIZER "Build visualizer (requires SFML)" ON)
option(BUILD_TESTS "Build unit tests (requires gtest)" ON)

# CLI11 settings
add_library(CLI11::CLI11 IMPORTED INTERFACE)
target_include_directories(CLI11::CLI11 INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(CLI11::CLI11 INTERFACE cxx_std_11)

# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------
# We make sure that CMake sees all the files.
include_directories(
    ${PROJECT_SOURCE_DIR}/GUI/
    ${PROJECT_SOURCE_DIR}/GUI/Include
    ${PROJECT_SOURCE_DIR}/GUI/Include/Extra
    ${PROJECT_SOURCE_DIR}/GUI/Include/Search_Algorithms
    ${PROJECT_SOURCE_DIR}/GUI/Files
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/tests)

file(GLOB SOURCES "src/*.cpp")
add_library(graph ${SOURCES})

# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------


if (${BUILD_VISUALIZER})
    add_definitions(-DUSE_SFML)
    

    configure_file(${PROJECT_SOURCE_DIR}/GUI/Files/font.ttf ${CMAKE_CURRENT_BINARY_DIR}/font.ttf COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/GUI/Files/fondo.jpg ${CMAKE_CURRENT_BINARY_DIR}/fondo.jpg COPYONLY)
    
    #file(GLOB SOURCES "src/*.cpp")

    add_executable(visualizer main.cpp )
    add_dependencies(visualizer graph)
    #target_link_libraries(visualizer graph sfml-window sfml-graphics sfml-system CLI11::CLI11 "${TORCH_LIBRARIES}")
    target_link_libraries(visualizer graph sfml-window sfml-graphics sfml-system CLI11::CLI11)

endif()

# --------------------------------------------------------------------------------
#                         Make Tests (no change needed).
# --------------------------------------------------------------------------------
# Add a make target 'gtest', that runs the tests (and builds all dependencies).
# The setup of Google Test is done at the very end of this file.

if(${BUILD_TESTS})
    find_package(GTest)
    find_package(Threads REQUIRED)
    file(GLOB TESTFILES "tests/*.cpp")
    set(TEST_MAIN unit_tests.x)
	if (${GTEST_FOUND})
        message("Using system's google test")
        include_directories(${GTEST_INCLUDE_DIRS})
        add_executable(${TEST_MAIN} ${TESTFILES} )
	endif()
    
    target_link_libraries(${TEST_MAIN} ${GTEST_LIBRARIES} graph Threads::Threads)
    add_custom_target(gtest COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}" DEPENDS ${TEST_MAIN})

    # Add a standard make target 'test' that runs the tests under CTest (only as an alt. to gtest).
    include(CTest)
    enable_testing()
    add_test(unit_tests ${PROJECT_BINARY_DIR}/${TEST_MAIN})
endif()
