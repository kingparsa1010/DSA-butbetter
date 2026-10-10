/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <S1/simplearray.hpp>

// Question 3: Design a program with manual input for a simple queue.
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
    Enqueue,
    Dequeue,
    Front,
    Size,
    Empty
};

constexpr std::string_view to_string( Operation op )
{
    switch ( op )
    {
        case Operation::Enqueue:
            return "ENQUEUE";
        case Operation::Dequeue:
            return "DEQUEUE";
        case Operation::Front:
            return "FRONT";
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
    SimpleQueue<std::string> queue{};
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

        if ( operation == to_string( Operation::Enqueue ) )
        {
            queue.Push( value );
        } else if ( operation == to_string( Operation::Dequeue ) )
        {
            if ( queue.Empty() )
                results.Push( "EMPTY" );
            else
                results.Push( queue.Pop() );
        } else if ( operation == to_string( Operation::Front ) )
        {
            if ( queue.Empty() )
                results.Push( "EMPTY" );
            else
                results.Push( queue.Front() );
        } else if ( operation == to_string( Operation::Size ) )
        {
            results.Push( std::to_string( queue.Size() ) );
        } else if ( operation == to_string( Operation::Empty ) )
        {
            results.Push( queue.Empty() ? "YES" : "NO" );
        } else if ( !operation.empty() )
        {
            std::fprintf( stderr, "Unknown operation \"%s\" ignored.\n", operation.c_str() );
        }
    }

    PrintResults( results );

    return 0;
}
