__vectors:
 	jmp	0x68	; 0x68 <__ctors_end>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>
 	jmp	0xa2	; 0xa2 <__bad_interrupt>

__ctors_end:
   eor	r1, r1
   out	0x3f, r1	
   ldi	r28, 0xFF	
   ldi	r29, 0x08	
   out	0x3e, r29	
   out	0x3d, r28	

__do_copy_data:
  ldi	r17, 0x01	
  ldi	r26, 0x00	
  ldi	r27, 0x01	
  ldi	r30, 0x22	
  ldi	r31, 0x01	
  rjmp do_copy_data_start

do_copy_data_loop:
  lpm	r0, Z+
  st	X+, r0

do_copy_data_start:
  cpi	r26, 0x00	
  cpc	r27, r17
  brne do_copy_data_loop

__do_clear_bss:
  ldi	r17, 0x01	
  ldi	r26, 0x00	
  ldi	r27, 0x01	
  rjmp do_clear_bss_start

do_clear_bss_loop:
 	st	X+, r1

do_clear_bss_start:
 	cpi	r26, 0x00	; 0
 	cpc	r27, r17
 	brne	do_clear_bss_loop
 	call	main
__bad_interrupt
idle: 	
  rjmp idle
































































0000011e <_exit>:
 11e:	f8 94       	cli

00000120 <__stop_program>:
 120:	ff cf       	rjmp	.-2      	; 0x120 <__stop_program>
