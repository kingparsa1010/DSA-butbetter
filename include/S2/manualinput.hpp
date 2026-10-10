/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#ifndef DSA_MANUALINPUT_HPP
#define DSA_MANUALINPUT_HPP

// Shared input/output helpers for the S2 "manual input" programs.

#include "S2/simplequeue.hpp"

#include <cstdio>
#include <iostream>
#include <string>

// Strips surrounding whitespace, including the '\r' left behind by Windows line endings.
inline std::string
TrimLine( std::string const &line )
{
    const char *whitespace = " \t\r\n";
    const size_t begin = line.find_first_not_of( whitespace );

    if ( begin == std::string::npos )
        return {};

    const size_t end = line.find_last_not_of( whitespace );
    return line.substr( begin, end - begin + 1 );
}

// Reads the first line of input: the number of operations that follow.
// Returns false (and explains why on stderr) if it is missing or not a positive integer.
inline bool
ReadOperationCount( long &out_count )
{
    std::string line{};
    size_t parsed{};

    if ( !std::getline( std::cin, line ) )
    {
        std::fprintf( stderr, "Expected the number of operations on the first line.\n" );
        return false;
    }

    line = TrimLine( line );

    try
    {
        out_count = std::stol( line, &parsed );
    } catch ( ... )
    {
        parsed = 0;
    }

    if ( parsed == 0 || parsed != line.size() || out_count <= 0 )
    {
        std::fprintf( stderr, "Operations must be a positive integer, got \"%s\".\n", line.c_str() );
        return false;
    }

    return true;
}

// Splits "OPERATION rest of the line" into the operation and its (possibly empty) value.
inline void
SplitCommand( std::string const &line, std::string &out_operation, std::string &out_value )
{
    const size_t space_pos = line.find( ' ' );

    if ( space_pos == std::string::npos )
    {
        out_operation = line;
        out_value.clear();
    } else
    {
        out_operation = line.substr( 0, space_pos );
        out_value = TrimLine( line.substr( space_pos + 1 ) );
    }
}

// Parses a non-negative index. Rejects signs, spaces and trailing garbage such as "3abc".
inline bool
ParseIndex( std::string const &text, size_t &out_index )
{
    size_t value = 0;

    // Anything longer could overflow size_t, and is out of bounds anyway.
    if ( text.empty() || text.size() > 18 )
        return false;

    for ( char c : text )
    {
        if ( c < '0' || c > '9' )
            return false;

        value = value * 10 + static_cast<size_t>( c - '0' );
    }

    out_index = value;
    return true;
}

// Prints every buffered result, one per line. Does nothing if there are none.
inline void
PrintResults( SimpleQueue<std::string> &results )
{
    while ( !results.Empty() )
        std::printf( "%s\n", results.Pop().c_str() );
}

#endif //DSA_MANUALINPUT_HPP
