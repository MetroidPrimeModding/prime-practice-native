ENTRY(_prolog)
ENTRY(_earlyboot_memset)
PHDRS
{
  gap_atlas PT_LOAD;
  maindata PT_LOAD;
  bss PT_DYNAMIC;
  embedded_data PT_NOTE;
}

SECTIONS
{
  .patcher_config : {
    KEEP(*(.patcher_config*))
  } : embedded_data

  .new_bnr : {
      KEEP(*(.new_bnr*))
    } : embedded_data

  . = 0x805A02F8;
  .gap_atlas :
  {
    . = ALIGN(4);
     *(.rodata._ZN9FontAtlasL10ATLAS_DATAE*)
     *(.rodata._ZN9FontAtlasL11FONT_GLYPHSE*)
     *(.rodata._ZN9FontAtlasL12INDEX_LOOKUPE*)
     *(.rodata._ZN9FontAtlasL15INDEX_ADVANCE_XE*)
     *(.rodata._ZN9FontAtlasL10TexUvLinesE*)
    ASSERT(((0x805A02F8 + 0x2868) > .), "gap_atlas section too large");
  } :gap_atlas
  _GAP_ATLAS_END = .;

  . = _LINK_START;
  .text :
  {
    _INIT_START = .;
    . = ALIGN(4);
    *(.boot._prolog*)
    _earlyboot_memset = .;
    *(.boot._earlyboot_memset*)
    *(.boot*)
    *(.init)
    *(.init_array)
    *(.text._prolog)
    *(.text*)
    *(.text.*)
    *(.gnu.warning)
    *(.gnu.linkonce.t*)
    *(.glue_7)
    *(.glue_7t)
  } :maindata

  .data :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.eh_frame)
    *all.rodata*(*)
    *(.roda)
    *(.rodata.*)
    *(.gnu.linkonce.r*)
    *(.data)
    *(.data.*)
    *(.gnu.linkonce.d*)
  } :maindata

  .bss :
  {
    _BSS_START = .;
    . = ALIGN(4);
    *(.dynbss)
    *(.gnu.linkonce.b*)
    *(.bss*)
    *(COMMON)
    _BSS_END = .;
  } :bss

	_LINK_END = .;
	_LINK_SIZE = _LINK_END - _LINK_START;
}
