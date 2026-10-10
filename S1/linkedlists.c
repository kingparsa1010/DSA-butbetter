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

// Question 2: Implement a dynamic array with insertion and deletion.
// Create a List type that stores elements in a dynamically allocated linked list.

/* WARNING: THE FOLLOWING IMPLEMENTATION IS NOT THREAD SAFE!!! */

typedef struct _SimpleByteArrayNode
{
// PRIVATE:
    BYTE value;
    struct _SimpleByteArrayNode *next;
} SimpleByteArrayNode;

typedef struct _SimpleByteArray
{
    SimpleByteArrayNode *head;
    SimpleByteArrayNode *tail;
    size_t size;
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

    newList->head = NULL;
    newList->tail = NULL;
    newList->size = 0;

    *out_list = newList;

    return SUCCESS;
}

STATUS SimpleByteArray_free( SimpleByteArray *in_list )
{
    SimpleByteArrayNode *current;
    SimpleByteArrayNode *next;

    if ( !in_list )
    {
        fprintf(stderr, "SimpleByteArray_free: in_list is nullptr!\n");
        return POINTER;
    }

    current = in_list->head;
    while ( current )
    {
        next = current->next;
        free( current );
        current = next;
    }

    free( in_list );

    return SUCCESS;
}

/* insertion, and removal */
STATUS SimpleByteArray_insert( SimpleByteArray *list, size_t index, BYTE value )
{
    SimpleByteArrayNode *newNode = NULL;
    SimpleByteArrayNode *current = NULL;
    size_t iterator;

    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_insert: list is nullptr!\n");
        return POINTER;
    }

    if ( index > list->size )
    {
        fprintf(stderr, "SimpleByteArray_insert: out of bounds insertion!\n");
        return BOUNDS;
    }

    newNode = (SimpleByteArrayNode *)malloc( sizeof(*newNode) );
    if ( !newNode )
        return OUTOFMEMORY;

    newNode->value = value;
    newNode->next = NULL;

    if ( index == list->size )
    {
        // insert at tail
        if ( list->tail )
        {
            list->tail->next = newNode;
            list->tail = newNode;
        } else
        {
            list->head = newNode;
            list->tail = newNode;
        }
    } else if ( index == 0 )
    {
        //insert at head
        newNode->next = list->head;
        list->head = newNode;
    } else
    {
        current = list->head;
        for ( iterator = 1 /* stop right before the dest node*/; iterator < index; iterator++ )
            current = current->next;

        newNode->next = current->next;
        current->next = newNode;
    }

    list->size++;

    return SUCCESS;
}

STATUS SimpleByteArray_insert_bytes( SimpleByteArray *list, size_t index, const BYTE *values, size_t count )
{
    SimpleByteArrayNode *first = NULL;
    SimpleByteArrayNode *last = NULL;
    SimpleByteArrayNode *newNode = NULL;
    SimpleByteArrayNode *current = NULL;
    size_t iterator;

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

    if ( index > list->size )
    {
        fprintf(stderr, "SimpleByteArray_insert_bytes: out of bounds insertion!\n");
        return BOUNDS;
    }

    if ( !count )
        return SUCCESS;

    // build the whole chain first, so a failed allocation leaves the list untouched
    for ( iterator = 0; iterator < count; iterator++ )
    {
        newNode = (SimpleByteArrayNode *)malloc( sizeof(*newNode) );
        if ( !newNode )
        {
            while ( first )
            {
                newNode = first->next;
                free( first );
                first = newNode;
            }
            fprintf(stderr, "SimpleByteArray_insert_bytes: failed to allocate nodes!\n");
            return OUTOFMEMORY;
        }

        newNode->value = values[iterator];
        newNode->next = NULL;

        if ( last )
            last->next = newNode;
        else
            first = newNode;

        last = newNode;
    }

    // then splice it in with a single walk, instead of one walk per byte
    if ( index == 0 )
    {
        last->next = list->head;
        list->head = first;

        if ( !list->tail )
            list->tail = last;
    } else
    {
        if ( index == list->size )
            current = list->tail;
        else
        {
            current = list->head;
            for ( iterator = 1 /* stop right before the dest node*/; iterator < index; iterator++ )
                current = current->next;
        }

        last->next = current->next;
        current->next = first;

        if ( current == list->tail )
            list->tail = last;
    }

    list->size += count;

    return SUCCESS;
}

STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed )
{
    SimpleByteArrayNode *current = NULL;
    SimpleByteArrayNode *removed = NULL;
    size_t iterator;

    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_remove: list is nullptr!\n");
        return POINTER;
    }

    if ( list->size <= index )
    {
        fprintf(stderr, "SimpleByteArray_remove: out of bounds deletion!\n");
        return BOUNDS;
    }

    if ( index == 0 )
    {
        // remove head
        removed = list->head;
        list->head = removed->next;

        if (list->head == NULL)
            list->tail = NULL;
    }
    else
    {
        current = list->head;

        for ( iterator = 1 /* stop right before the dest node*/; iterator < index; iterator++ )
            current = current->next;

        removed = current->next;
        current->next = removed->next;

        if ( removed == list->tail )
            list->tail = current;
    }

    if ( opt_removed )
        *opt_removed = removed->value;

    free( removed );

    list->size--;

    return SUCCESS;
}

/* getters */
STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value )
{
    SimpleByteArrayNode *current = NULL;
    size_t iterator;

    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_get: list is nullptr!\n");
        return POINTER;
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

    current = list->head;
    for ( iterator = 0; iterator < index; iterator++ )
        current = current->next;

    *value = current->value;

    return SUCCESS;
}

STATUS SimpleByteArray_size( const SimpleByteArray *list, size_t *value )
{
    if ( !list )
    {
        fprintf(stderr, "SimpleByteArray_size: list is nullptr!\n");
        return POINTER;
    }

    if ( !value )
    {
        fprintf(stderr, "SimpleByteArray_size: value is null!\n");
        return POINTER;
    }

    *value = list->size;

    return SUCCESS;
}