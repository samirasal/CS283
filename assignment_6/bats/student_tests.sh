#!/usr/bin/env bats

# Test: Basic command execution
@test "Basic command execution" {
    run "./dsh" <<EOF
ls
exit
EOF
    [ "$status" -eq 0 ]
}

# Test: Echo output
@test "Echo command" {
    run "./dsh" <<EOF
echo "Hello, World!"
exit
EOF
    [[ "$output" =~ "Hello, World!" ]]
}

# Test: Piping
@test "Pipes: ls | grep .c" {
    run "./dsh" <<EOF
ls | grep .c
exit
EOF
    [[ "$output" =~ ".c" ]]
}



# Test: Exit command
@test "Built-in exit command" {
    run "./dsh" <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

# Test: Output ordering (Debugging the "local mode" issue)
@test "Output ordering - local mode before dshlib" {
    run "./dsh" <<EOF
ls | grep dshlib.c
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="dshlib.clocalmodedsh4>dsh4>cmdloopreturned0"
    [ "$stripped_output" = "$expected_output" ]
}

# Test: Remote execution (Client-Server Mode)
@test "Remote execution - Connect to server" {
    ./dsh -s -p 5678 &
    sleep 1  # Give server time to start

    run "./dsh -c -p 5678" <<EOF
ls
exit
EOF

    [[ "$output" =~ "dshlib.c" ]]

    pkill -f "./dsh -s -p 5678"
}
