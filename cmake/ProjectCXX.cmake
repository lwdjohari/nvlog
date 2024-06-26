# The MIT License (MIT)
#
# Copyright (c) 2020 Linggawasistha Djohari
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ProjectCXX.cmake - Collection of CMAKE function for handling CXX project
# 
# This module defines functions
# NV_DETECT_IS_ROOT_PROJECT : Detect is the current cmake is root project or not
#   Parameters: RESULT_VAR, return Bool TRUE if root project
#
# NV_PROPOGATE_CXX_OPTIONS : Check and propogate the current CXX options
#   Parameters: CXX_STANDARD            when on root project will use the parameter as CMAKE_CXX_STANDARD 
#                                       otherwise will propogate from parent
#               CXX_STANDARD_REQUIRED   when on root project will use the parameter as CXX_STANDARD_REQUIRED 
#                                       otherwise will propogate from parent
#               CXX_EXTENSIONS          when on root project will use the parameter as CXX_EXTENSIONS 
#                                       otherwise will propogate from parent
#
# Linggawasistha Djohari <linggawasistha.djohari@outlook.com>, 2020

# Function detect is root project
function(NV_DETECT_IS_ROOT_PROJECT RESULT_VAR)
  if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
      set(${RESULT_VAR} TRUE PARENT_SCOPE)
  else()
      set(${RESULT_VAR} FALSE PARENT_SCOPE)
  endif()
endfunction()

# Function to propagate C++ options from the root
function(NV_PROPOGATE_CXX_OPTIONS CXX_STANDARD CXX_STANDARD_REQUIRED CXX_EXTENSIONS)
  message(STATUS "Set Project CXX Options")
  set(IS_ROOT FALSE)
  NV_DETECT_IS_ROOT_PROJECT(IS_ROOT)

  

  if(IS_ROOT)
      set(CMAKE_CXX_STANDARD ${CXX_STANDARD} CACHE STRING "C++ standard")
      set(CMAKE_CXX_STANDARD_REQUIRED ${CXX_STANDARD_REQUIRED} CACHE BOOL "C++ standard required")
      set(CMAKE_CXX_EXTENSIONS ${CXX_EXTENSIONS} CACHE BOOL "Enable C++ extensions")

      message(STATUS "C++ Standard: ${CXX_STANDARD}")
      message(STATUS "C++ Standard Required: ${CXX_STANDARD_REQUIRED}")
      message(STATUS "C++ Extensions: ${CXX_EXTENSIONS}")
  else()
      # Propagate C++ options if not already set
      
      if(DEFINED CMAKE_CXX_STANDARD)
        message(STATUS "CMAKE_CXX_STANDARD is already defined: ${CMAKE_CXX_STANDARD}")
      else()
        message(STATUS "CMAKE_CXX_STANDARD is not defined.")
      endif()
      if(DEFINED CMAKE_CXX_STANDARD_REQUIRED)
        message(STATUS "CMAKE_CXX_STANDARD_REQUIRED is already defined: ${CMAKE_CXX_STANDARD_REQUIRED}")
      else()
        message(STATUS "CMAKE_CXX_STANDARD_REQUIRED is not defined.")
      endif()
      if(DEFINED CMAKE_CXX_EXTENSIONS)
        message(STATUS "CMAKE_CXX_EXTENSIONS is already defined: ${CMAKE_CXX_EXTENSIONS}")
      else()
        message(STATUS "CMAKE_CXX_EXTENSIONS is not defined :${CMAKE_CXX_EXTENSIONS}")
      endif()

      if (NOT DEFINED CMAKE_CXX_STANDARD)
          set(CMAKE_CXX_STANDARD ${CXX_STANDARD}  "C++ standard")
          message(STATUS "Propagating C++ Standard: ${CXX_STANDARD}")
      else()
        set(CXX_STANDARD ${CMAKE_CXX_STANDARD})
        message(STATUS "Propagating C++ Standard from parent: ${CXX_STANDARD}")
      endif()

      if (NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
          set(CMAKE_CXX_STANDARD_REQUIRED ${CXX_STANDARD_REQUIRED} "C++ standard required")
          message(STATUS "Propagating C++ Standard Required: ${CXX_STANDARD_REQUIRED}")
      else()
          set(CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED})
          message(STATUS "Propagating C++ Standard Required from parent: ${CMAKE_CXX_STANDARD_REQUIRED}")
      endif()

      if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
          set(CMAKE_CXX_EXTENSIONS ${CXX_EXTENSIONS} "Enable C++ extensions")
          message(STATUS "Propagating C++ Extensions: ${CXX_EXTENSIONS}")
      else()
          set(CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS})
          message(STATUS "Propagating C++ Extensions from parent: ${CXX_EXTENSIONS}")
      endif()
  endif()
endfunction()

# Function to return the cxx_std_xx compile feature based on the C++ standard version
function(NV_GET_CXX_STD_FEATURE CXX_STANDARD OUTPUT_VAR)
    if(CXX_STANDARD EQUAL 98)
        set(${OUTPUT_VAR} "cxx_std_98" PARENT_SCOPE)
    elseif(CXX_STANDARD EQUAL 11)
        set(${OUTPUT_VAR} "cxx_std_11" PARENT_SCOPE)
    elseif(CXX_STANDARD EQUAL 14)
        set(${OUTPUT_VAR} "cxx_std_14" PARENT_SCOPE)
    elseif(CXX_STANDARD EQUAL 17)
        set(${OUTPUT_VAR} "cxx_std_17" PARENT_SCOPE)
    elseif(CXX_STANDARD EQUAL 20)
        set(${OUTPUT_VAR} "cxx_std_20" PARENT_SCOPE)
        elseif(CXX_STANDARD EQUAL 23)
        set(${OUTPUT_VAR} "cxx_std_23" PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Unsupported C++ standard version: ${CXX_STANDARD}")
    endif()
endfunction()

function(NV_GET_CXX_VERSION OUTPUT_VAR)
  if(CMAKE_CXX_STANDARD EQUAL 14)
    set(${OUTPUT_VAR} 14 PARENT_SCOPE)
  elseif(CMAKE_CXX_STANDARD EQUAL 17)
    set(${OUTPUT_VAR} 17 PARENT_SCOPE)
  elseif(CMAKE_CXX_STANDARD GREATER OR EQUAL 20)
    set(${OUTPUT_VAR} 20 PARENT_SCOPE)
  else()
  message(WARNING "Unknown or unsupported C++ standard: ${CMAKE_CXX_STANDARD}")
  endif()
endfunction()

function(NV_COMPILE_MODE IS_DEBUG)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(IS_DEBUG ON)
else()
  set(IS_DEBUG OFF)
endif()
endfunction()

function(NV_COMPILER)
    # Initialize variables
    set(NV_C_COMPILER ${CMAKE_C_COMPILER} CACHE STRING "Detected C compiler" FORCE)
    set(NV_CXX_COMPILER ${CMAKE_CXX_COMPILER} CACHE STRING "Detected C++ compiler" FORCE)

    if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
        # For MSVC, use the MSVC compiler id
        set(NV_COMPILER_KEY "msvc" CACHE STRING "Detected MSVC compiler" FORCE)
    else()
        # For other compilers, use -dumpmachine to extract the key
        execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpmachine
                        OUTPUT_VARIABLE NV_COMP_KEY
                        OUTPUT_STRIP_TRAILING_WHITESPACE)

        # Extract the relevant part from the compiler key
        string(REGEX MATCH "mingw32|linux-gnu|darwin|cygwin|freebsd" NV_COMPILER_KEY ${NV_COMP_KEY})

        # Check if the compiler key was correctly extracted
        if (NV_COMPILER_KEY STREQUAL "")
            set(NV_COMPILER_KEY "unknown" CACHE STRING "Detected unknown compiler" FORCE)
        else()
            set(NV_COMPILER_KEY ${NV_COMPILER_KEY} CACHE STRING "Detected compiler key" FORCE)
        endif()
    endif()

    option(NV_MINGW "Is Compiled using mingw" OFF)
    if(${NV_COMPILER_KEY} STREQUAL "mingw32")
      message(STATUS "MINGW: ON")
      set(NV_MINGW ON CACHE BOOL "" FORCE)
    endif()

    # Display the detected values
    message(STATUS "Detected C compiler: ${NV_C_COMPILER}")
    message(STATUS "Detected C++ compiler: ${NV_CXX_COMPILER}")
    message(STATUS "Compiler key: ${NV_COMPILER_KEY}")

endfunction()

function (NV_SET_DIST_DIR PROJ_NAME DIR PFID )
set(NV_PROJ_IS_DEBUG ON)
NV_COMPILE_MODE(${NV_PROJ_IS_DEBUG})
if(NV_PROJ_IS_DEBUG)
  set(NV_PROJ_COMP_MODE "debug" CACHE STRING "" FORCE)
else()
  set(NV_PROJ_COMP_MODE "rel" CACHE STRING "" FORCE)
endif()

set_target_properties(${PROJ_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${DIR}/dist/${NV_PROJ_COMP_MODE}/${PFID}
    LIBRARY_OUTPUT_DIRECTORY ${DIR}/dist/${NV_PROJ_COMP_MODE}/${PFID}
    RUNTIME_OUTPUT_DIRECTORY ${DIR}/dist/${NV_PROJ_COMP_MODE}/${PFID}
)

message(STATUS "Dist DIR: ${DIR}/${NV_PROJ_COMP_MODE}/${PFID}")

endfunction()