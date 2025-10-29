This is a side project where I attempt to blink an LED, and hopefully do other things, on the MCU without using vendored code, meaning that I write the code required to run the MCU (linker, startup file, makefile, etc). This readme will also serve as an update page.

10/29: had issues with code breaking at 0x0. Implemented hardfault function which then it broke there instead. Reviewed SP and saw that it went into reserved space since I set SP wrong in vector table. After setting it right the PC finally accessed the Reset Handler :D
