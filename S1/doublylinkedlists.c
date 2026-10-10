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

// Doubly linked variant: same list as linkedlists.c, but every node also points back to the previous one.
// This makes tail removal O(1) and lets lookups walk from whichever end is closer.

/* WARNING: THE FOLLOWING IMPLEMENTATION IS NOT THREAD SAFE!!! */

typedef struct _SimpleByteArrayNode
{
// PRIVATE:
    BYTE value;
    struct _SimpleByteArrayNode *next;
    struct _SimpleByteArrayNode *previous;
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

/* helpers */

// Walks from whichever end is closer, which is the main advantage of a doubly linked list.
// Caller guarantees index < list->size.
static SimpleByteArrayNode *SimpleByteArray_node_at( const SimpleByteArray *list, size_t index )
{
    SimpleByteArrayNode *current;
    size_t iterator;

    if ( index < list->size / 2 )
    {
        current = list->head;
        for ( iterator = 0; iterator < index; iterator++ )
            current = current->next;
    } else
    {
        current = list->tail;
        for ( iterator = list->size - 1; iterator > index; iterator-- )
            current = current->previous;
    }

    return current;
}

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
    newNode->previous = NULL;

    if ( index == 0 )
    {
        //insert at head
        newNode->next = list->head;

        if ( list->head )
            list->head->previous = newNode;
        else
            list->tail = newNode;

        list->head = newNode;
    } else if ( index == list->size )
    {
        // insert at tail
        newNode->previous = list->tail;

        list->tail->next = newNode;
        list->tail = newNode;
    } else
    {
        current = SimpleByteArray_node_at( list, index );

        newNode->next = current;
        newNode->previous = current->previous;

        current->previous->next = newNode;
        current->previous = newNode;
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
        newNode->previous = last;

        if ( last )
            last->next = newNode;
        else
            first = newNode;

        last = newNode;
    }

    // then splice it in with a single walk, instead of one walk per byte
    if ( index == list->size )
    {
        first->previous = list->tail;

        if ( list->tail )
            list->tail->next = first;
        else
            list->head = first;

        list->tail = last;
    } else
    {
        // current is the node that will come right after the new chain
        current = SimpleByteArray_node_at( list, index );

        first->previous = current->previous;
        last->next = current;

        if ( current->previous )
            current->previous->next = first;
        else
            list->head = first;

        current->previous = last;
    }

    list->size += count;

    return SUCCESS;
}

STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed )
{
    SimpleByteArrayNode *current = NULL;

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

    current = SimpleByteArray_node_at( list, index );

    if ( current->previous )
        current->previous->next = current->next;
    else
        list->head = current->next;

    if ( current->next )
        current->next->previous = current->previous;
    else
        list->tail = current->previous;

    if ( opt_removed )
        *opt_removed = current->value;

    free( current );

    list->size--;

    return SUCCESS;
}

/* getters */
STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value )
{
    SimpleByteArrayNode *current = NULL;

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

    current = SimpleByteArray_node_at( list, index );

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