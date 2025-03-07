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
@test "Built-in cd command changes directory" {
    run ./dsh <<EOF
    cd /
    pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/" ]]  # Ensure output contains /
}

@test "Which command locates executables" {
    run ./dsh <<EOF
    which ls
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/bin/ls" ]]  # Ensure it outputs a valid path
}

@test "ls command runs successfully" {
    run ./dsh <<EOF
    ls
EOF
    [ "$status" -eq 0 ]
}

