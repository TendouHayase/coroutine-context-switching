#include <windows.h>

#include <cstdint>

extern "C" void* context_switching(void* dst_rsp);

class Coroutine
{
    void* dst_rsp_ = nullptr;
    void* stack_ = nullptr;
    void (*fn_)() = nullptr;

  public:
    static constexpr int STACK_SIZE = 4096;

    Coroutine(void (*fn)())
    {
        this->fn_ = fn;
        stack_ = VirtualAlloc(NULL, STACK_SIZE, MEM_COMMIT | MEM_RESERVE,
                              PAGE_READWRITE);

        dst_rsp_ =
            reinterpret_cast<void*>(reinterpret_cast<std::uintptr_t>(stack_) +
                                    STACK_SIZE - sizeof(std::uintptr_t));
        *static_cast<std::uintptr_t*>(dst_rsp_) =
            reinterpret_cast<std::uintptr_t>(fn_);
    }

    ~Coroutine() { VirtualFree(stack_, 0, MEM_RELEASE); }

    Coroutine(const Coroutine&) = delete;
    Coroutine operator=(const Coroutine&) = delete;
    Coroutine(Coroutine&&) = delete;
    Coroutine operator=(Coroutine&&) = delete;

    void co_resume() { dst_rsp_ = context_switching(dst_rsp_); }
};

__forceinline void co_wait() { context_switching(nullptr); }
