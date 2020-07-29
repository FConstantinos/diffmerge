# 
# Copyright 2019-2020 Konstantinos Fragkiadakis
# All rights reserved.
# 

cmake_minimum_required (VERSION 3.10)
include(GNUInstallDirs)

project (diffmerge)

option(ENABLE_TEST "Build tests" OFF)
option(ENABLE_DOC  "Create and install the HTML based documentation (requires Doxygen)" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

########################################################################
## Targets
########################################################################

add_library(lcs SHARED
	include/lcs.h
	)

target_include_directories(lcs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    
install(TARGETS lcs
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

set_target_properties(lcs
    PROPERTIES LINKER_LANGUAGE CXX #required because diff.h file is not recognized as CXX by cmake
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )

add_executable(diff
	src/diffmain.cc
	)
	
target_link_libraries(diff lcs stdc++fs)

install(TARGETS diff
    RUNTIME
		DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

set_target_properties(diff PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    )
########################################################################
## Sub-directory Targets
########################################################################

if (ENABLE_TEST)
    add_subdirectory(unittests)
endif(ENABLE_TEST)
