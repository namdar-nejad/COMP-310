mykernel: shell.o interpreter.o shellmemory.o kernel.o ram.o cpu.o pcb.o 
	gcc -o mykernel shell.o interpreter.o shellmemory.o kernel.o ram.o cpu.o pcb.o

shell.o: shell.c interpreter.h shellmemory.h shell.h
	gcc -c shell.c

interpreter.o: interpreter.c interpreter.h shellmemory.h shell.h kernel.h
	gcc -c interpreter.c

shellmemory.o: shellmemory.c shellmemory.h
	gcc -c shellmemory.c

kernel.o: kernel.c shell.h ram.h pcb.h cpu.h kernel.h
	gcc -c kernel.c

ram.o: ram.c ram.h shell.h
	gcc -c ram.c

cpu.o: cpu.c cpu.h kernel.h interpreter.h ram.h
	gcc -c cpu.c

pcb.o: pcb.c pcb.h
	gcc -c pcb.c

clean:
	rm -f mysh shell.o interpreter.o shellmemory.o kernel.o ram.o cpu.o pcb.o