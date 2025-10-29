all: main.o startup.o
link: main.o startup.o
	arm-none-eabi-gcc -T linker.ld -nostartfiles -Wl,-Map=output.map -o main.elf main.o startup.o
startup.o: Core/Startup/startup.c
	arm-none-eabi-gcc -g -c -mcpu=cortex-m0plus -mthumb -std=gnu11 -Wall -O0 Core/Startup/startup.c -o startup.o
main.o: Core/Src/main.c
	arm-none-eabi-gcc -g -c -mcpu=cortex-m0plus -mthumb -std=gnu11 -Wall -O0 Core/Src/main.c -o main.o
clean:
	del *.o *.elf