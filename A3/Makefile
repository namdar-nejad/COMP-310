mykernel: kernel.c shell.o interpreter.o shellmemory.o kernel.o ram.o cpu.o pcb.o memorymanager.o
	gcc -o mykernel shell.o interpreter.o shellmemory.o kernel.o ram.o cpu.o pcb.o memorymanager.o
	
kernel.o: kernel.c ram.h pcb.h cpu.h kernel.h interpreter.h memorymanager.h
	gcc -c kernel.c

shell.o: shell.c interpreter.h shell.h
	gcc -c shell.c

interpreter.o: interpreter.c interpreter.h shellmemory.h shell.h kernel.h memorymanager.h ram.h
	gcc -c interpreter.c

shellmemory.o: shellmemory.c shellmemory.h
	gcc -c shellmemory.c

memorymanager.o: memorymanager.c memorymanager.h ram.h kernel.h pcb.h
	gcc -c memorymanager.c

ram.o: ram.c ram.h memorymanager.h pcb.h
	gcc -c ram.c

cpu.o: cpu.c cpu.h shell.h interpreter.h ram.h memorymanager.h
	gcc -c cpu.c

pcb.o: pcb.c pcb.h memorymanager.h
	gcc -c pcb.c

clean:
	rm -f mykernel shell.o interpreter.o shellmemory.o kernel.o ram.o cpu.o pcb.o memorymanager.o