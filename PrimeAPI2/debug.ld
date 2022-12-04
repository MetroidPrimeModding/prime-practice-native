ENTRY(_prolog)
ENTRY(_earlyboot_memset)
PHDRS
{
  gap1 PT_LOAD;
  gap_vsnprintf PT_LOAD;
  gap_atlas PT_LOAD;
  end PT_LOAD;
}

/*
0x8056A7E4, 0x1610, 0x8056BDF4
0x8056D5E0, 0x138,
0x8056F654, 0x104,
0x8056F874, 0x20,
0x8056F8CC, 0x1080, 0x8057094C
0x80571ABC, 0xD4,
0x80571CCC, 0x1A0,
0x80572030, 0xC,
0x80572054, 0xC,
0x80572088, 0x50,
0x805723EC, 0xC,
0x80572414, 0x100,
0x8057267C, 0xC,
0x80577BAC, 0x14000, 0x8058BBAC
0x8059FBB8, 0xC,
0x8059FBE8, 0x90,
0x805A02F8, 0x2868, 0x805A2B60
0x805A53D4, 0xC,
0x805A56E4, 0x78C, 0x805A5E70
0x805A66AC, 0x48,
0x805A676C, 0x18,
0x805A67EC, 0x10,
0x805A6B90, 0x10,
*/
SECTIONS
{
  . = 0x8056A7E4;
  .gap_vsnprintf :
  {
    . = ALIGN(4);
     *(.text._vsnprintf*)
    ASSERT(((0x8056A7E4 + 0x1610) > .), "gap_vsnprintf section too large");
  } :gap_vsnprintf
  _GAP_VSNPRINGF_END = .;

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
  .imgui :
  {
    . = ALIGN(4);
    *(.imgui*)
    *(.imgui.*)
  }: end

	.bss :
	{
		*(.dynbss)
		*(.gnu.linkonce.b*)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
	} :end

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
      . = ALIGN(4);
    } :gap1

    .text :
    {
      _INIT_START = .;
      . = ALIGN(4);
      *(.boot._prolog*)
      _earlyboot_memset = .;
      *(.boot._earlyboot_memset*)
      *(.boot*)
      *(.init)
      *(.text._prolog)
      *(.text*)
      *(.text.*)
      *(.gnu.warning)
      *(.gnu.linkonce.t*)
      *(.glue_7)
      *(.glue_7t)
    } :gap1

    .gap1 :
    {
      . = ALIGN(4);
      *(.gap1*)
    } :gap1

	_LINK_END = .;
	_LINK_SIZE = _LINK_END - _LINK_START;
}
