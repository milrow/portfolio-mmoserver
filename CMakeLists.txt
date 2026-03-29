cmake_minimum_required(VERSION 3.20)
project(MMORPGServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)

link_directories(${PROTOBUF_ROOT}/lib)

file(GLOB_RECURSE SOURCES 
    "Src/*.cpp" 
    "Src/*.h" 
    "Src/*.cc"
)


add_executable(${PROJECT_NAME} ${SOURCES})


target_link_libraries(${PROJECT_NAME} PRIVATE 
protobuf::libprotobuf 
odbc32.lib
)


target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Protobuf/Generated_CPP
)