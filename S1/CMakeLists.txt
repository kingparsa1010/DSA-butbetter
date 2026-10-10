# One executable per list implementation, so every program can run on each of them.
set( DSA_LIST_IMPLEMENTATIONS
        Linked linkedlists.c LINKED_LISTS
        DoublyLinked doublylinkedlists.c LINKED_LISTS
        Contiguous contiguouslists.c CONTIGUOUS_LISTS )

while ( DSA_LIST_IMPLEMENTATIONS )
    list( POP_FRONT DSA_LIST_IMPLEMENTATIONS kind source define )

    add_executable( ${kind}ListsTests
            tester.c
            ${source} )

    add_executable( ${kind}ListsSample
            sample.cpp
            ${source} )

    target_compile_definitions( ${kind}ListsSample PRIVATE
            ${define} )

    add_test( NAME ${kind}ListsTests COMMAND ${kind}ListsTests )

    # The sample runs its self-test first; option 5 then exits the menu.
    dsa_add_io_test( ${kind}ListsSample ${kind}ListsSample
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/sample_exit.in
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/sample_exit_${kind}.out )
endwhile ()
