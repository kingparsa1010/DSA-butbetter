/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <S1/simplearray.hpp>

// Question 4: Design a program with manual input for a simple list.
/*
 * Simple Queue Implementation: */ #include <S2/simplequeue.hpp> /*
 */
/*
 * Shared input helpers: */ #include <S2/manualinput.hpp> /*
 */

#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

enum class Operation
{
    Append,
    Insert,
    Delete,
    Get,
    Set,
    Find,
    Size,
    Print
};

constexpr std::string_view to_string( Operation op )
{
    switch ( op )
    {
        case Operation::Append:
            return "APPEND";
        case Operation::Insert:
            return "INSERT";
        case Operation::Delete:
            return "DELETE";
        case Operation::Get:
            return "GET";
        case Operation::Set:
            return "SET";
        case Operation::Find:
            return "FIND";
        case Operation::Size:
            return "SIZE";
        case Operation::Print:
            return "PRINT";
    }
    return "";
}

int main()
{
    long numOperations;
    size_t index;
    SimpleArray<std::string> list{};
    SimpleQueue<std::string> results{};
    std::string user_input{};
    std::string current_token;

    if ( !ReadOperationCount( numOperations ) )
        return 1;

    while ( numOperations > 0 && std::getline( std::cin, user_input ) )
    {
        SimpleArray<std::string> command{};
        std::istringstream user_input_stream( user_input );

        numOperations--;

        while ( user_input_stream >> current_token )
            command.Insert( command.Size(), current_token );

        const size_t argc = command.Size();

        if ( argc == 0 )
        {
            //malformed command
            continue;
        }

        const std::string operation = command[0];

        if ( operation == to_string( Operation::Append ) )
        {
            if ( argc < 2 )
                results.Push( "ERROR" );
            else
                list.Insert( list.Size(), command[1] );
        } else if ( operation == to_string( Operation::Insert ) )
        {
            if ( argc < 3 || !ParseIndex( command[1], index ) || index > list.Size() )
                results.Push( "ERROR" );
            else
                list.Insert( index, command[2] );
        } else if ( operation == to_string( Operation::Delete ) )
        {
            if ( argc < 2 || !ParseIndex( command[1], index ) || index >= list.Size() )
                results.Push( "ERROR" );
            else
            {
                results.Push( list.Get( index ) );
                list.Remove( index );
            }
        } else if ( operation == to_string( Operation::Get ) )
        {
            if ( argc < 2 || !ParseIndex( command[1], index ) || index >= list.Size() )
                results.Push( "ERROR" );
            else
                results.Push( list.Get( index ) );
        } else if ( operation == to_string( Operation::Set ) )
        {
            // Validate before touching the list, so a bad SET never deletes the old value.
            if ( argc < 3 || !ParseIndex( command[1], index ) || index >= list.Size() )
                results.Push( "ERROR" );
            else
                list.Set( index, command[2] );
        } else if ( operation == to_string( Operation::Find ) )
        {
            if ( argc < 2 )
                results.Push( "ERROR" );
            else
            {
                try
                {
                    results.Push( std::to_string( list.Find( command[1] ) ) );
                } catch ( Exception & )
                {
                    results.Push( "-1" );
                }
            }
        } else if ( operation == to_string( Operation::Size ) )
        {
            results.Push( std::to_string( list.Size() ) );
        } else if ( operation == to_string( Operation::Print ) )
        {
            std::string output{};
            const size_t size = list.Size();

            if ( size == 0 )
                output = "EMPTY";
            else
                for ( index = 0; index < size; index++ )
                {
                    if ( index != 0 )
                        output += ' ';
                    output += list[index];
                }
            results.Push( output );
        } else
        {
            std::fprintf( stderr, "Unknown operation \"%s\" ignored.\n", operation.c_str() );
        }
    }

    PrintResults( results );

    return 0;
}
