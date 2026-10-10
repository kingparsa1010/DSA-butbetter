cmake_minimum_required(VERSION 3.21) # C23 needs 3.21+

project(DSA LANGUAGES C CXX)

set( CMAKE_C_STANDARD 23 )
set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_C_STANDARD_REQUIRED ON )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_EXTENSIONS OFF )

option( DSA_WARNINGS "Build with compiler warnings enabled" ON )

if ( DSA_WARNINGS )
    if ( MSVC )
        add_compile_options( /W4 )
    else ()
        add_compile_options( -Wall -Wextra )
    endif ()
endif ()

include_directories( ${PROJECT_SOURCE_DIR}/include )

# Run the tests with `ctest --test-dir build` after building.
enable_testing()

# dsa_add_io_test( <name> <target> <input file> <expected output file> )
# Feeds <input file> to <target> on stdin and compares stdout with <expected output file>.
function( dsa_add_io_test name target input expected )
    add_test( NAME ${name}
              COMMAND ${CMAKE_COMMAND}
                  -DEXE=$<TARGET_FILE:${target}>
                  -DINPUT=${input}
                  -DEXPECTED=${expected}
                  -P ${PROJECT_SOURCE_DIR}/cmake/RunIOTest.cmake )
endfunction()

add_subdirectory( S1 )
add_subdirectory( S2 )
