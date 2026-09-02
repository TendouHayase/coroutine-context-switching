#include <intrin.h>
#include <windows.h>

#include <cstdint>
#include <memory>

extern "C" void* context_switching(void* dst_rsp);
extern "C" void* make_trap_frame(void* stack_top, void (*entry)(void*));

struct FatPointer
{
    void (*fn)(void*);
    void* data;
};

struct alignas(16) CoroutineFrame
{
    void* stack_base;            // 0x00
    size_t stack_size;           // 0x08
    void* co_rsp;                // 0x10
    void* caller_rsp;            // 0x18
    void* saved_stack_base;      // 0x20
    void* saved_stack_limit;     // 0x28
    bool finished = false;       // 0x30
    void (*invoke)(void* data);  // 0x38
    void (*destroy)(void* data); // 0x40
    void* closure_ptr;           // 0x48
};

inline uint8_t* align_down(uint8_t* p, size_t alignment)
{
    auto v = reinterpret_cast<uintptr_t>(p);
    return reinterpret_cast<uint8_t*>(v &
                                      ~(static_cast<uintptr_t>(alignment) - 1));
}

class Coroutine
{
  public:
    static constexpr std::uintptr_t STACK_SIZE = 1 << 20;
    template <typename F> Coroutine(F&& f)
    {
        using ClosureT = std::decay_t<F>;
        coroutine_statk_base_ = VirtualAlloc(
            NULL, STACK_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        coroutine_rsp_ = frame_ =
            (CoroutineFrame*)((std::uintptr_t)coroutine_statk_base_ +
                              STACK_SIZE - sizeof(CoroutineFrame));
        size_t lo, hi;

        GetCurrentThreadStackLimits(&lo, &hi);

        *(CoroutineFrame*)(coroutine_rsp_) =
            CoroutineFrame{coroutine_statk_base_,
                           STACK_SIZE,
                           nullptr,
                           nullptr,
                           (void*)lo,
                           (void*)hi,
                           false,
                           nullptr,
                           nullptr};

        auto* closure_addr = (uint8_t*)(coroutine_rsp_) - sizeof(ClosureT);
        closure_addr = align_down(closure_addr, alignof(closure_addr));

        new (closure_addr) ClosureT(std::forward<F>(f));

        frame_->invoke = [](void* data) {
            auto* closure = static_cast<ClosureT*>(data);
            (*closure)();
            closure->~ClosureT();
        };
        frame_->destroy = [](void* data) {
            static_cast<ClosureT*>(data)->~ClosureT();
        };

        // 트랩 프레임 정렬
        auto* trap_frame_top = align_down(closure_addr, 16);
        frame_->co_rsp = coroutine_rsp_ =
            make_trap_frame(coroutine_rsp_, &Coroutine::entry);
    }

    ~Coroutine()
    {
        if (!frame_->finished)
        {
            frame_->destroy(frame_->closure_ptr);
        }
        VirtualFree(coroutine_statk_base_, STACK_SIZE, MEM_RELEASE);
    }

    Coroutine(const Coroutine&) = delete;
    Coroutine operator=(const Coroutine&) = delete;
    Coroutine(Coroutine&&) = delete;
    Coroutine operator=(Coroutine&&) = delete;

    bool co_resume()
    {
        coroutine_rsp_ = context_switching(coroutine_rsp_);

        return !((CoroutineFrame*)coroutine_statk_base_)->finished;
    }

  private:
    static void entry(void* arg)
    {
        auto* frame = static_cast<CoroutineFrame*>(arg);
        frame->invoke(frame->closure_ptr); // storage → closure_ptr만 바뀜
        frame->finished = true;
        context_switching(frame->caller_rsp);

        // 이하 UB
    }

    void* coroutine_statk_base_;
    CoroutineFrame* frame_;
    void* coroutine_rsp_;
};

__forceinline void co_wait()
{

    CoroutineFrame* self =
        (CoroutineFrame*)__readgsqword(0x08) - sizeof(CoroutineFrame);
    self->caller_rsp = context_switching(self->caller_rsp);
}
