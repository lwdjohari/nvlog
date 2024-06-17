cmake_minimum_required(VERSION 3.10)
project(nvlog CXX)


message(STATUS "NvLog : Configure")
message(STATUS "-----------------------")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})


include(ProjectCXX)
set(ISROOT FALSE)
set(CXX_FEATURE "")
NV_DETECT_IS_ROOT_PROJECT(ISROOT)

message(STATUS "NvServ Root Project: ${ISROOT} ${NVSERV_CXX_VERSION}")

if(ISROOT)
    if(NOT NVSERV_CXX_VERSION)
    set(NVSERV_CXX_VERSION 17)
    endif()
    option(NVSERV_CXX_STANDARD_REQUIRED "CXX Required" ON)
    option(NVSERV_CXX_EXTENSIONS "CXX Extensions" ON)
    NV_PROPOGATE_CXX_OPTIONS(${NVSERV_CXX_VERSION} ${NVSERV_CXX_STANDARD_REQUIRED} ${NVSERV_CXX_EXTENSIONS})
else()
    set(NVSERV_CXX_VERSION ${CMAKE_CXX_STANDARD})
endif()

NV_GET_CXX_STD_FEATURE(${NVSERV_CXX_VERSION} CXX_FEATURE)
message(STATUS "CXX Feature: ${CXX_FEATURE}")

#Main headers and sources
file(GLOB_RECURSE SOURCES_NVLOG CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.s
)

add_library(${PROJECT_NAME} STATIC ${SOURCES_NVLOG} )
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME} PUBLIC ${CXX_FEATURE})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )

add_subdirectory(tests build-nvlog-test)

add_executable(${PROJECT_NAME}_runner main.cc)
target_link_libraries(${PROJECT_NAME}_runner PUBLIC nvlog::nvlog)
set_target_properties(${PROJECT_NAME}_runner PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(${PROJECT_NAME}_runner PUBLIC ${CXX_FEATURE})
# target_include_directories(${PROJECT_NAME}_test
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/src/
# )
