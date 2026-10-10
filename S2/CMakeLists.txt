# Every S2 program is built twice: once on top of linked lists and once on contiguous lists.
#   <Program>_Linked<Suffix>     -> ../S1/linkedlists.c     + LINKED_LISTS
#   <Program>_Contiguous<Suffix> -> ../S1/contiguouslists.c + CONTIGUOUS_LISTS
#
# dsa_add_s2_program( <program name> <suffix> <source file> <test name> )
# Also registers tests/<test name>*.in with tests/<same name>.out as expected output.
# A <test name> of SELF means the program is a self-checking test and only its exit code matters.
function( dsa_add_s2_program program suffix source test_name )
    foreach ( kind Linked Contiguous )
        if ( kind STREQUAL "Linked" )
            set( list_source ../S1/linkedlists.c )
            set( list_define LINKED_LISTS )
        else ()
            set( list_source ../S1/contiguouslists.c )
            set( list_define CONTIGUOUS_LISTS )
        endif ()

        set( target ${program}_${kind}${suffix} )

        add_executable( ${target}
                ${list_source}
                ${source} )

        target_compile_definitions( ${target} PRIVATE
                ${list_define} )

        if ( test_name STREQUAL "SELF" )
            add_test( NAME ${target} COMMAND ${target} )
        else ()
            file( GLOB inputs ${CMAKE_CURRENT_SOURCE_DIR}/tests/${test_name}*.in )
            foreach ( input ${inputs} )
                get_filename_component( case ${input} NAME_WE )
                dsa_add_io_test( ${target}.${case} ${target}
                        ${input}
                        ${CMAKE_CURRENT_SOURCE_DIR}/tests/${case}.out )
            endforeach ()
        endif ()
    endforeach ()
endfunction()

dsa_add_s2_program( BracketTester Stacks brackettester.cpp bracket )
dsa_add_s2_program( ManualStack Stacks manualstack.cpp stack )
dsa_add_s2_program( ManualQueue Queues manualqueue.cpp queue )
dsa_add_s2_program( ManualList Lists manuallist.cpp list )
dsa_add_s2_program( ContainerTests "" tester.cpp SELF )
