cmake_minimum_required(VERSION 3.20)
project(async-test LANGUAGES CXX ASM_MASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(async-test main.cpp ctx.asm)
set_source_files_properties(ctx.asm PROPERTIES COMPILE_FLAGS "/Zi")
