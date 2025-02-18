; InDirect syscalls

.data

EXTERN syscallJumpAddr:QWORD
EXTERN virtualAllocCallNumber:DWORD

.code

DirectVirtualAlloc PROC
	mov r10, rcx
	mov eax, virtualAllocCallNumber
	jmp qword ptr [syscallJumpAddr]
	ret
DirectVirtualAlloc endp


end