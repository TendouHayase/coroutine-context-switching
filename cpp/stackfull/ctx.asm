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

    mov QWORD PTR gs:[08h], rcx
    mov r15, rcx
    sub r15d, DWORD PTR [rcx+8h]
    mov QWORD PTR gs:[10h], r15
    

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

; void* make_trap_frame(void* stack_top);

make_trap_frame PROC
    mov rdx, rsp
    mov rsp, rcx
    mov rax, 0CCCCCCCCCCCCCCCCh
    push rax                     
    push rax
    push rax
    push rax
    push rax
    push rax
    push rax
    push rax

    mov rsp, rdx

    ret
make_trap_frame ENDP

END
