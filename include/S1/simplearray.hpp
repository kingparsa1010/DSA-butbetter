/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#ifndef DSA_SIMPLEARRAY_HPP
#define DSA_SIMPLEARRAY_HPP

#include "../utils.h"

#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>
#include <utility>

extern "C" {
typedef struct _SimpleByteArray SimpleByteArray;

extern STATUS SimpleByteArray_init( SimpleByteArray **out_list );
extern STATUS SimpleByteArray_free( SimpleByteArray *in_list );
extern STATUS SimpleByteArray_insert( SimpleByteArray *list, size_t index, BYTE value );
extern STATUS SimpleByteArray_insert_bytes( SimpleByteArray *list, size_t index, const BYTE *values, size_t count );
extern STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed );
extern STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value );
extern STATUS SimpleByteArray_size( const SimpleByteArray *list, size_t *value );
}

template <typename T>
class SimpleArray
{
    // For byte arrays, T needs to be trivially copyable.
    // Things like interfaces, mutated classes or ref counted objects cannot be copied!
    // std::string is the one exception: it is serialized as [length][characters].
    static_assert( std::is_same_v<T, std::string> || std::is_trivially_copyable_v<T>,
                   "SimpleArray<T> requires a trivially copyable T (or std::string)." );

public:
    SimpleArray()
    {
        check_st_( SimpleByteArray_init( &m_array ) );
    }

    ~SimpleArray()
    {
        if ( m_array )
            SimpleByteArray_free( m_array );
    }

    // The array owns a C handle: a shallow copy would free it twice.
    SimpleArray( SimpleArray const & ) = delete;
    SimpleArray &operator = ( SimpleArray const & ) = delete;

    SimpleArray( SimpleArray &&other ) noexcept
        : m_array( std::exchange( other.m_array, nullptr ) )
    {
    }

    SimpleArray &operator = ( SimpleArray &&other ) noexcept
    {
        if ( this != &other )
        {
            if ( m_array )
                SimpleByteArray_free( m_array );
            m_array = std::exchange( other.m_array, nullptr );
        }
        return *this;
    }

    void
    Insert( size_t index, T const &value )
    {
        if constexpr ( std::is_same_v<T, std::string> )
        {
            const size_t byte_index = StringByteIndex( index );
            const size_t length = value.size();

            check_st_( SimpleByteArray_insert_bytes( m_array, byte_index, reinterpret_cast<const BYTE*>(&length), sizeof(length) ) );

            if ( length != 0 )
                check_st_( SimpleByteArray_insert_bytes( m_array, byte_index + sizeof(length), reinterpret_cast<const BYTE*>(value.data()), length ) );
        }
        else
            check_st_( SimpleByteArray_insert_bytes( m_array, index * sizeof(T), reinterpret_cast<const BYTE*>(&value), sizeof(T) ) );
    }

    void
    Remove( size_t index )
    {
        size_t byte_index;
        size_t iterator;

        if constexpr ( std::is_same_v<T, std::string> )
        {
            size_t length{};
            size_t total_size;

            byte_index = StringByteIndex( index );
            GetBytes( byte_index, &length, sizeof(length) );
            total_size = sizeof(length) + length;

            for ( iterator = 0; iterator < total_size; iterator++ )
                check_st_( SimpleByteArray_remove( m_array, byte_index, nullptr ) );
        }
        else
        {
            byte_index = index * sizeof(T);

            for ( iterator = 0; iterator < sizeof(T); iterator++ )
                check_st_( SimpleByteArray_remove( m_array, byte_index, nullptr ) );
        }
    }

    T
    Get( size_t index ) const
    {
        T value{};
        size_t iterator;
        size_t byte_index;
        BYTE* bytes;

        if constexpr ( std::is_same_v<T, std::string> )
        {
            size_t length{};

            byte_index = StringByteIndex( index );
            GetBytes( byte_index, &length, sizeof(length) );

            value = std::string(length, '\0');

            if ( length != 0 )
                GetBytes( byte_index + sizeof(length), value.data(), length);

            return value;
        }
        else
        {
            bytes = reinterpret_cast<BYTE*>(&value);
            byte_index = index * sizeof(T);

            for ( iterator = 0; iterator < sizeof(T); iterator++ )
                check_st_( SimpleByteArray_get( m_array, byte_index + iterator, &bytes[iterator] ) );

            return value;
        }
    }

    // Replaces the element at index. The index is validated first, so a bad index never loses data.
    void
    Set( size_t index, T const &value )
    {
        if ( index >= Size() )
            throw Exception( BOUNDS );

        Remove( index );
        Insert( index, value );
    }

    size_t
    Find( T const &value ) const
    {
        size_t iterator;

        for ( iterator = 0; iterator < Size(); iterator++ )
        {
            if constexpr ( std::is_same_v<T, std::string> )
            {
                if ( Get( iterator ) == value )
                    return iterator;
            }
            else
            {
                T current = Get( iterator );

                if ( std::memcmp( &current, &value, sizeof(T) ) == 0 )
                    return iterator;
            }
        }

        throw Exception( BOUNDS );
    }

    size_t
    Size() const
    {
        size_t byte_size{};

        if constexpr ( std::is_same_v<T, std::string> )
        {
            size_t count = 0;
            size_t byte_index = 0;
            size_t length{};
            check_st_( SimpleByteArray_size( m_array, &byte_size ) );

            while ( byte_index < byte_size )
            {
                GetBytes( byte_index, &length, sizeof(length) );
                byte_index += sizeof(length) + length;
                ++count;
            }
            return count;
        }
        else
        {
            check_st_( SimpleByteArray_size( m_array, &byte_size ) );
            return byte_size / sizeof(T);
        }
    }

    bool
    Empty() const
    {
        size_t byte_size{};

        check_st_( SimpleByteArray_size( m_array, &byte_size ) );
        return byte_size == 0;
    }

    // push front
    void operator << ( T const &value )
    {
        Insert( 0, value );
    }

    T operator [] ( size_t index ) const
    {
        return Get( index );
    }

private:
    void
    GetBytes( size_t byte_index, void* destination, size_t size ) const
    {
        BYTE* bytes = static_cast<BYTE*>(destination);
        size_t iterator;

        for ( iterator = 0; iterator < size; iterator++ )
            check_st_( SimpleByteArray_get( m_array, byte_index + iterator, &bytes[iterator] ) );
    }

    size_t
    StringByteIndex( size_t index ) const
    {
        size_t current_index = 0;
        size_t byte_index = 0;
        size_t byte_size{};
        size_t length{};

        check_st_( SimpleByteArray_size( m_array, &byte_size ) );
        while ( current_index < index )
        {
            if ( byte_index >= byte_size )
                throw Exception( BOUNDS );

            GetBytes( byte_index, &length, sizeof(length) );

            byte_index += sizeof(length) + length;
            ++current_index;
        }

        return byte_index;
    }

    SimpleByteArray *m_array{ nullptr };
};


#endif
