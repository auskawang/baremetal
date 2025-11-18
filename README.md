This is a side project where I attempt to blink an LED, and hopefully do other things, on the MCU without using vendored code, meaning that I write the code required to run the MCU (linker, startup file, makefile, etc). This readme will also serve as an update page.

10/29: had issues with code breaking at 0x0. Implemented hardfault function which then it broke there instead. Reviewed SP and saw that it went into reserved space since I set SP wrong in vector table. After setting it right the PC finally accessed the Reset Handler :D

11/10: the map file shows the addresses that the CPU will when accessing globals. therefore in startup code its important to copy from flash to the addresses that the map file has
additionally, these addresses are mapped to sram regions, meaning that before the startup code that copies over data from flash to sram, this sram region has trash data. this means that using a global variable in assisting with copying over data does not work, so you must use linker variables or use const globals since the map file links addresses in flash to these variables rather than in sram

11/18: value of the location counter (.) relies on the location of the memory section, whether it starts at >FLASH or >SRAM. If copying over data from FLASH (>SRAM AT>FLASH), then we can access the location of where to start copying from using LOADADDR(section_name)
