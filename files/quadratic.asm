
push 0
pop rfx
call :main
hlt

main:

in
pop [rfx + 0]
in
pop [rfx + 1]
in
pop [rfx + 2]

push 6
push rfx
add
pop rfx

call :solveSquare

push rfx
push 6
sub
pop rfx



isZero:
pop [rfx + 0]


If_begin0:
push 0
jne :If_end0

push 1
ret

If_end0:

push 0
ret

solveLinear:
pop [rfx + 0]
pop [rfx + 1]

push -1
mul 
div 
ret

solveSquare:
pop [rfx + 0]
pop [rfx + 1]
pop [rfx + 2]


If_begin1:

push 36
push rfx
add
pop rfx

call :isZero

push rfx
push 36
sub
pop rfx


pop [rfx + 4]
out
push 0
ret

If_end1:


If_begin2:

push 36
push rfx
add
pop rfx

call :isZero

push rfx
push 36
sub
pop rfx


pop [rfx + 9]
out
push 0
out
push 0
ret

If_end2:

push 2
pow 
push 4
mul 
mul 
sub 
pop [rfx + 13]
push -1
mul 
push 2
mul 
div 
pop [rfx + 17]

If_begin3:
push 0
jbe :If_end3

pop [rfx + 21]
push 2
mul 
div 
pop [rfx + 23]
add 
pop [rfx + 26]
add 
pop [rfx + 29]
out
out

If_end3:


If_begin4:

push 36
push rfx
add
pop rfx

call :isZero

push rfx
push 36
sub
pop rfx


out

If_end4:


