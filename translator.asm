func_0_1 : 
	pop rax 	#ret addr -> rax
	mov [rbp + 2 * 8], rax 
	sub rsp , 8
	push rbp 
	push 999 
	mov rbp , rsp 
	call func_2_1 
	mov rsp , rbp 
	add rsp , 8
	pop rbp 
	ret 

func_2_1 : 
	pop rax 	#ret addr -> rax
	mov [rbp + 2 * 8], rax 
	mov rsp , rbp 
	add rsp , 8
	pop rbp 
	ret 

