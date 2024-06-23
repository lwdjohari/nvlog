cmake_minimum_required(VERSION 3.10)
project(nvlog CXX)

message(STATUS "NvLog : Configure")
message(STATUS "-----------------------")

option(NVLOG_TRACE "Print trace while in DEBUG mode" OFF)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
include(ProjectCXX)

NV_COMPILER()

set(ISROOT FALSE)
set(CXX_FEATURE "")
NV_DETECT_IS_ROOT_PROJECT(ISROOT)

set (ISDEBUG ON) 
NV_COMPILE_MODE(ISDEBUG)


message(STATUS "NvLog Root Project: ${ISROOT} ${NVSERV_CXX_VERSION}")

if(ISROOT)
    if(NOT NVSERV_CXX_VERSION)
    set(NVSERV_CXX_VERSION 17)
    endif()
    option(NVSERV_CXX_STANDARD_REQUIRED "CXX Required" ON)
    option(NVSERV_CXX_EXTENSIONS "CXX Extensions" OFF)
    set(NVLOG_TRACE ON)
    NV_PROPOGATE_CXX_OPTIONS(${NVSERV_CXX_VERSION} ${NVSERV_CXX_STANDARD_REQUIRED} ${NVSERV_CXX_EXTENSIONS})
else()
    set(NVSERV_CXX_VERSION ${CMAKE_CXX_STANDARD})
endif()

set(NVLOG_FEATURE_DEFINITION)
if(ISDEBUG)
    LIST(APPEND NVLOG_FEATURE_DEFINITION NVLOG_DEBUG=1)
else()
    LIST(APPEND NVLOG_FEATURE_DEFINITION NVLOG_DEBUG=0)
endif()

if(NVLOG_TRACE)
    LIST(APPEND NVLOG_FEATURE_DEFINITION NVLOG_TRACE=1)
else()
    LIST(APPEND NVLOG_FEATURE_DEFINITION NVLOG_TRACE=0)
endif()

NV_GET_CXX_STD_FEATURE(${NVSERV_CXX_VERSION} CXX_FEATURE)
message(STATUS "CXX Feature: ${CXX_FEATURE}")

#Main headers and sources
file(GLOB_RECURSE SOURCES_NVLOG CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)

# Convert the list to a comma-separated string
string(JOIN ", " NVLOG_FEATURES_JOIN ${NVLOG_FEATURE_DEFINITION})

# Print the comma-separated list
message(STATUS "Compile Flags: ${NVLOG_FEATURES_JOIN}")


add_library(${PROJECT_NAME} STATIC ${SOURCES_NVLOG} )
if (NV_MINGW)
    message(STATUS "Compile Lib using MINGW: ON")

    target_link_libraries(${PROJECT_NAME} 
        PUBLIC 
            pthread 
            -static-libgcc 
            -static-libstdc++)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_definitions(${PROJECT_NAME} PUBLIC  ${NVLOG_FEATURE_DEFINITION})
target_compile_features(${PROJECT_NAME} PUBLIC ${CXX_FEATURE})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )

NV_SET_DIST_DIR(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR} ${NV_COMPILER_KEY})


if(NOT NV_MINGW)
    message(STATUS "NvLog Test: ON")
    add_subdirectory(tests build-nvlog-test)
else()
    message(STATUS "NvLog Test: OFF")
endif()

add_executable(${PROJECT_NAME}_runner main.cc)
if (NV_MINGW)
    message(STATUS "Compile Executable using MINGW: ON")
    target_link_libraries(${PROJECT_NAME}_runner 
        PUBLIC 
            pthread 
            -static-libgcc 
            -static-libstdc++
            nvlog::nvlog 
            )
else()
    message(STATUS "Compile Executable using MINGW: OFF")
    target_link_libraries(${PROJECT_NAME}_runner PUBLIC nvlog::nvlog)
endif()
set_target_properties(${PROJECT_NAME}_runner PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_runner PUBLIC ${CXX_FEATURE})

NV_SET_DIST_DIR(${PROJECT_NAME}_runner ${CMAKE_CURRENT_SOURCE_DIR} ${NV_COMPILER_KEY})


# target_include_directories(${PROJECT_NAME}_test
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/src/
# )
