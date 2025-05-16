PUSH 0                     
POP AX                   

PUSH 0                     
POP BX                   

PUSH AX                    
PUSH 0                   

CALL MAIN_HOI:   
SPU_OUT                    
HLT                        

	; инициализация функции 'MAIN_HOI'   
MAIN_HOI:

	; инициализация переменной 'arg'   

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

PUSH 1

PUSH 2

ADD 
	; конец инициализации функции 'MAIN_HOI'   
