
INCLUDES=include
SOURCE=source

lang: main.o lang.o
	gcc -o lang $^ -llogger -lonegin -lm

main.o: main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

lang.o: ${SOURCE}/lang.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

valgrind: lang
	valgrind --leak-check=yes ./lang

clean:
	rm lang main.o lang.o