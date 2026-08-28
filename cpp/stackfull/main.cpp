#if defined(_MSC_VER)

#include <cstdio>

#include "coroutine.cpp"

void print(const char* str)
{
    std::printf("%s", str);
    co_wait();
}

void help() { print("success"); }

int main(void)
{
    Coroutine c(help);
    c.co_resume();
}

#elif defined(__GNUC__)
#elif defined(__clang__)
#endif
