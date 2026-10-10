/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <S1/simplearray.hpp>

// Tests the S2 containers on top of whichever list implementation this target is built with.
/*
 * Simple Stack Implementation: */ #include <S2/simplestack.hpp> /*
 */
/*
 * Simple Queue Implementation: */ #include <S2/simplequeue.hpp> /*
 */
/*
 * Simple Deque Implementation: */ #include <S2/simpledeque.hpp> /*
 */

#include <cstdio>
#include <cstdlib>
#include <string>

#define EXPECT( condition ) \
    do { if ( !( condition ) ) { std::fprintf( stderr, "FAIL: %s (line %d)\n", #condition, __LINE__ ); std::abort(); } } while (0)

template <typename F>
static void expect_underflow( F &&operation )
{
    try
    {
        operation();
    } catch ( Exception &e )
    {
        EXPECT( e.status == STACK_UNDERFLOW );
        return;
    }

    std::fprintf( stderr, "FAIL: expected an underflow exception\n" );
    std::abort();
}

static void test_stack()
{
    SimpleStack<int> stack{};

    std::printf( "test_stack\n" );

    EXPECT( stack.Empty() );
    expect_underflow( [&] { stack.Pop(); } );
    expect_underflow( [&] { stack.Top(); } );

    stack.Push( 1 );
    stack.Push( 2 );
    stack.Push( 3 );

    EXPECT( stack.Size() == 3 );
    EXPECT( stack.Top() == 3 );
    EXPECT( stack.Pop() == 3 );
    EXPECT( stack.Pop() == 2 );
    EXPECT( stack.Pop() == 1 );
    EXPECT( stack.Empty() );
}

static void test_string_stack()
{
    SimpleStack<std::string> stack{};

    std::printf( "test_string_stack\n" );

    stack.Push( "first" );
    stack.Push( "" );
    stack.Push( "with spaces inside" );

    EXPECT( stack.Size() == 3 );
    EXPECT( stack.Pop() == "with spaces inside" );
    EXPECT( stack.Pop().empty() );
    EXPECT( stack.Top() == "first" );
    EXPECT( stack.Pop() == "first" );
    EXPECT( stack.Empty() );
}

static void test_queue()
{
    SimpleQueue<std::string> queue{};

    std::printf( "test_queue\n" );

    EXPECT( queue.Empty() );
    expect_underflow( [&] { queue.Pop(); } );
    expect_underflow( [&] { queue.Front(); } );

    queue.Push( "a" );
    queue.Push( "bb" );
    queue.Push( "ccc" );

    EXPECT( queue.Size() == 3 );
    EXPECT( queue.Front() == "a" );
    EXPECT( queue.Pop() == "a" );
    EXPECT( queue.Pop() == "bb" );

    queue.Push( "dddd" );

    EXPECT( queue.Pop() == "ccc" );
    EXPECT( queue.Pop() == "dddd" );
    EXPECT( queue.Empty() );
}

static void test_deque()
{
    SimpleDeque<long> deque{};

    std::printf( "test_deque\n" );

    EXPECT( deque.Empty() );
    expect_underflow( [&] { deque.PopFront(); } );
    expect_underflow( [&] { deque.PopBack(); } );
    expect_underflow( [&] { deque.Front(); } );
    expect_underflow( [&] { deque.Back(); } );

    deque.PushBack( 2 );
    deque.PushFront( 1 );
    deque.PushBack( 3 );

    EXPECT( deque.Size() == 3 );
    EXPECT( deque.Front() == 1 );
    EXPECT( deque.Back() == 3 );
    EXPECT( deque.PopBack() == 3 );
    EXPECT( deque.PopFront() == 1 );
    EXPECT( deque.PopFront() == 2 );
    EXPECT( deque.Empty() );
}

static void test_many_elements()
{
    SimpleStack<int> stack{};
    SimpleQueue<int> queue{};
    int i;

    std::printf( "test_many_elements\n" );

    for ( i = 0; i < 2000; i++ )
    {
        stack.Push( i );
        queue.Push( i );
    }

    for ( i = 0; i < 2000; i++ )
    {
        EXPECT( stack.Pop() == 1999 - i );
        EXPECT( queue.Pop() == i );
    }

    EXPECT( stack.Empty() );
    EXPECT( queue.Empty() );
}

int main()
{
#ifdef LINKED_LISTS
    std::printf( "=== S2 containers on linked lists ===\n" );
#elif defined( CONTIGUOUS_LISTS )
    std::printf( "=== S2 containers on contiguous lists ===\n" );
#endif

    test_stack();
    test_string_stack();
    test_queue();
    test_deque();
    test_many_elements();

    std::printf( "\nALL TESTS PASSED\n" );
    return 0;
}
