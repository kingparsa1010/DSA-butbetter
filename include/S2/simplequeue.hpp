/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#ifndef DSA_SIMPLEQUEUE_HPP
#define DSA_SIMPLEQUEUE_HPP

#include "S1/simplearray.hpp"

template <typename T>
class SimpleQueue :
    public SimpleArray<T>
{
public:
    SimpleQueue() = default;
    ~SimpleQueue() = default;

    void
    Push( T const &value )
    {
        this->Insert( this->Size(), value );
    }

    T
    Pop()
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

        T value = this->Get( 0 );
        this->Remove( 0 );

        return value;
    }

    T
    Front() const
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

        return this->Get( 0 );
    }
};

#endif //DSA_SIMPLEQUEUE_HPP
