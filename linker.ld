ENTRY(Reset_Handler)
MEMORY
{
	FLASH(rx):ORIGIN =0x08000000,LENGTH =32K
	SRAM(rwx):ORIGIN =0x20000000,LENGTH =12K
	
}
SECTIONS
{
	.vec_table :
	{
		. = ALIGN(4);
		KEEP(*(.vec_table))
		. = ALIGN(4);
	}>FLASH
	.text :
	{
		. = ALIGN(4);
		KEEP(*(.text))
		. = ALIGN(4);
	}>FLASH
	.data :
	{
		_sdata = .;
		*(.data)
		_edata = .;
	} >SRAM AT>FLASH
	.bss :
	{
		_sbss = .;
		*(.bss)
		_ebss = .;
		
	} >SRAM AT>FLASH
}