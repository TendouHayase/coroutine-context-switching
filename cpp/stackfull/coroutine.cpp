#include <windows.h>

#include <cstdint>

extern "C" void context_switching(void* dst_rsp);

class Coroutine
{
    std::uintptr_t dst_rsp_ = 0;
    std::uintptr_t dst_top_ = 0;
    void (*fn_)() = nullptr;

  public:
    static constexpr std::uintptr_t STACK_SIZE = 1 << 20;

    Coroutine(void (*fn)())
    {
        this->fn_ = fn;
        dst_top_ = (std::uintptr_t)VirtualAlloc(
            NULL, STACK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        dst_rsp_ = dst_top_ + STACK_SIZE;

        dst_rsp_ -= sizeof(decltype(STACK_SIZE));

        *(std::uintptr_t*)dst_rsp_ =
            STACK_SIZE -
            (sizeof(std::uintptr_t) + sizeof(decltype(STACK_SIZE)));

        dst_rsp_ -= sizeof(std::uintptr_t);

        *(void (**)())dst_rsp_ = fn_;
    }

    ~Coroutine() { VirtualFree((void*)dst_top_, 0, MEM_RELEASE); }

    Coroutine(const Coroutine&) = delete;
    Coroutine operator=(const Coroutine&) = delete;
    Coroutine(Coroutine&&) = delete;
    Coroutine operator=(Coroutine&&) = delete;

    void co_resume() { context_switching((void*)dst_rsp_); }
};

__forceinline void co_wait() { context_switching(nullptr); }
