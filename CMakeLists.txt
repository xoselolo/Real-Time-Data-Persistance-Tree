cmake_minimum_required(VERSION 3.16)
project(transactions_binary_tree C)

set(CMAKE_C_STANDARD 99)

add_executable(transactions_binary_tree main.c passive.c passive.h frame.c frame.h binary_tree.c binary_tree.h types.h transaction.c transaction.h config.c config.h tools.c tools.h)