push 0
pop rax
call main:
hlt
main:
in
pop [rax + 0]
in
pop [rax + 1]
in
pop [rax + 2]
push 6
push rax
add
pop rax
call solveSquare:
push rax
push 6
sub
pop rax
solveSquare:
pop [rax + 0]
pop [rax + 1]
pop [rax + 2]
IfBegin0:
push 36
push rax
add
pop rax
call isZero:
push rax
push 36
sub
pop rax
pop [rax + 4]
out
push 0
ret
IfEnd0:
IfBegin1:
push 36
push rax
add
pop rax
call isZero:
push rax
push 36
sub
pop rax
pop [rax + 9]
out
push 0
out
push 0
ret
IfEnd1:
push 2
pow 
push 4
mul 
mul 
sub 
pop [rax + 13]
push -1
mul 
push 2
mul 
div 
pop [rax + 17]
IfBegin2:
push 0
ja  IfEnd2:
pop [rax + 21]
push 2
mul 
div 
pop [rax + 23]
add 
pop [rax + 26]
add 
pop [rax + 29]
out
out
IfEnd2:
IfBegin3:
push 36
push rax
add
pop rax
call isZero:
push rax
push 36
sub
pop rax
out
IfEnd3:
isZero:
pop [rax + 0]
IfBegin4:
push 0
je  IfEnd4:
push 1
ret
IfEnd4:
push 0
ret
solveLinear:
pop [rax + 0]
pop [rax + 1]
push -1
mul 
div 
ret
