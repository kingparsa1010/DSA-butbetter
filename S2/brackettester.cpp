/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <S1/simplearray.hpp>

// Question 1: Design a simple bracket tester that tests nested brackets.
/*
 * Simple Stack Implementation: */ #include <S2/simplestack.hpp> /*
 */

#include <cstdio>
#include <iostream>
#include <string>

// ---- brackettester.cpp ---- //
int main()
{
    std::string user_input{};
    enum class Nest {
        Bracket,
        Parentheses,
        Brace
    };
    SimpleStack<Nest> nest{};

    // Read the whole line, so input with spaces like "( [ ] )" is tested in full.
    std::getline( std::cin, user_input );

    for ( char value : user_input )
    {
        Nest expected;

        switch ( value )
        {
            case '(':
                nest.Push( Nest::Parentheses );
                continue;
            case '{':
                nest.Push( Nest::Brace );
                continue;
            case '[':
                nest.Push( Nest::Bracket );
                continue;
            case ')':
                expected = Nest::Parentheses;
                break;
            case '}':
                expected = Nest::Brace;
                break;
            case ']':
                expected = Nest::Bracket;
                break;
            default:
                // Not a bracket, ignore it.
                continue;
        }

        // A closing bracket must match the most recent unclosed opening one.
        if ( nest.Empty() || nest.Pop() != expected )
        {
            std::printf("NO\n");
            return 0;
        }
    }

    std::printf( nest.Empty() ? "YES\n" : "NO\n" );

    return 0;
}
