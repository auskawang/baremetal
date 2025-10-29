#include <stdint.h>

extern int _sdata;
extern int _edata;
extern int _sbss;
extern int _ebss;
int* SRAM_START = (int*)0x20000000;
void Reset_Handler();
void HardFault_Handler();
int main();
uint32_t vectors[] __attribute__((section(".vec_table")))= {
	0x20002FFF,
	(uint32_t)Reset_Handler,
	0,
	(uint32_t)HardFault_Handler,
};

void Reset_Handler() {
	//copy data from FLASH to SRAM
	for (int i = _sdata; i < _edata; i++) {
		*(SRAM_START++) = (*&_sdata)++;
	}

	//copy from FLASH TO SRAM for bss
	for (int i = _sbss; i < _ebss; i++) {
		*(SRAM_START)++ = 0;
	}

	//call main
	main();
}

void HardFault_Handler() {
	while(1) {};
}
