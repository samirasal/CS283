#!/usr/bin/env bats

# Test simple command execution
@test "Execute ls command" {
    run ./dsh <<EOF
ls
EOF
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

# Test multi-stage pipeline: ls | grep .c | wc -l
@test "Multi-stage pipeline" {
    run ./dsh <<EOF
ls | grep .c | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" =~ ^[0-9]+$ ]]  # Output should be a number
}

# Test echo with quoted arguments
@test "Echo preserves quoted spaces" {
    run ./dsh <<EOF
echo "hello   world"
EOF
    [ "$status" -eq 0 ]
    [ "$output" = "hello   world" ]
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

# Test invalid command handling
@test "Invalid command returns error" {
    run ./dsh <<EOF
invalidcommand
EOF
    [ "$status" -ne 0 ]  # Expect non-zero exit code
}

# Test exit command
@test "Exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}
