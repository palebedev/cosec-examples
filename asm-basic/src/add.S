    .intel_syntax noprefix

#ifndef __x86_64__
#error Only x86-64 supported!
#endif

#ifdef __WIN64__
    #define ALIGN16 .p2align 4,,15

    #define ARG0 rcx
    #define ARG1 rdx
    #define ARG2 r8
    #define ARG3 r9
    #define ARG4 [rbp+48]
    #define ARG5 [rbp+56]
    #define ARG6 [rbp+64]
    #define ARG7 [rbp+72]
    #define ARG8 [rbp+80]
#else
    #define ALIGN16 .align 16

    #define ARG0 rdi
    #define ARG1 rsi
    #define ARG2 rdx
    #define ARG3 rcx
    #define ARG4 r8
    #define ARG5 r9
    #define ARG6 [rbp+16]
    #define ARG7 [rbp+24]
    #define ARG8 [rbp+32]
#endif

    ALIGN16
// Normal labels are symbols (by default local, or "internal linkage")
// Give a symbol global visibility ("external linkage") to use from other TUs.
    .global add
// Set symbol type
#ifdef __ELF__
    .type add,@function
#else
    .def add; .scl 2; .type 32; .endef
#endif
add:
// Prologue, not needed if no locals/pushes.
    push rbp
    mov rbp,rsp
    // Allocate local variables of total 32 bytes (not needed here, example only)
    sub rsp,32

    mov rax,ARG0
    add rax,ARG1
    add rax,ARG2
    add rax,ARG3
    add rax,ARG4
// labels starting with .L do not become symbols ("local").
.Lmiddle:
    add rax,ARG5
    add rax,ARG6
    add rax,ARG7
    add rax,ARG8

// Epilogue, not needed if no prologue.
    // Restore rsp, not needed if you didn't modify it.
    mov rsp,rbp
    pop rbp
    ret
#ifdef __ELF__
// Set symbol size, . is "here".
    .size add, .-add
#endif

// Mark object file as not requiring executable stack.
#if defined(__linux__) && defined(__ELF__)
.section .note.GNU-stack,"",%progbits
#endif
