.386
.model flat, stdcall
.stack 4096

findTilePosASM PROTO C vectorAddress:DWORD

.code
findTilePosASM PROC C vectorAddress:DWORD

	finit

	fld vectorAddress
	fld vectorAddress
	fadd

	ret

findTilePosASM ENDP
END
