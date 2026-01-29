cmake_minimum_required(VERSION 3.27)
project(P2)

set(CMAKE_CXX_STANDARD 17)

add_executable(participant
        TCPServer.h
        TCPServer.cpp
        participant.cpp
        2PC_Participant.h
        2PC_Participant.cpp
        Protocol.h
        )

 add_executable(coordinator
         TCPClient.h
         TCPClient.cpp
         2PC_Coordinator.h
         2PC_Coordinator.cpp
         coordinator.cpp)
