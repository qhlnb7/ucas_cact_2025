cmake_minimum_required(VERSION 3.21)
project(CACT_Compiler)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include include/util
        include/frontend include/mir include/backend
)

# Explicitly specify source files to include backend
file(GLOB UTIL_FILES
        include/util/*.h src/util/*.cpp
        )
file(GLOB FRONTEND_FILES
        include/frontend/*.h src/frontend/*.cpp
        )
file(GLOB MIR_FILES
        include/mir/*.h src/mir/*.cpp
        )
file(GLOB BACKEND_FILES
        include/backend/*.h src/backend/*.cpp
        )
file(GLOB ROOT_FILES
        include/*.h src/Compiler.cpp
        )
        
set(SOURCE_FILES
        ${UTIL_FILES}
        ${FRONTEND_FILES}
        ${MIR_FILES}
        ${BACKEND_FILES}
        ${ROOT_FILES}
)
        
add_executable(compiler ${SOURCE_FILES})