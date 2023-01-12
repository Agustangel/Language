
INCLUDES=include
SOURCE=source

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