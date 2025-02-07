cmake_minimum_required(VERSION 3.12.0)
project(codepage VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.c resource.rc)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    # 静态编译，去除调试信息
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE
            -ffunction-sections
            -fdata-sections
        )
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "-flto -Wl,--gc-sections"
        )
        set(CMAKE_EXE_LINKER_FLAGS "-s -static")
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${PROJECT_NAME} PRIVATE /MT)
    endif()
endif()