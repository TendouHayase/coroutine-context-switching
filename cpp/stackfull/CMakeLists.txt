cmake_minimum_required(VERSION 3.20)
project(async-test LANGUAGES CXX ASM_MASM)

add_executable(async-test main.cpp ctx.asm)
set_source_files_properties(ctx.asm PROPERTIES COMPILE_FLAGS "/Zi")
