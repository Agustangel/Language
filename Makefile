
INCLUDES=include
SOURCE=source

lang: main.o Frontend.o
	gcc -o lang $^ -llogger -lonegin -lm

main.o: main.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

Frontend.o: ${SOURCE}/Frontend.c
	gcc -g -O0 -I${INCLUDES}/ -c $^

valgrind: lang
	valgrind --leak-check=yes ./lang

clean:
	rm lang main.o Frontend.o