pre_main:
	mov eax, 3
	mov ebx, .data
	int 0x50
	nop
	ret

.data: db 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 10, 0

