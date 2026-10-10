/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#ifndef DSA_SIMPLEDEQUE_HPP
#define DSA_SIMPLEDEQUE_HPP

#include "S1/simplearray.hpp"

template <typename T>
class SimpleDeque
    : public SimpleArray<T>
{
public:
    SimpleDeque() = default;
    ~SimpleDeque() = default;

    void
    PushFront( T const &value )
    {
        this->Insert( 0, value );
    }

    void
    PushBack( T const &value )
    {
        this->Insert( this->Size(), value );
    }

    T
    PopFront()
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

        T value = this->Get( 0 );
        this->Remove( 0 );
        return value;
    }

    T
    PopBack()
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

        T value = this->Get( this->Size() - 1 );
        this->Remove( this->Size() - 1 );
        return value;
    }

    T
    Front() const
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

        return this->Get( 0 );
    }

    T
    Back() const
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

        return this->Get( this->Size() - 1 );
    }
};

#endif //DSA_SIMPLEDEQUE_HPP