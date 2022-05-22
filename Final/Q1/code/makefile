mykernel: kernel.o cpu.o ram.o shell.o interpreter.o pcb.o shellmemory.o memorymanager.o DISK_driver.o DISK_driver_problem1.o
	gcc -o mykernel kernel.o cpu.o ram.o shell.o interpreter.o pcb.o shellmemory.o memorymanager.o DISK_driver.o DISK_driver_problem1.o  -lm
kernel.o: kernel.c kernel.h interpreter.h DISK_driver.h memorymanager.h 
	gcc -c kernel.c
cpu.o: cpu.c interpreter.h shell.h ram.h cpu.h pcb.h
	gcc -c cpu.c
ram.o: ram.c
	gcc -c ram.c
shell.o: shell.c interpreter.h
	gcc -c shell.c 
DISK_driver.o: DISK_driver.c DISK_driver.h
	gcc -c DISK_driver.c
interpreter.o: interpreter.c interpreter.h DISK_driver.h shellmemory.h shell.h kernel.h ram.h memorymanager.h DISK_driver_problem1.h
	gcc -c interpreter.c 
pcb.o: pcb.c
	gcc -c pcb.c 
shellmemory.o: shellmemory.c shellmemory.h
	gcc -c shellmemory.c 
memorymanager.o: memorymanager.c interpreter.h ram.h pcb.h kernel.h
	gcc -c memorymanager.c 
DISK_driver_problem1.o: DISK_driver_problem1.c DISK_driver.h DISK_driver_problem1.h
	gcc -c DISK_driver_problem1.c

clear:
	rm *.o