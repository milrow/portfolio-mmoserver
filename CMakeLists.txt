cmake_minimum_required(VERSION 3.20)
project(MMORPGServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)

link_directories(${PROTOBUF_ROOT}/lib)

set(SOURCE_FILES
Src/MMORPGServer.cpp
Src/JobManager.cpp
Src/JobManager.h
Src/JobQueue.cpp
Src/PacketHandler.cpp
Src/PacketHandler.h
Src/RecvBuffer.cpp
Src/RecvBuffer.h
Src/SendBuffer.cpp
Src/SendBuffer.h
Src/Session.cpp
Src/Session.h
Src/SessionManager.cpp
Src/SessionManager.h
Src/Protocol.pb.cc
Src/Protocol.pb.h
)


add_executable(${PROJECT_NAME} ${SOURCE_FILES})


target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf)


target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}/Src
    ${CMAKE_SOURCE_DIR}/Protobuf/Generated_CPP
)