mysh: shell.c interpreter.c shellmemory.c pcb.c readyQueue.c
	gcc -c shell.c interpreter.c pcb.c readyQueue.c
	gcc -D FRAME_STORE_SIZE=$(framesize) -D VARIABLE_STORE_SIZE=$(varmemsize) -c shellmemory.c
	gcc -o mysh shell.o interpreter.o shellmemory.o pcb.o readyQueue.o

clean: 
	rm mysh; rm *.o
