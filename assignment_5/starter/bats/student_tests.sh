#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


# Test basic pipelining: ls | grep
@test "Basic pipeline ls | grep .c" {
    run ./dsh <<EOF
ls | grep .c
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ ".c" ]]
}


# Test cd command
@test "Built-in cd command" {
    run ./dsh <<EOF
cd /
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/" ]]
}


# Test exit command
@test "Exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}


