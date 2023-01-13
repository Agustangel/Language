
INCLUDES=include
SOURCE=source

INCL=CPU_/cpu-include
CPU=CPU_/cpu
ASM=CPU_/asm

lang: main.o frontend.o tree.o backend.o stack.o
	gcc -o lang $^ -llogger -lonegin -lm

main.o: main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DPTR -DREMOVE_SAFETY

frontend.o: ${SOURCE}/frontend.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DPTR -DREMOVE_SAFETY

tree.o: ${SOURCE}/tree.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DPTR -DREMOVE_SAFETY

backend.o: ${SOURCE}/backend.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DPTR -DREMOVE_SAFETY

stack.o: ${SOURCE}/stack.c
	gcc -g -O0 -I${INCLUDES}/ -c $^ -DPTR -DREMOVE_SAFETY


valgrind: lang
	valgrind --leak-check=yes ./lang

clean:
	rm lang main.o frontend.o backend.o tree.o stack.o



asm: asm_main.o asm.o
	gcc -o asm_bin $^ -llogger -lonegin -lstack

asm_main.o: ${ASM}/asm_main.c
	gcc -g -O0 -I${INCL}/ -c $^ -DCHAR

asm.o: ${ASM}/asm.c
	gcc -g -O0 -I${INCL}/ -c $^ -DCHAR


cpu: cpu_main.o cpu.o
	gcc -o cpu_bin $^ -llogger -lonegin -lstack -lm

cpu_main.o: $(CPU)/cpu_main.c
	gcc -g -O0 -I${INCL}/ -c $^ -DCHAR

cpu.o: $(CPU)/cpu.c
	gcc -g -O0 -I${INCL}/ -c $^ -DCHAR

.PHONY: clean
clean:
	rm asm cpu disasm asm_main.o assembler.o
