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

    mov rsp, rax
    mov r14d, DWORD PTR [rcx+8h]
    mov rsp, rcx

    add rcx, 10h

    mov QWORD PTR gs:[08h], rcx
    mov r15, rcx
    sub r15, r14
    mov QWORD PTR gs:[10h], r15
 
    ret 
context_switching ENDP

END
