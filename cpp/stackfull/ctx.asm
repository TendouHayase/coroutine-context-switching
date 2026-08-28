.code

; void context_switching(void* dst_rsp);

context_switching PROC
    push rbp
    push rbx
    push rsi
    push rdi
    push r15
    push r14
    push r13
    push r12
    sub rsp, 160  ; XMM6-XMM15

    mov rax, rsp
    mov rsp, rcx
 
    ret 
context_switching ENDP

END