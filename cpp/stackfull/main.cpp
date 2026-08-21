#if defined(_MSC_VER)

#include <cstdint>
#include <cstdio>

extern "C" uint64_t asm_add(uint64_t lhs, uint64_t b);

int main(void) { std::printf("%llu\n", asm_add(15, 15)); }

#elif defined(__GNUC__)
#elif defined(__clang__)
#endif
