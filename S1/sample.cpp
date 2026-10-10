/**
 * Proprietary Source-Available License.
 *
 * Copyright (c) 2026 Weather. All Rights Reserved.
 *
 * This code is for educational purposes only.
 * Unauthorized copying, distribution, or modification is prohibited.
 */

#include <S1/simplearray.hpp>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

void testArray()
{
    SimpleArray<int> array;

#ifdef LINKED_LISTS
    std::printf("Using Linked Lists...\n");
#elif defined(CONTIGUOUS_LISTS)
    std::printf("Using Contiguous Lists...\n");
#endif

    // Initially empty
    assert(array.Size() == 0);

    // Insert one value
    array.Insert(0, 42);
    assert(array.Size() == 1);
    assert(array.Get(0) == 42);

    // Insert another value
    array.Insert(1, 100);
    assert(array.Size() == 2);
    assert(array.Get(0) == 42);
    assert(array.Get(1) == 100);

    // Insert in the middle
    array.Insert(1, 55);
    assert(array.Size() == 3);
    assert(array.Get(0) == 42);
    assert(array.Get(1) == 55);
    assert(array.Get(2) == 100);

    // Remove a value
    array.Remove(1);
    assert(array.Size() == 2);
    assert(array.Get(0) == 42);
    assert(array.Get(1) == 100);

    // Remove first value
    array.Remove(0);
    assert(array.Size() == 1);
    assert(array.Get(0) == 100);

    // Remove last value
    array.Remove(0);
    assert(array.Size() == 0);

    // Test another trivially-copyable type
    SimpleArray<double> doubles;
    doubles.Insert(0, 3.14);
    doubles.Insert(1, 2.71);

    assert(doubles.Size() == 2);
    assert(doubles.Get(0) == 3.14);
    assert(doubles.Get(1) == 2.71);

    // Find, Set and move
    assert(doubles.Find(2.71) == 1);
    doubles.Set(0, 1.41);
    assert(doubles.Get(0) == 1.41);

    SimpleArray<double> moved = std::move(doubles);
    assert(moved.Size() == 2);
    assert(moved.Get(1) == 2.71);

    // Strings are serialized as [length][characters]
    SimpleArray<std::string> strings;
    strings.Insert(0, "world");
    strings.Insert(0, "hello");
    strings.Insert(2, "");
    assert(strings.Size() == 3);
    assert(strings.Get(0) == "hello");
    assert(strings.Get(1) == "world");
    assert(strings.Get(2).empty());
    strings.Remove(0);
    assert(strings.Get(0) == "world");
}

struct Student
{
    int age;
    int id;
    double score;
};

void printStudent(const Student& student)
{
    std::printf("ID: %d\n", student.id);
    std::printf("Age: %d\n", student.age);
    std::printf("Score: %f\n", student.score);
}

// Prompts until a valid value of type T is entered. Exits cleanly when input runs out.
template <typename T>
T readValue(const char* prompt)
{
    T value{};

    while (true)
    {
        std::printf("%s", prompt);
        std::fflush(stdout);

        if (std::cin >> value)
            return value;

        if (std::cin.eof())
        {
            std::printf("\nNo more input. Goodbye!\n");
            std::exit(0);
        }

        // Not a number: drop the rest of the line and ask again.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::printf("Invalid input, please enter a number.\n");
    }
}

int main()
{
    SimpleArray<Student> students;
    int choice;

    testArray();

    while (true)
    {
        std::printf("\n--- Student Menu ---\n");
        std::printf("1. Add student\n");
        std::printf("2. Remove student\n");
        std::printf("3. Get student\n");
        std::printf("4. List students\n");
        std::printf("5. Exit\n");

        choice = readValue<int>("Choose an option: ");

        if ( choice == 1 )
        {
            Student student{};

            student.id = readValue<int>("Enter student ID: ");
            student.age = readValue<int>("Enter student age: ");
            student.score = readValue<double>("Enter student score: ");

            // Add to the end of the array
            students.Insert( students.Size(), student );

            std::printf("Student added.\n");
        }
        else if ( choice == 2 )
        {
            if ( students.Size() == 0 )
            {
                std::printf("There are no students.\n");
                continue;
            }

            size_t index = readValue<size_t>("Enter student index to remove: ");

            if ( index >= students.Size() )
            {
                std::printf("Invalid index.\n");
                continue;
            }

            students.Remove( index );

            std::printf("Student removed.\n");
        }
        else if ( choice == 3 )
        {
            if ( students.Size() == 0 )
            {
                std::printf("There are no students.\n");
                continue;
            }

            size_t index = readValue<size_t>("Enter student index: ");

            if ( index >= students.Size() )
            {
                std::printf("Invalid index.\n");
                continue;
            }

            Student student = students.Get( index );

            std::printf("\nStudent information:\n");
            printStudent( student );
        }
        else if ( choice == 4 )
        {
            if ( students.Size() == 0 )
            {
                std::printf("There are no students.\n");
                continue;
            }

            std::printf("\n--- Students ---\n");

            for ( size_t i = 0; i < students.Size(); i++ )
            {
                Student student = students.Get( i );

                std::printf("\nIndex: %zu\n", i);
                printStudent( student );
            }
        }
        else if ( choice == 5 )
        {
            std::printf("Goodbye!\n");
            break;
        }
        else
        {
            std::printf("Invalid option.\n");
        }
    }

    return 0;
}
