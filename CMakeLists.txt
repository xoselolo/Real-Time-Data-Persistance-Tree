cmake_minimum_required(VERSION 3.16)
project(transactions_binary_tree C)

set(CMAKE_C_STANDARD 99)

add_executable(transactions_binary_tree main.c binary_tree.c binary_tree.h types.h)