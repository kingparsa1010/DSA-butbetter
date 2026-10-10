/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <utils.h>

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _SimpleByteArray SimpleByteArray;

extern STATUS SimpleByteArray_init( SimpleByteArray **out_list );
extern STATUS SimpleByteArray_free( SimpleByteArray *in_list );
extern STATUS SimpleByteArray_insert( SimpleByteArray *list, size_t index, BYTE value );
extern STATUS SimpleByteArray_insert_bytes( SimpleByteArray *list, size_t index, const BYTE *values, size_t count );
extern STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed );
extern STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value );
extern STATUS SimpleByteArray_size( const SimpleByteArray *list, size_t *value );

/* Test Helpers */
static void expect_status( const char *what, STATUS got, STATUS expected )
{
    if ( got != expected )
    {
        fprintf(stderr, "FAIL: %s: got STATUS %d, expected %d\n", what, (int)got, (int)expected );
        abort();
    }
}

static void expect_ok( const char *what, STATUS got )
{
    expect_status( what, got, SUCCESS );
}

static void expect_bytes( const SimpleByteArray *list, const PBYTE expected, size_t expected_count )
{
    STATUS st = SUCCESS;
    BYTE actual = 0;
    size_t actual_size = 999999;
    size_t iterator;

    expect_ok( "SimpleByteArray_size", SimpleByteArray_size( list, &actual_size ) );

    if (actual_size != expected_count)
    {
        fprintf( stderr, "FAIL: size mismatch: got %zu, expected %zu\n", actual_size, expected_count );
        abort();
    }

    for ( iterator = 0; iterator < expected_count; iterator++ )
    {
        st = SimpleByteArray_get( list, iterator, &actual );

        if ( st != SUCCESS || actual != expected[iterator] )
        {
            fprintf( stderr, "FAIL: element[%zu]: got status=%d value=%d, " "expected value=%d\n", iterator, (int)st, (int)actual, (int)expected[iterator] );
            abort();
        }
    }
}

static void expect_get_failure( const SimpleByteArray *list, size_t index )
{
    BYTE value = 0x55;
    STATUS st = SimpleByteArray_get( list, index, &value );

    if ( st == SUCCESS )
    {
        fprintf( stderr, "FAIL: get(%zu) unexpectedly succeeded\n", index );
        abort();
    }
}

static void expect_remove_failure( SimpleByteArray *list, size_t index )
{
    BYTE removed = 0x55;
    STATUS st = SimpleByteArray_remove(list, index, &removed);

    if ( st == SUCCESS )
    {
        fprintf( stderr, "FAIL: remove(%zu) unexpectedly succeeded\n", index );
        abort();
    }
}

/* Init tests */
static void test_empty_list()
{
    SimpleByteArray *list = NULL;

    printf( "test_empty_list\n" );

    expect_ok( "init", SimpleByteArray_init( &list ) );

    expect_bytes( list, NULL, 0 );

    expect_get_failure( list, 0 );
    expect_get_failure( list, SIZE_MAX );

    expect_remove_failure( list, 0 );
    expect_remove_failure( list, SIZE_MAX );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

static void test_single_element()
{
    SimpleByteArray *list = NULL;
    BYTE expected[] = { 'A' };
    BYTE value = 0;
    BYTE removed = 0;

    printf("test_single_element\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );

    expect_ok( "insert(0)", SimpleByteArray_insert( list, 0, 'A' ) );

    expect_bytes( list, expected, 1 );

    expect_ok( "get(0)", SimpleByteArray_get( list, 0, &value ) );
    assert( value == 'A' );

    expect_ok( "remove(0)", SimpleByteArray_remove( list, 0, &removed ) );
    assert( removed == 'A' );

    expect_bytes( list, NULL, 0 );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* Insertions */
static void test_head_tail_insertions()
{
    SimpleByteArray *list = NULL;
    BYTE expected[] = { 'X', 'A', 'Y', 'B', 'C' };

    printf("test_head_tail_insertions\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );

    expect_ok( "insert A", SimpleByteArray_insert( list, 0, 'A' ) );
    expect_ok( "insert B", SimpleByteArray_insert( list, 1, 'B' ) );
    expect_ok( "insert C", SimpleByteArray_insert( list, 2, 'C' ) );
    expect_ok( "insert X at head", SimpleByteArray_insert( list, 0, 'X' ) );
    expect_ok( "insert Y in middle", SimpleByteArray_insert( list, 2, 'Y' ) );

    expect_bytes( list, expected, sizeof(expected) );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* Removals */
static void test_removals()
{
    SimpleByteArray *list = NULL;
    BYTE initial[] = { 10, 20, 30, 40, 50 };
    BYTE expected1[] = { 20, 30, 40, 50 };
    BYTE expected2[] = { 20, 40, 50 };
    BYTE expected3[] = { 20, 40 };
    BYTE removed;

    printf("test_removals\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );

    expect_ok( "insert_bytes", SimpleByteArray_insert_bytes( list, 0, initial, sizeof(initial) ) );

    removed = 0;
    expect_ok( "remove head", SimpleByteArray_remove( list, 0, &removed ) );
    assert( removed == 10 );

    expect_bytes( list, expected1, sizeof(expected1) );

    removed = 0;
    expect_ok( "remove middle", SimpleByteArray_remove( list, 1, &removed ) );
    assert( removed == 30 );

    expect_bytes( list, expected2, sizeof(expected2) );

    removed = 0;
    expect_ok( "remove tail", SimpleByteArray_remove( list, 2, &removed ) );
    assert( removed == 50 );

    expect_bytes( list, expected3, sizeof(expected3) );

    expect_ok( "remove first", SimpleByteArray_remove( list, 0, &removed ) );
    assert( removed == 20 );

    expect_ok( "remove last", SimpleByteArray_remove( list, 0, &removed ) );
    assert( removed == 40 );

    expect_bytes( list, NULL, 0 );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* Insert Bytes */
static void test_insert_bytes()
{
    SimpleByteArray *list = NULL;
    BYTE a[] = { 1, 2, 3 };
    BYTE b[] = { 10, 11, 12, 13 };
    BYTE c[] = { 99 };
    BYTE expected[] = {
        99,
        1, 2, 3,
        10, 11, 12, 13
    };

    printf("test_insert_bytes\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );
    expect_ok( "insert a", SimpleByteArray_insert_bytes( list, 0, a, 3 ) );
    expect_ok( "insert b at tail", SimpleByteArray_insert_bytes( list, 3, b, 4 ) );
    expect_ok( "insert c at head", SimpleByteArray_insert_bytes( list, 0, c, 1 ) );

    expect_bytes( list, expected, sizeof(expected) );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

static void test_insert_bytes_middle()
{
    SimpleByteArray *list = NULL;
    BYTE initial[] = { 1, 2, 5, 6 };
    BYTE middle[] = { 3, 4 };
    BYTE expected[] = { 1, 2, 3, 4, 5, 6 };

    printf("test_insert_bytes_middle\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );
    expect_ok( "insert initial", SimpleByteArray_insert_bytes( list, 0, initial, sizeof(initial) ) );
    expect_ok( "insert middle", SimpleByteArray_insert_bytes( list, 2, middle, sizeof(middle) ) );

    expect_bytes( list, expected, sizeof(expected) );

    // inserting nothing is a no-op
    expect_ok( "insert zero bytes", SimpleByteArray_insert_bytes( list, 3, middle, 0 ) );
    expect_bytes( list, expected, sizeof(expected) );

    // an out of bounds insertion must leave the list untouched
    expect_status( "insert past the end", SimpleByteArray_insert_bytes( list, sizeof(expected) + 1, middle, sizeof(middle) ), BOUNDS );
    expect_bytes( list, expected, sizeof(expected) );

    // the tail must still be correct after the splice
    expect_ok( "insert at tail", SimpleByteArray_insert( list, sizeof(expected), 7 ) );
    expect_ok( "remove tail", SimpleByteArray_remove( list, sizeof(expected), NULL ) );
    expect_bytes( list, expected, sizeof(expected) );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* ---------- Invalid arguments ---------- */

static void test_invalid_arguments()
{
    SimpleByteArray *list = NULL;
    BYTE value = 0;
    size_t size = 0;

    printf("test_invalid_arguments\n");

    expect_status( "init(NULL)", SimpleByteArray_init( NULL ), POINTER );
    expect_status( "free(NULL)", SimpleByteArray_free( NULL ), POINTER );
    expect_status( "insert(NULL)", SimpleByteArray_insert( NULL, 0, 1 ), POINTER );
    expect_status( "insert_bytes(NULL)", SimpleByteArray_insert_bytes( NULL, 0, &value, 1 ), POINTER );
    expect_status( "remove(NULL)", SimpleByteArray_remove( NULL, 0, NULL ), POINTER );
    expect_status( "get(NULL)", SimpleByteArray_get( NULL, 0, &value ), POINTER );
    expect_status( "size(NULL)", SimpleByteArray_size( NULL, &size ), POINTER );

    expect_ok( "init", SimpleByteArray_init( &list ) );
    expect_ok( "insert", SimpleByteArray_insert( list, 0, 1 ) );

    expect_status( "insert_bytes(values = NULL)", SimpleByteArray_insert_bytes( list, 0, NULL, 1 ), POINTER );
    expect_status( "get(value = NULL)", SimpleByteArray_get( list, 0, NULL ), POINTER );
    expect_status( "size(value = NULL)", SimpleByteArray_size( list, NULL ), POINTER );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* ---------- Growing and shrinking ---------- */

static void test_grow_and_shrink()
{
    SimpleByteArray *list = NULL;
    BYTE value = 0;
    size_t size = 0;
    size_t i;

    printf("test_grow_and_shrink\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );

    for ( i = 0; i < 1000; i++ )
        expect_ok( "insert", SimpleByteArray_insert( list, i, (BYTE)(i & 0x7f) ) );

    // remove every other element from the front half
    for ( i = 0; i < 500; i++ )
    {
        expect_ok( "remove", SimpleByteArray_remove( list, i, &value ) );
        assert( value == (BYTE)((2 * i) & 0x7f) );
    }

    expect_ok( "size", SimpleByteArray_size( list, &size ) );
    assert( size == 500 );

    for ( i = 0; i < 500; i++ )
    {
        expect_ok( "get", SimpleByteArray_get( list, i, &value ) );
        assert( value == (BYTE)((2 * i + 1) & 0x7f) );
    }

    while ( size > 0 )
    {
        expect_ok( "remove last", SimpleByteArray_remove( list, size - 1, NULL ) );
        size--;
    }

    expect_bytes( list, NULL, 0 );

    // the list must still be usable after shrinking all the way down
    expect_ok( "insert after shrink", SimpleByteArray_insert( list, 0, 42 ) );
    expect_ok( "get after shrink", SimpleByteArray_get( list, 0, &value ) );
    assert( value == 42 );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* ---------- Boundary indices ---------- */

static void test_index_boundaries()
{
    STATUS st = SUCCESS;
    SimpleByteArray *list = NULL;
    BYTE expected[] = { 1 };
    BYTE expected2[] = { 1, 2 };

    printf("test_index_boundaries\n");

    expect_ok( "init", SimpleByteArray_init( &list ) );
    expect_ok( "insert into empty at 0", SimpleByteArray_insert( list, 0, 1 ) );

    expect_bytes(list, expected, 1);

    expect_ok( "insert at size", SimpleByteArray_insert( list, 1, 2 ) );

    expect_bytes( list, expected2, 2 );

    st = SimpleByteArray_insert( list, 3, 3 );
    assert( st != SUCCESS );

    expect_bytes( list, expected2, 2 );

    expect_ok( "free", SimpleByteArray_free( list ) );
}

/* Lifecycle tests */
static void test_repeated_lifecycle()
{
    size_t iteration;
    size_t i;
    SimpleByteArray *list = NULL;

    printf("test_repeated_lifecycle\n");

    for ( iteration = 0; iteration < 10000; iteration++ )
    {
        expect_ok( "init", SimpleByteArray_init( &list ) );

        for ( i = 0; i < 100; i++ )
        {
            expect_ok( "insert", SimpleByteArray_insert( list, i, (BYTE)(i & 0x7f) ) );
        }

        expect_ok( "free", SimpleByteArray_free( list ) );
    }
}

int main()
{
    printf("=== SimpleByteArray stress test ===\n");

    test_empty_list();
    test_single_element();
    test_head_tail_insertions();
    test_removals();
    test_insert_bytes();
    test_insert_bytes_middle();
    test_invalid_arguments();
    test_grow_and_shrink();
    test_index_boundaries();
    test_repeated_lifecycle();

    printf("\nALL TESTS PASSED\n");
    return 0;
}