/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <S1/simplearray.hpp>

// Question 2: Design a program with manual input for a simple stack.
/*
 * Simple Stack Implementation: */ #include <S2/simplestack.hpp> /*
 */
/*
 * Simple Queue Implementation: */ #include <S2/simplequeue.hpp> /*
 */
/*
 * Shared input helpers: */ #include <S2/manualinput.hpp> /*
 */

#include <cstdio>
#include <iostream>
#include <string>
#include <string_view>

enum class Operation
{
    Push,
    Top,
    Pop,
    Size,
    Empty
};

constexpr std::string_view to_string( Operation op )
{
    switch ( op )
    {
        case Operation::Push:
            return "PUSH";
        case Operation::Top:
            return "TOP";
        case Operation::Pop:
            return "POP";
        case Operation::Size:
            return "SIZE";
        case Operation::Empty:
            return "EMPTY";
    }
    return "";
}

int main()
{
    long numOperations;
    SimpleStack<std::string> stack{};
    SimpleQueue<std::string> results{};
    std::string user_input{};
    std::string operation{};
    std::string value{};

    if ( !ReadOperationCount( numOperations ) )
        return 1;

    while ( numOperations > 0 && std::getline( std::cin, user_input ) )
    {
        numOperations--;

        SplitCommand( TrimLine( user_input ), operation, value );

        if ( operation == to_string( Operation::Push ) )
        {
            stack.Push( value );
        } else if ( operation == to_string( Operation::Pop ) )
        {
            if ( stack.Empty() )
                results.Push( "EMPTY" );
            else
                results.Push( stack.Pop() );
        } else if ( operation == to_string( Operation::Top ) )
        {
            if ( stack.Empty() )
                results.Push( "EMPTY" );
            else
                results.Push( stack.Top() );
        } else if ( operation == to_string( Operation::Size ) )
        {
            results.Push( std::to_string( stack.Size() ) );
        } else if ( operation == to_string( Operation::Empty ) )
        {
            results.Push( stack.Empty() ? "YES" : "NO" );
        } else if ( !operation.empty() )
        {
            std::fprintf( stderr, "Unknown operation \"%s\" ignored.\n", operation.c_str() );
        }
    }

    PrintResults( results );

    return 0;
}
