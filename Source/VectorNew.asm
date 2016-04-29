.386
.model flat, stdcall
.stack 4096

findTilePosASM PROTO C vectorAddress:DWORD
checkPointCollisionASM PROTO C rectAddress:DWORD, vectorAddress:DWORD

.data
	tileSize DWORD 64d
	tempInt DWORD 0d
	tempFloat REAL4 0.0

.code
			; findTilePosASM is a function which takes a pair of coordinates (a Vector2) and
			; determines the coordinates of the tile to which that point rests in. 
			; It is used at line 305 of Game.cpp for addTileToRedraw() Where you can see it
			; is currently being used instead of the c++ implementation.
		
			; How it looks in c++:
			; toReturn.x = ((x / 64) * 64);
			; toReturn.y = ((y / 64) * 64);
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

			; checkPointCollisionASM is a function which checks if a point lies within a rectangle,
			; and returns true or false. Comparisons are done in opposite (< to >=) because to continue 
			; through the checks means to not jump down to the false return.

			; How it looks in c++:
			; if (point.x >= rect.mPosition.x 
			; && point.x <= rect.mPosition.x + rect.mSize.x 
			; && point.y >= rect.mPosition.y 
			; && point.y <= rect.mPosition.y + rect.mSize.y)
			; return true 
			; else return false
checkPointCollisionASM PROC C rectAddress:DWORD, vectorAddress:DWORD

	push ecx
	push edx

	xor ecx, ecx
	xor edx, edx
	xor eax, eax

	mov ecx, rectAddress		; ecx holds address of Rect in heap
	mov edx, vectorAddress		; edx holds address of Vector2 in heap

	; Is vector.x < rect.x
	finit
	fld DWORD PTR [ecx]
	fld DWORD PTR [edx]

	fcomi st, st(1)				; cmp for floats
	jb GOTO_FALSE

	; Is vector.x > rect.x + rect.width
	finit	
	fld DWORD PTR [ecx]
	fld DWORD PTR [ecx + 8]
	fadd st(0), st(1)			; rect.x + rect.width
	fld DWORD PTR [edx]

	fcomi st, st(1)
	ja GOTO_FALSE

	; Is vector.y < rect.y
	finit
	fld DWORD PTR [ecx + 4]
	fld DWORD PTR [edx + 4]

	fcomi st, st(1)
	jb GOTO_FALSE

	; Is vector.y > rect.y + rect.height
	finit	
	fld DWORD PTR [ecx + 4]
	fld DWORD PTR [ecx + 12]
	fadd st(0), st(1)			; rect.y + rect.height
	fld DWORD PTR [edx + 4]

	fcomi st, st(1)
	ja GOTO_FALSE
	
	; Return true, if all test cases are true
	pop edx
	pop ecx

	xor eax, eax
	mov eax, 1
	ret

	; Return false
	GOTO_FALSE:

	pop edx
	pop ecx

	xor eax, eax
	ret

checkPointCollisionASM ENDP

END