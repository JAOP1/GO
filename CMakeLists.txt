cmake_minimum_required(VERSION 3.2)

project(GUI_GO)
enable_language(C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

option(BUILD_VISUALIZER "Build visualizer (requires SFML)" ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/GUI/
    ${PROJECT_SOURCE_DIR}/GUI/Include
    ${PROJECT_SOURCE_DIR}/GUI/Include/Extra)




if (${BUILD_VISUALIZER})
    add_definitions(-DUSE_SFML)
    
    include_directories(GUI/)
    include_directories(GUI/Include)
    include_directories(GUI/Include/Extra)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/font.ttf ${CMAKE_CURRENT_BINARY_DIR}/font.ttf COPYONLY)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/fondo.jpg ${CMAKE_CURRENT_BINARY_DIR}/fondo.jpg COPYONLY)
    
    #file(GLOB SOURCES "src/*.cpp")

    add_executable(visualizer main.cpp )
    
    target_link_libraries(visualizer sfml-window sfml-graphics sfml-system)
    
endif()
