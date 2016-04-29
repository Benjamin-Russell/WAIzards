.386
.model flat, stdcall
.stack 4096

findTilePosASM PROTO C vectorAddress:DWORD

.data
	tileSize DWORD 64d
	tempInt DWORD 0d
	tempFloat REAL4 0.0

.code
findTilePosASM PROC C vectorAddress:DWORD

	push ecx

	xor ecx, ecx
	xor eax, eax

	mov ecx, vectorAddress		; ecx holds address of Vector2 in heap

	; Prepare for x coord
	finit
	fld DWORD PTR [ecx]
	fistp tempInt				; Converting Floating point value to integer
	mov eax, tempInt

	; Compute Tile x coord
	xor edx, edx
	div tileSize
	xor edx, edx
	mul tileSize

	; Place back in heap Vector2 as floating point again
	mov tempInt, eax
	fild tempInt
	fstp tempFloat	
	mov eax, tempFloat
	mov [ecx], eax

	; Prepare for y coord
	xor eax, eax
	fld DWORD PTR [ecx + 4]
	fistp tempInt
	mov eax, tempInt

	; Compute Tile y coord
	xor edx, edx
	div tileSize
	xor edx, edx
	mul tileSize

	; Place back in heap Vector2 as floating point again
	mov tempInt, eax
	fild tempInt
	fstp tempFloat
	mov eax, tempFloat
	mov [ecx + 4], eax

	; Give address of the vector2 of the tile to eax
	mov eax, ecx

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