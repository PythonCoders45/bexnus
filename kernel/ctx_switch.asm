global ctx_switch

; void ctx_switch(struct regs *old, struct regs *new)
ctx_switch:
    ; save old
    mov [eax + 0*4], edi
    mov [eax + 1*4], esi
    mov [eax + 2*4], ebp
    mov [eax + 3*4], esp
    mov [eax + 4*4], ebx
    mov [eax + 5*4], edx
    mov [eax + 6*4], ecx
    mov [eax + 7*4], eax

    ; load new
    mov edi, [edx + 0*4]
    mov esi, [edx + 1*4]
    mov ebp, [edx + 2*4]
    mov esp, [edx + 3*4]
    mov ebx, [edx + 4*4]
    mov edx, [edx + 5*4]
    mov ecx, [edx + 6*4]
    mov eax, [edx + 7*4]

    ret
