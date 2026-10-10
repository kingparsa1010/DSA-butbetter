/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Question 1: Implement a dynamic array with insertion and deletion.
// Create a List type that stores elements in a dynamically allocated contiguous array.

/* WARNING: THE FOLLOWING IMPLEMENTATION IS NOT THREAD SAFE!!! */

typedef struct _SimpleByteArray
{
// PRIVATE:
    PBYTE array;
    size_t size;
    size_t capacity;
} SimpleByteArray;

STATUS SimpleByteArray_init( SimpleByteArray **out_list );
STATUS SimpleByteArray_free( SimpleByteArray *in_list );
STATUS SimpleByteArray_insert( SimpleByteArray *list, size_t index, BYTE value );
STATUS SimpleByteArray_insert_bytes( SimpleByteArray *list, size_t index, const BYTE *values, size_t count );
STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed );
STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value );
STATUS SimpleByteArray_size( const SimpleByteArray *list, size_t *value );

/* initialization */
STATUS SimpleByteArray_init( SimpleByteArray **out_list )
{
    SimpleByteArray *newList = NULL;

    if ( !out_list )
    {
        fprintf(stderr, "SimpleByteArray_init: out_list is nullptr!\n");
        return POINTER;
    }

    newList = (SimpleByteArray *)calloc( 1, sizeof(SimpleByteArray) );
    if ( !newList )
    {
        fprintf(stderr, "SimpleByteArray_init: failed to allocate list.\n");
        return OUTOFMEMORY;
    }

    newList->array = NULL;
    newList->size = 0;
    newList->capacity = 1;

    // sizeof( *list->array ) is mainly for re-using code later on.
    newList->array = (PBYTE)malloc( newList->capacity * sizeof( *newList->array ) );
    if ( !newList->array )
    {
        fprintf(stderr, "SimpleByteArray_init: failed to allocate list array!\n");
        free( newList );
        return OUTOFMEMORY;
    }

    *out_list = newList;

    return SUCCESS;
}

STATUS SimpleByteArray_free( SimpleByteArray *in_list )
{
    if ( !in_list )
    {
        fprintf(stderr, "SimpleByteArray_free: in_list is nullptr!\n");
        return POINTER;
    }

    // capacity = 0 -> list has been freed already!
    if ( in_list->capacity == 0 )
    {
        fprintf(stderr, "SimpleByteArray_free: in_list has already been freed!\n");
        return ILLEGAL_METHOD_CALL;
    }

    in_list->size = 0;
    in_list->capacity = 0;
    free( (void *)in_list->array );
    in_list->array = NULL;

    free( in_list );

    return SUCCESS;
}

/* insertion, and removal */
STATUS SimpleByteArray_insert( SimpleByteArray *list, size_t index, BYTE value )
{
    PBYTE tmpArray = NULL;
    size_t iterator;

    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_insert: list is nullptr!\n");
        return POINTER;
    }

    if ( !list->capacity )
    {
        fprintf(stderr, "SimpleByteArray_insert: list has already been freed!\n");
        return ILLEGAL_METHOD_CALL;
    }

    if ( list->size < index )
    {
        fprintf(stderr, "SimpleByteArray_insert: out of bounds insertion!\n");
        return BOUNDS;
    }

    if ( list->size == list->capacity )
    {
        // grow by 2x
        tmpArray = (PBYTE)realloc( (void *)list->array, (list->capacity * 2) * sizeof(*list->array) );
        if ( !tmpArray )
        {
            fprintf(stderr, "SimpleByteArray_insert: failed to grow list array!\n");
            return OUTOFMEMORY;
        }
        list->array = tmpArray;
        list->capacity *= 2;
    }
    list->size += 1;

    // shift elements to the right hand side of index
    for ( iterator = list->size - 1; iterator > index; iterator-- )
        list->array[iterator] = list->array[iterator - 1];

    list->array[index] = value;

    return SUCCESS;
}

STATUS SimpleByteArray_insert_bytes( SimpleByteArray *list, size_t index, const BYTE *values, size_t count )
{
    PBYTE tmpArray = NULL;
    size_t newCapacity;

    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_insert_bytes: list is nullptr!\n");
        return POINTER;
    }

    if ( !values )
    {
        fprintf(stderr, "SimpleByteArray_insert_bytes: values is nullptr!\n");
        return POINTER;
    }

    if ( !count )
        return SUCCESS;

    if ( !list->capacity )
    {
        fprintf(stderr, "SimpleByteArray_insert_bytes: list has already been freed!\n");
        return ILLEGAL_METHOD_CALL;
    }

    if ( list->size < index )
    {
        fprintf(stderr, "SimpleByteArray_insert_bytes: out of bounds insertion!\n");
        return BOUNDS;
    }

    // grow by 2x until everything fits, with a single reallocation
    newCapacity = list->capacity;
    while ( newCapacity - list->size < count )
        newCapacity *= 2;

    if ( newCapacity != list->capacity )
    {
        tmpArray = (PBYTE)realloc( (void *)list->array, newCapacity * sizeof(*list->array) );
        if ( !tmpArray )
        {
            fprintf(stderr, "SimpleByteArray_insert_bytes: failed to grow list array!\n");
            return OUTOFMEMORY;
        }
        list->array = tmpArray;
        list->capacity = newCapacity;
    }

    // shift the tail once, instead of once per inserted byte
    memmove( list->array + index + count, list->array + index, (list->size - index) * sizeof(*list->array) );
    memcpy( list->array + index, values, count * sizeof(*list->array) );
    list->size += count;

    return SUCCESS;
}

STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed )
{
    PBYTE tmpArray = NULL;
    size_t iterator;

    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_remove: list is nullptr!\n");
        return POINTER;
    }

    if ( !list->capacity )
    {
        fprintf(stderr, "SimpleByteArray_remove: list has already been freed!\n");
        return ILLEGAL_METHOD_CALL;
    }

    if ( list->size <= index )
    {
        fprintf(stderr, "SimpleByteArray_remove: out of bounds deletion!\n");
        return BOUNDS;
    }
    if ( opt_removed )
        *opt_removed = list->array[index];

    // shift elements of the right hand side of index to the left
    for ( iterator = index + 1; iterator < list->size; iterator++ )
        list->array[iterator - 1] = list->array[iterator];

    list->size -= 1;
    if ( list->size < list->capacity/2 )
    {
        // shrink by /2
        tmpArray = (PBYTE)realloc( (void *)list->array, (list->capacity / 2) * sizeof(*list->array) );
        if ( tmpArray )
        {
            list->array = tmpArray;
            list->capacity /= 2;
        }
    }

    return SUCCESS;
}

/* getters */
STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value )
{
    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_get: list is nullptr!\n");
        return POINTER;
    }

    if ( !list->capacity )
    {
        fprintf(stderr, "SimpleByteArray_get: list has already been freed!\n");
        return ILLEGAL_METHOD_CALL;
    }

    if ( !value )
    {
        fprintf(stderr, "SimpleByteArray_get: value is null!\n");
        return POINTER;
    }

    if ( list->size <= index )
    {
        fprintf(stderr, "SimpleByteArray_get: out of bounds fetch!\n");
        return BOUNDS;
    }

    *value = list->array[index];

    return SUCCESS;
}

STATUS SimpleByteArray_size( const SimpleByteArray *list, size_t *value )
{
    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_size: list is nullptr!\n");
        return POINTER;
    }

    if ( !list->capacity )
    {
        fprintf(stderr, "SimpleByteArray_size: list has already been freed!\n");
        return ILLEGAL_METHOD_CALL;
    }

    if ( !value )
    {
        fprintf(stderr, "SimpleByteArray_size: value is null!\n");
        return POINTER;
    }

    *value = list->size;

    return SUCCESS;
}