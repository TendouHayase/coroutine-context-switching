.code

; uint64_t asm_add(uint64_t lhs, uint64_t rhs);

asm_add PROC
    mov rax, rcx ; 1번째 인자
    add rax, rdx ; 2번째 인자
    ret 
asm_add ENDP

END