.386
.model flat, stdcall
.stack 4096

findTilePosASM PROTO C vectorAddress:DWORD

.data
	tileSize DWORD 64
	;aFloat REAL4 24.5

.code
findTilePosASM PROC C vectorAddress:DWORD

	push ecx
	push eax

	mov ecx, vectorAddress	; ecx holds address of vector in heap
	mov eax, [ecx]			; eax holds current value being worked on

	; Compute Tile x coord
	div tileSize
	mul tileSize

	; Place back in heap vector
	mov [ecx], REAL4 PTR eax

	xor eax, eax
	mov eax, [ecx + 4]

	; Compute Tile y coord
	div tileSize
	mul tileSize

	; Place back in heap vector
	mov [ecx + 4], REAL4 PTR eax

	pop eax
	pop ecx

	ret

findTilePosASM ENDP
END



; For Vector2 / floating point functions:

	;mov eax, vectorAddress

	;finit

	;fld DWORD PTR [eax]
	;fld DWORD PTR [eax + 4]
	;fadd