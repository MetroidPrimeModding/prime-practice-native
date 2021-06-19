ENTRY(_prolog)
PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
}

SECTIONS
{
  . = _LINK_START;
	.text :
	{
    _INIT_START = .;
    . = ALIGN(4);
    *(.init)
	  *(.text._prolog)
		*(.text*)
		*(.text.*)
		*(.gnu.warning)
		*(.gnu.linkonce.t*)
		*(.glue_7)
		*(.glue_7t)
	} :text

	.data :
	{
    . = ALIGN(4);
    *(.rodata)
    *all.rodata*(*)
    *(.roda)
    *(.rodata.*)
    *(.gnu.linkonce.r*)
		*(.data)
		*(.data.*)
		*(.gnu.linkonce.d*)
		. = ALIGN(4);
	} :data

	.bss :
	{
		*(.dynbss)
		*(.gnu.linkonce.b*)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
	} :data
	_LINK_END = .;
	_LINK_SIZE = _LINK_END - _LINK_START;
}
