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

	; ������������� ���������� 'a'   
SPU_IN

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������������� ���������� 'b'   
SPU_IN

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������������� ���������� 'c'   
SPU_IN

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������ ����� if
PUSH [AX + 1]
PUSH 0

JNE if_mark_0:

	; ����� ������� 'Linerial'   
PUSH AX   
	; �������� ���������� � ������� 
	; �������� ���������� � ������� 
PUSH [AX + 3]
PUSH [AX + 2]
PUSH BX   
POP AX   
CALL Linerial: 
POP CX  
PUSH AX  
POP BX  
POP AX  
	PUSH CX  
	; ����� ������ ������� 'Linerial'   
SPU_OUT

PUSH 0

RET
if_mark_0:
	; ����� ����� if

	; ������������� ���������� 'discr'   
PUSH [AX + 2]
PUSH [AX + 2]
MUL 
PUSH 4

PUSH [AX + 1]
MUL 
PUSH [AX + 3]
MUL 
SUB 

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

PUSH [AX + 4]
SPU_OUT

	; ������ ����� if
PUSH [AX + 4]
PUSH 0

JA if_mark_1:

PUSH 666

SPU_OUT

PUSH 0

RET
if_mark_1:
	; ����� ����� if

	; ������ ����� if
PUSH [AX + 4]
PUSH 0

JNE if_mark_2:

PUSH 0

PUSH [AX + 2]
SUB 
PUSH 2

DIV 
PUSH [AX + 1]
DIV 
SPU_OUT

PUSH 0

RET
if_mark_2:
	; ����� ����� if

	; ������������� ���������� 'x1'   
PUSH 0

PUSH [AX + 2]
SUB 
PUSH [AX + 4]
SQRT 
SUB 
PUSH 2

DIV 
PUSH [AX + 1]
DIV 

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������������� ���������� 'x2'   
PUSH 0

PUSH [AX + 2]
SUB 
PUSH [AX + 4]
SQRT 
ADD 
PUSH 2

DIV 
PUSH [AX + 1]
DIV 

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

PUSH [AX + 5]
SPU_OUT

PUSH [AX + 6]
SPU_OUT

PUSH 0

RET
	; ����� ������������� ������� 'MAIN_HOI'   

	; ������������� ������� 'Linerial'   
Linerial:

	; ������������� ���������� 'b'   

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������������� ���������� 'c'   

POP [BX] 
PUSH BX   
PUSH 1    
ADD      
POP BX   

	; ������ ����� if
PUSH [AX + 1]
PUSH 0

JNE if_mark_3:

PUSH 666

RET
if_mark_3:
	; ����� ����� if

PUSH [AX + 1]
PUSH [AX + 0]
DIV 
RET
	; ����� ������������� ������� 'Linerial'   
