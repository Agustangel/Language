
INCLUDES=include
SOURCE=source

lang: main.o frontend.o tree.o
	gcc -o lang $^ -llogger -lonegin -lm

main.o: main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

frontend.o: ${SOURCE}/frontend.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

tree.o: ${SOURCE}/tree.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

valgrind: lang
	valgrind --leak-check=yes ./lang

clean:
	rm lang main.o frontend.o tree.o