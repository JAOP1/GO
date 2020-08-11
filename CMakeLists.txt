cmake_minimum_required(VERSION 3.1)
project(GUI_GO)
enable_language(C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_PREFIX_PATH "/home/dell/libtorch/") #Change to real path of libtorch.


find_package(SFML 2 REQUIRED network audio graphics window system)
find_package(Torch REQUIRED)


option(BUILD_VISUALIZER "Build visualizer (requires SFML and libtorch)" OFF)
option(BUILD_TESTS "Build unit tests (requires gtest)" OFF)
option(BUILD_TRAINER "Build trainer. (requires libtorch)" ON)
option(BUILD_TESTING "Build testing functions and other stuff." ON)
# --------------------------------------------------------------------------------
#                         Locate files (no change needed).
# --------------------------------------------------------------------------------
# We make sure that CMake sees all the files.

include_directories(
    ${PROJECT_SOURCE_DIR}/GUI/
    ${PROJECT_SOURCE_DIR}/GUI/Files/
    ${PROJECT_SOURCE_DIR}/Include/
    ${PROJECT_SOURCE_DIR}/Include/Extra/
    ${PROJECT_SOURCE_DIR}/Include/Extra/External/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/C_49/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/C_49/NN_classes/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/C_49/Encoders_classes/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/C_49/Include/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/C_49/Models/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/C_49/Data/
    ${PROJECT_SOURCE_DIR}/Search_Algorithms/testing/
    ${PROJECT_SOURCE_DIR}/src/
    ${PROJECT_SOURCE_DIR}/tests/
    ${PROJECT_SOURCE_DIR}/Graphs/)
    
    file(GLOB SOURCES "src/*.cpp")
    add_library(graph ${SOURCES})
    
# --------------------------------------------------------------------------------
#                            Build! (Change as needed)
# --------------------------------------------------------------------------------
    
    
if (${BUILD_VISUALIZER})

    message(STATUS "Building visualizer.")
    
    add_definitions(-DUSE_SFML)
    
    
    configure_file(${PROJECT_SOURCE_DIR}/GUI/Files/font.ttf ${CMAKE_CURRENT_BINARY_DIR}/font.ttf COPYONLY)
    configure_file(${PROJECT_SOURCE_DIR}/GUI/Files/fondo.jpg ${CMAKE_CURRENT_BINARY_DIR}/fondo.jpg COPYONLY)
    
    add_executable(visualizer GUI_main.cpp )
    add_dependencies(visualizer graph)
    
    target_link_libraries(visualizer graph   sfml-graphics sfml-window sfml-system)
    #target_link_libraries(visualizer ${TORCH_LIBRARIES})
    
    
    
endif()

# --------------------------------------------------------------------------------
#                         Make Tests (no change needed).
# --------------------------------------------------------------------------------
# Add a make target 'gtest', that runs the tests (and builds all dependencies).
# The setup of Google Test is done at the very end of this file.

if(${BUILD_TESTS})
    message(STATUS "Creating unit tests.")
    
    find_package(GTest)
    find_package(Threads REQUIRED)

    file(GLOB TESTFILES "tests/*.cpp")
    set(TEST_MAIN unit_tests.x)
    
    if (${GTEST_FOUND})
        message("Using system's google test")
        include_directories(${GTEST_INCLUDE_DIRS})
        add_executable(${TEST_MAIN} ${TESTFILES} )
    endif()
    
    target_link_libraries(${TEST_MAIN} ${GTEST_LIBRARIES} graph Threads::Threads ${TORCH_LIBRARIES})
    add_custom_target(gtest COMMAND "${PROJECT_BINARY_DIR}/${TEST_MAIN}" DEPENDS ${TEST_MAIN})
    
    # Add a standard make target 'test' that runs the tests under CTest (only as an alt. to gtest).
    include(CTest)
    enable_testing()
    add_test(unit_tests ${PROJECT_BINARY_DIR}/${TEST_MAIN})
endif()
    
    
# --------------------------------------------------------------------------------
#                              Trainer build.
# --------------------------------------------------------------------------------
    
if(${BUILD_TRAINER})
    message(STATUS "Building Trainer.")
    
    
    add_executable(trainer training_main.cpp)
    add_dependencies(trainer graph)

    target_link_libraries(trainer graph ${TORCH_LIBRARIES})
endif()

# --------------------------------------------------------------------------------
#                              Tester build.
# --------------------------------------------------------------------------------

if(${BUILD_TESTING})
    message(STATUS "Building Testing.")
    add_executable(tester testing_main.cpp)
    add_dependencies(tester graph)

    target_link_libraries(tester graph ${TORCH_LIBRARIES} )
endif()