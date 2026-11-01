cmake_minimum_required(VERSION 3.10)

project(phonefetch C)

add_executable(phonefetch main.c)

install(TARGETS phonefetch DESTINATION bin)