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

	; ������������� ���������� 'a'   

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������������� ���������� 'i'   
PUSH 0


POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������ ����� while
while_start_mark_0:
PUSH [AX + 1]
PUSH 500

JA while_end_mark_0:

	; ����� ������� 'fact'   
PUSH AX   
	; �������� ���������� � ������� 
PUSH 5

PUSH BX   
POP AX   
CALL fact: 
POP CX  
PUSH AX  
POP BX  
POP AX  
	PUSH CX  
	; ����� ������ ������� 'fact'   

	; ������������ �������� ���������� 'i'
PUSH [AX + 1]
PUSH 1

ADD 
POP [AX + 1]
JMP while_start_mark_0:  
while_end_mark_0:       
	; ����� ����� while

PUSH 0

RET
	; ����� ������������� ������� 'MAIN_HOI'   

	; ������������� ������� 'fact'   
fact:

	; ������������� ���������� 'n'   

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������ ����� if
PUSH [AX + 0]
PUSH 1

JA if_mark_0:

PUSH 1

RET
if_mark_0:
	; ����� ����� if

PUSH [AX + 0]
	; ����� ������� 'fact'   
PUSH AX   
	; �������� ���������� � ������� 
PUSH [AX + 0]
PUSH 1

SUB 
PUSH BX   
POP AX   
CALL fact: 
POP CX  
PUSH AX  
POP BX  
POP AX  
	PUSH CX  
	; ����� ������ ������� 'fact'   
MUL 
RET
	; ����� ������������� ������� 'fact'   
