PUSH 0                     
POP AX                   

PUSH 0                     
POP BX                   

PUSH AX                    
PUSH 0                   

CALL MAIN_HOI:   
SPU_OUT                    
HLT                        

	; ������������� ������� 'MAIN_HOI'   
MAIN_HOI:

	; ������������� ���������� 'arg'   

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

PUSH 1

PUSH 2

ADD 
	; ����� ������������� ������� 'MAIN_HOI'   
