.code

; void context_switching(void* dst_rsp);

context_switching PROC
    push rbx
    push rbp
    push rdi
    push rsi
    push r12
    push r13
    push r14
    push r15

    mov rax, rsp
    mov rsp, rcx
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rsi
    pop rdi
    pop rbp
    pop rbx

    ret 
context_switching ENDP

; void* make_trap_frame(void* stack_top, void (*entry)(void*));

make_trap_frame PROC
    mov rsp, rcx
    push rdx
    mov r10, rsp
    mov r11, 0CCCCCCCCCCCCCCCCh
    push r11                     
    push r11
    push r11
    push r11
    push r11
    push r11
    push r11
    push r11

    mov rax, rsp
    mov rsp, r10

    ret
make_trap_frame ENDP

END
