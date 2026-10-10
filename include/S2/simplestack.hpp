/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#ifndef DSA_SIMPLESTACK_HPP
#define DSA_SIMPLESTACK_HPP

#include "S1/simplearray.hpp"

// The stack picks its "top" end based on the backing list, so one of these must be set.
// Without it Push() would silently do nothing and Pop() would never remove anything.
#if !defined( LINKED_LISTS ) && !defined( CONTIGUOUS_LISTS )
#error "SimpleStack requires LINKED_LISTS or CONTIGUOUS_LISTS to be defined."
#endif

template <typename T>
class SimpleStack :
    public SimpleArray<T>
{
public:
    SimpleStack() = default;
    ~SimpleStack() = default;

    T
    Pop()
    {
        T value{};

        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );

#ifdef LINKED_LISTS
        // Top operations are faster
        value = this->Get( 0 );
        this->Remove( 0 );
#elif defined( CONTIGUOUS_LISTS )
        // Bottom operations are faster
        value = this->Get( this->Size() - 1 );
        this->Remove( this->Size() - 1 );
#endif
        return value;
    }

    void
    Push( T const &value )
    {
#ifdef LINKED_LISTS
        // Top operations are faster
        *this << value;
#elif defined( CONTIGUOUS_LISTS )
        // Bottom operations are faster
        this->Insert( this->Size(), value );
#endif
    }

    T
    Top() const
    {
        if ( this->Empty() )
            throw Exception( STACK_UNDERFLOW );
#ifdef LINKED_LISTS
        return this->Get( 0 );
#elif defined( CONTIGUOUS_LISTS )
        return this->Get( this->Size() - 1 );
#endif
    }
};

#endif //DSA_SIMPLESTACK_HPP
