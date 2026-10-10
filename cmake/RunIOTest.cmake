# Runs EXE with INPUT on stdin and checks that stdout matches EXPECTED.
# Line endings are normalized so the same files work on Windows and Linux.

execute_process( COMMAND ${EXE}
                 INPUT_FILE ${INPUT}
                 OUTPUT_VARIABLE actual
                 RESULT_VARIABLE exit_code )

file( READ ${EXPECTED} expected )

string( REPLACE "\r\n" "\n" actual "${actual}" )
string( REPLACE "\r\n" "\n" expected "${expected}" )

if ( NOT exit_code EQUAL 0 )
    message( FATAL_ERROR "${EXE} exited with ${exit_code}\n--- output ---\n${actual}" )
endif ()

if ( NOT actual STREQUAL expected )
    message( FATAL_ERROR "Output mismatch for ${INPUT}\n--- expected ---\n${expected}--- actual ---\n${actual}" )
endif ()
