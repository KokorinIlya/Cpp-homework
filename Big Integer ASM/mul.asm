section         .text

global          _start

%define First rdi
%define Second rsi
%define size rcx
%define short rbx
%define stack_pointer rsp
%define Result rdx
%define FirstCopy rbp

_start:
	sub 			stack_pointer, 4 * 128 * 8 ;allocate memory

	lea 			First, [stack_pointer + 128 * 8] ;lea moves adress of the second operand to the first. x = [sp + 128 * 8]
                                                                 ;lea moves adress of x to Firsrt => First = sp + 128 * 8
	mov 			size, 128
	call 			read_long 					    ;reads first number to rdi
	mov 			First, stack_pointer                        ;in First now adress of the begining of the first number

	call 			read_long 					    ;reads second number to rsi
	lea                     Second, [stack_pointer + 128 * 8]  ;in Second adress of the begining of the first number

	lea 			FirstCopy, [Second +  128 * 8]                       ;requests memory for the FirstCopy
	lea 			Result, [FirstCopy + 128 * 8]                       ;requests memory for the result

	call 			mul_long_long
	mov 			First, Result
	call 			write_long
	
	push 			rax        ;starts writing endline
	mov 			rax, 0x0a
	call 			write_char
	pop 			rax        ;finish writing endline

	jmp             exit

mul_long_long:
	push            Second						
	push            size
	push 		First 
	push 		Result	
	push 		FirstCopy
	push 		rax
	push 		rdx

	mov 		rax, First
	mov 		First, Result
	call 		set_zero
	mov 		First, rax						; result = 0

	clc										
	.loop:			;loop will be done (size) times
			
		call 			array_copy 							; FirstCopy = copy of First

		mov                     short, [Second]								; c = Second[i]
		call		 	mul_long_short 								; First *= c
		mov 			rax, Second							        ; rax = Second
		mov 			Second, First								; Second = First
		mov 			First, Result								; First = Result
		call 			add_long_long       						; First += Second 
                                                                                            ;Result += FirstNumber * c * (2^64)^i
		mov 			First, Second 								; First = Second
		mov 			Second, rax 								; Second = rax
		lea             Second, [Second + 8]						; Second[i] -> Second[i + 1]

		mov 			rax, First
		mov 			First, FirstCopy
		mov 			FirstCopy, rax								; swap(First, FirstCopy)
		call 			array_copy 									; FirstCopy = First
		mov 			rax, First
		mov 			First, FirstCopy
		mov 			FirstCopy, rax								; swap(First, FirstCopy)

		call 			shift_left							; array(First) *= 2^64

		dec             size										; i -> i++
		jnz             .loop										; while size > 0

	pop 			rdx
	pop 			rax
	pop 			FirstCopy
	pop 			Result
	pop 			First
	pop             size			
	pop             Second
	ret

array_copy:                                     ;copies First to FirstCopy
	push 			First
	push 			FirstCopy
	push 			size
	push 			rax

	mov 			size, 128

	.loop:
		mov 			rax, [First]
		mov 			[FirstCopy], rax ; moves from First to FirstCopy
		add                     First, 8
		add                     FirstCopy, 8 ; First[i] -> First[i + 1], FirstCopy[i] -> FirstCopy[i + 1]
		dec 			size       ; loop will be done 128 times, for each i: 1..128
		jnz 			.loop

	pop 			rax
	pop 			size
	pop 			FirstCopy
	pop 			First
	ret


shift_left:							;First *= 2^64, [x128, x127, ..., x1] -> [x127, x126, ..., 0]
	push 			First
	push 			size
	push 			rax
	push 			rbx

	mov 			size, 127
	lea 			First, [First + 127 * 8] ;start moving from the 127th element 

	.loop:
		mov 			rax, First
		sub 			First, 8
		mov 			rbx, [First]
		mov 			[rax], rbx     ;First[i] = First[i - 1]
		dec 			size           ;cicle will bo done 127 times, First[128] will be lost
		jnz 			.loop


        mov                     rax, 0
	mov 			[First], rax        ;First[1] = 0

	pop 			rbx
	pop 			rax
	pop 			size
	pop 			First
	ret



add_long_long:					;First += Second
	push            First			
	push            Second
	push            size
	push 	        rax

	mov 			size, 128
	clc				
	.loop:						
		mov             rax, [Second]			   
		adc             [First], rax		;adding with carry flag, First[i] += Second[i]
		lea             First, [First + 8]      ;First[i] -> First[i + 1]
                lea             Second, [Second + 8]    ;Second[i] -> Second[i + 1]
		dec             size	;cycle will be done 128 times, for each i: 1..128		
		jnz             .loop			

	pop 	        rax
	pop 	        size
	pop             Second
	pop             First
	ret

; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:				
	push            First			
	push            size			
	push            Result		

	clc
	xor             rdx, rdx			
	.loop:			
		add            [First], rax	;FIrst[i] += x		
		adc             rdx, 0	;rdx = carry		
		mov             rax, rdx	;x = carry		
		xor             rdx, rdx	;carry = 0		
		add             First, 8 ;First[i] -> First[i + 1]			
		dec             rcx			
		jnz             .loop			

	pop             Result			
	pop             size			
	pop             First			
	ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to rdi
mul_long_short:			
	push            rax			
	push            First			
	push            size			
	push 	        Second
	push 	        short
	push 	        Result

	clc
	mov 		size, 128
	mov             rsi, 0			
	.loop:			
		mov             rax, [First]			; rsi - carry
		mul             short				; rdx:rax = rax * short, rdx will be used as carry the next time
		add             rax, rsi			; rax += rsi, pluses carry, makes CarryFlag
		adc             rdx, 0				; rdx += CarryFlag
		mov             [First], rax			; First[i] = rax
		add             First, 8		        ; First[i] -> First[i + 1]
		mov             rsi, rdx			; rsi = rdx, updates carry
		dec             size				
		jnz             .loop			

	pop 	        Result
	pop 	        short
	pop 	        Second
	pop             size			
	pop             First	
	pop             rax			
	ret

; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:			
	push            rdi			
	push            rax			
	push            rcx			
	push 	        rbx

	clc
	lea             rdi, [rdi + 8 * rcx - 8]			
	xor             rdx, rdx			

	.loop:			
		mov             rax, [rdi]			
		div             rbx			
		mov             [rdi], rax			
		sub             rdi, 8			
		dec             rcx			
		jnz             .loop			

	pop 			rbx
	pop             rcx			
	pop             rax			
	pop             rdi			
	ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:			
	push            rax			
	push            rdi			
	push            rcx			
	push  			rax

	xor             rax, rax			
	rep stosq			

	pop 			rax
	pop             rcx			
	pop             rdi			
	pop             rax			
	ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:			
	push            rax			
	push            rdi			
	push            rcx			
	push 			rax

	xor             rax, rax			
	rep scasq			

	pop 			rax
	pop             rcx			
	pop             rdi			
	pop             rax			
	ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:			
	push            rcx			
	push            rdi			
	push 			rax

	call            set_zero			
	.loop:			
	call            read_char			
	or              rax, rax			
	js              exit			
	cmp             rax, 0x0a			
	je              .done			
	cmp             rax, '0'			
	jb              .invalid_char			
	cmp             rax, '9'			
	ja              .invalid_char			

	sub             rax, '0'			
	mov             rbx, 10			
	call            mul_long_short			
	call            add_long_short			
	jmp             .loop			

.done:			
	pop 			rax
	pop             rdi			
	pop             rcx			
	ret

.invalid_char:			
	mov             rsi, invalid_char_msg			
	mov             rdx, invalid_char_msg_size			
	call            print_string			
	call            write_char			
	mov             al, 0x0a			
	call            write_char			

.skip_loop:			
	call            read_char			
	or              rax, rax			
	js              exit			
	cmp             rax, 0x0a			
	je              exit			
	jmp             .skip_loop			

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:			
	push            rax			
	push            rcx			
	push 			rbp
	push 			rsi
	push 			rbx
	push 			rdx
	push 			stack_pointer

	mov             rax, 20			
	mul             rcx			
	mov             rbp, stack_pointer			
	sub             stack_pointer, rax			

	mov             rsi, rbp			

	.loop:			
		mov             rbx, 10			
		call            div_long_short			
		add             rdx, '0'			
		dec             rsi			
		mov             [rsi], dl			
		call            is_zero			
		jnz             .loop			

	mov             rdx, rbp			
	sub             rdx, rsi			
	call            print_string			

	mov             stack_pointer, rbp			

	pop 			stack_pointer
	pop 			rdx
	pop 			rbx
	pop 			rsi
	pop 			rbp
	pop             rcx			
	pop             rax			
	ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax
                push            rdi

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rdi
                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg
