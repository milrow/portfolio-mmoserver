cmake_minimum_required(VERSION 3.20)
project(MMORPGServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(flatbuffers CONFIG REQUIRED)

link_directories(${PROTOBUF_ROOT}/lib)

file(GLOB_RECURSE SOURCES 
    "Src/*.cpp" 
    "Src/*.h" 
    "Src/*.cc"
)


add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _HAS_STD_BYTE=0
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
protobuf::libprotobuf 
unofficial-sodium::sodium
odbc32.lib
flatbuffers::flatbuffers
)


target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Protobuf/Generated_CPP
)
