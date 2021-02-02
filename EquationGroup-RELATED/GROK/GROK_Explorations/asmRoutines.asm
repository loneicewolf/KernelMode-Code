.code

ASM_GetNextIPAddress proc
	mov rax, [rsp]
	ret
ASM_GetNextIPAddress endp

ASM_MemorySwitch proc
	mov [rsp], rcx
	ret
ASM_MemorySwitch endp

ASM_HashExportedFn proc
	push rbx
	test rcx, rcx
	mov rax, rcx
	jz exit
	mov cl, [rcx]
	xor edx, edx
loopHashChar:
	movsx ecx, cl
	mov ebx, 0f673b679h
	imul ecx, ebx
	xor edx, ecx
	add rax, 1
	mov cl, [rax]
	test cl, cl		;test NUL byte
	jnz loopHashChar
	mov eax, edx		;return the hash
exit:
	pop rbx
	ret
ASM_HashExportedFn endp

ASM_CallRsi proc
	call rsi
ASM_CallRsi endp

cleanupEverybodyCleanup proc
	add rsp, 68h
	mov r10, [rbx]		; ogReturnAddress, push and call it
	push r10
	mov rsi, [rbx+8h]
	mov rbp, [rbx+18h]
	mov rbx, [rbx+10h]
	ret
cleanupEverybodyCleanup endp



ASM_HiddenCall proc
	pop rax
	mov r11, rax	; store return address in r11
	xor rax, rax
	mov rax, rsp
	xor rax, rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	push rax
	mov rax, r11
	mov [rsp+60h], rax
	mov [rsp+68h], rdx
	mov [rsp+70h], r8
	mov [rsp+78h], r9
	mov r11, rcx
	shl r11, 3		; determine the # of args
	mov r10, [rsp+r11+70h]
	push r10
	mov r10, [rsp+r11+70h]
	push r10
	cmp rcx, 0
	jz runTheRoutine
	dec ecx
	jz prepareFirstArg
	dec ecx
	jz prepareSecondArg
	dec ecx
	jz prepareThirdArg
	mov r9, [rsp+90h]
prepareThirdArg:
	mov r8, [rsp+88h]
prepareSecondArg:
	mov rdx, [rsp+80h]
prepareFirstArg:
	mov rcx, [rsp+78h]
	push rbx
	xor r10, r10
loopStart:
	cmp r10, r11
	jz exitLoop
	mov rbx, [rsp+r10+80h]
	mov [rsp+r10+18h], rbx
	add r10, 8
	jmp loopStart
exitLoop:
	pop rbx
runTheRoutine:
	mov [rsp+r11+70h], rax
	mov [rsp+r11+78h], rsi
	mov [rsp+r11+80h], rbx
	mov [rsp+r11+88h], rbp
	xor rbp, rbp
	lea rbx, [rsp+r11+70h]			; store ogReturn in rbx
	lea rsi, cleanupEverybodyCleanup	; you know it
	ret
ASM_HiddenCall endp


ASM_LocateImageBaseFromRoutine proc
	mov [rsp+8h], rbx
	mov [rsp+10h], rbp
	mov [rsp+18h], rsi
	push r12
	sub rsp, 20h
	mov esi, edx
	mov rbp, r9
	mov r12, r8
	lea rbx, [rsi-1]
	not rbx
	and rbx, rcx
checkDosSignature:
	cmp dword ptr [rbx], 905a4dh
	jnz subtractAndKeepGoing
	cmp dword ptr [rbx+3Ch], 4000h
	jge subtractAndKeepGoing
	movsxd rax, dword ptr [rbx+3Ch]
	cmp dword ptr [rax+rbx], 4550h
	jz foundImageBase
subtractAndKeepGoing:
	sub rbx, rsi
	jmp checkDosSignature
foundImageBase:
	mov [r12], rbx    ;store image base in 3rd param
	mov rsi, [rsp+40h]
	mov rbx, [rsp+30h]
	mov rbp, [rsp+38h]
	add rsp, 20h
	pop r12
	ret
ASM_LocateImageBaseFromRoutine endp


ASM_FixRelocations proc
	mov [rsp+8], rbx
	mov [rsp+20h], r9
	push rbp
	push rsi
	push rdi
	push r12
	push r13
	push r14
	push r15
	sub rsp, 20h
	mov rdi, rcx
	movsxd rax, dword ptr [rcx+3Ch]
	mov r12d, edx
	mov edx, [rax+rcx+0B0h]
	add r12, rcx
	mov ecx, [rax+rcx+0B4h]
	test edx, edx            ; nothing to relcoate
	jz statusSuccess
	test ecx, ecx            ; nothing to relocate
	jz statusSuccess
getRelocAndCalculateDelta:
	lea rbx, [rdi+rdx]       ; rbx = reloc
	mov r13, rdi
	sub r13, [rax+rdi+30h]   ; delta
	lea rbp, [rbx+rcx]
loopStart:
	cmp rbx, rbp
	jge statusSuccess
	mov esi, [rbx+4]
	and dword ptr [rsp+70h], 0h
	lea r15, [rbx+8]
	sub rsi, 8
	shr rsi, 1
	test esi, esi
	jz getNextEntry
innerLoop:
	mov dx, [r15]
	mov ecx, edx
	and ecx, 0FFFh
	add ecx, [rbx]
	add rcx, rdi
	cmp rcx, rdi
	jb statusFailed
	cmp rcx, r12
	jge statusFailed
	shr dx, 0Ch
	movzx eax, dx
	cmp eax, 0Ah
	jnz shouldNeverHappen
	add [rcx], r13
shouldNeverHappen:
	xor eax, eax
	mov r8d, [rsp+70h]
	mov r9, [rsp+78h]
	add r8d, 1
	add r15, 2
	cmp r8d, esi
	mov [rsp+70h], r8d
	jb innerLoop
getNextEntry:
	mov eax, [rbx+4]
	add rbx, rax
	jmp loopStart
statusFailed:
	mov eax, 0C0000001h
	jmp exit
statusSuccess:
	xor eax, eax
exit:
	mov rbx, [rsp+60h]
	add rsp, 20h
	pop r15
	pop r14
	pop r13
	pop r12
	pop rdi
	pop rsi
	pop rbp
	ret
ASM_FixRelocations endp

ASM_StriCmp proc
	mov r11, rcx
	mov r10, rdx
loopStart:
	movzx r8d, byte ptr [r11]
	inc r11
	movzx edx, byte ptr [r10]
	inc r10
	lea eax, [r8-41h]
	cmp eax, 19h
	lea r9d, [r8+20h]
	lea ecx, [rdx-41h]
	cmova r9d, r8d
	lea eax, [rdx+20h]
	cmp ecx, 19h
	cmova eax, edx
	test r9d, r9d				;check the NUL byte
	je exit
	cmp r9d, eax
	je loopStart
exit:
	sub r9d, eax
	mov eax, r9d
	ret
ASM_StriCmp endp


end
