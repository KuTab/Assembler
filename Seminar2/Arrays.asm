format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize   db 'size of vector? ', 0
        strIncorSize db 'Incorrect size of vector = %d', 10, 0
        strVecElemI  db '[%d]? ', 0
        strScanInt   db '%d', 0
        strVector    db 'Vector A:',10,0
        strNewVector db 'Vector B:',10,0
        strVecElemOut  db '[%d] = %d', 10, 0
        strNegIndex  db 'Negative index = %d',10,0

        vec_size     dd 0
        negi         dd -1
        sum          dd 0
        i            dd ?
        tmp          dd ?
        tmpStack     dd ?
        vec          rd 100
        newvec       rd 100

;--------------------------------------------------------------------------
section '.code' code readable executable
start:
        call VectorInput

        push strVector
        call [printf]
        call VectorOut

        call VectorNegative

        call NewVecCreate

        push strNewVector
        call[printf]

        call NewVectorOut
finish:
        call [getch]

        push 0
        call [ExitProcess]

;--------------------------------------------------------------------------
VectorInput:
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        mov eax, [vec_size]
        cmp eax, 0
        jg  getVector

        push vec_size
        push strIncorSize
        call [printf]
        push 0
        call [ExitProcess]

getVector:
        xor ecx, ecx
        mov ebx, vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector

        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:
        ret
;--------------------------------------------------------------------------
VectorNegative:
        xor ecx,ecx
        mov ebx,vec
negLoop:
        cmp ecx,[vec_size]
        je endVectorNegative
        mov eax,[ebx]
        cmp eax,0
        jl negative
        inc ecx
        add ebx,4
        jmp negLoop
negative:
        mov [negi],ecx
        inc ecx
        add ebx,4
        jmp negLoop
endVectorNegative:
        ret
;--------------------------------------------------------------------------
NewVecCreate:
        xor ecx,ecx
        mov ebx,vec
        mov edx,newvec
newVecLoop:
        cmp ecx,[vec_size]
        je endNewVecCreate
        cmp [negi],0
        jl value
        cmp ecx,[negi]
        jne value
        inc ecx
        add ebx,4

value:
        mov eax,[ebx]
        mov [edx],eax
        inc ecx
        add ebx,4
        add edx,4
        jmp newVecLoop
endNewVecCreate:
        ret
;--------------------------------------------------------------------------
NewVectorOut:
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, newvec
        cmp [negi],0
        jge decrement
NewOutputVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endNewOutputVector
        mov [i], ecx

        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp NewOutputVecLoop
endNewOutputVector:
        mov esp, [tmpStack]
        ret
decrement:
        mov eax,[vec_size]
        dec eax
        mov [vec_size],eax
        jmp NewOutputVecLoop
;--------------------------------------------------------------------------
VectorOut:
        mov [tmpStack], esp
        xor ecx, ecx
        mov ebx, vec
putVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector
        mov [i], ecx

        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop
endOutputVector:
        mov esp, [tmpStack]
        ret
;-------------------------------third act - including HeapApi--------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'    
