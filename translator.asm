section .text  
global _start  

_start:        
	mov  rbp , rsp 
	sub  rsp , 8
	push rbp 
	push 0 
	mov  rax , rsp 
	mov  rbp , rax 
	call func_777_1 
	push rax 
	mov  rax , 60
	mov  rdi , 0
	syscall

func_777_1 : 
	pop  rax 	; ret addr -> rax
	mov  [rbp + 2 * 8], rax 
	mov  rax , 66 
	mov  rsp , rbp 
	add  rsp , 8
	pop  rbp 
	ret 




;=================================================================================
; printf
;
; Input:    rax = dec_num
; Output:   
; Destroys: rax, rbx, rcx, rsi, rdi
;=================================================================================
printf:
    mov rbx, 10
    xor rcx, rcx        ; rcx = digits counter

    mov  rdi, rax       ; save in rdi

    test rax, rax
    jns  parse_new_digit
    neg  rax

parse_new_digit:
    xor  rdx, rdx
    div  rbx            ; rax /= 10, rdx = digit
    add  rdx, '0'       ; rdx = letter
    push rdx            ; letter in stack
    inc  rcx            ; counter++

    test rax, rax
    jnz  parse_new_digit

    test rdi, rdi
    jns  print_next_digit
    push "-"
    inc  rcx

print_next_digit:
    mov  rsi, rsp       ; [rsi] = letter
    mov  rax, 1         ; sys_write
    mov  rdi, 1         ; stdout
    mov  rdx, 1         ; len = 1

    push rcx
    syscall
    pop  rcx

    add  rsp, 8         ; clear stack
    dec  rcx            ; counter--

    test rcx, rcx
    jnz  print_next_digit

    push 0xA            ; \n
    mov  rsi, rsp
    mov  rax, 1         ; sys_write
    mov  rdi, 1         ; stdout
    mov  rdx, 1         ; len = 1
    syscall
    add  rsp, 8         ; clear stack

    ret


;=================================================================================
; scanf
;
; Input:    none
; Output:   rax = decimal number
; Destroys: rax, rbx, rcx, rsi, rdi
;=================================================================================
scanf:
    xor rax, rax     ; �������� ���������
    mov rbx, 1       ; ��������� ����� (1 ��� -1)

.read_loop:
    ; ������ 1 ������
    push rax
    push 0
    mov rsi, rsp
    xor rax, rax     ; sys_read
    xor rdi, rdi     ; stdin
    mov rdx, 1       ; 1 ������

    syscall
    pop rcx

    pop rax

    test rcx, rcx
    jz   .return

    ; ������������ ����
    cmp cl, '-'
    je .negative
    cmp cl, '+'
    je .read_loop
    ; ; ��������� �����
    cmp cl, '0'
    jb .return
    cmp cl, '9'
    ja .return

    ; ���� �����
    imul rax, 10
    sub  cl, '0'
    add  rax, rcx
    jmp  .read_loop

.negative:
    neg rbx
    jmp .read_loop

.return:
    imul rax, rbx
    ret



;=================================================================================
; sqrt
;
; Input:    rbx = num
; Output:   rax = sqrt(num)
;=================================================================================
sqrt:
    ; ����: RBX = ����� ����� (64-bit), �����: RAX = ����� ����� sqrt(RBX)
    movq    xmm0, rbx          ; ��������� ����� ����� �� RBX � XMM0
    cvtsi2sd xmm0, rbx         ; ����������� 64-bit ����� � double (������)
    sqrtsd  xmm0, xmm0         ; ��������� ���������� ������ (double)
    cvtsd2si rax, xmm0         ; ������������ ��������� ������� � 64-bit �����
    ret