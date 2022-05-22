mysh: shell.o interpreter.o shellmemory.o
	gcc -o mysh shell.o interpreter.o shellmemory.o

shell.o: shell.c interpreter.h shellmemory.h shell.h
	gcc -c shell.c

interpreter.o: interpreter.c interpreter.h shellmemory.h shell.h
	gcc -c interpreter.c

shellmemory.o: shellmemory.c shellmemory.h
	gcc -c shellmemory.c

clean:
	rm -f mysh shell.o interpreter.o shellmemory.o