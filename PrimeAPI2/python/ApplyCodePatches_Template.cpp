/******************************************************
 *** ApplyCodePatches_Template.cpp                  ***
 *** This is a template file used by BuildModule.py ***
 *** to generate DOL patching code.                 ***
 ******************************************************/
#include <PrimeAPI.h>

// Generated Forward Decls
%s
// Function Prototypes
void Relocate_Addr32(void *pRelocAddress, void *pSymbolAddress);
void Relocate_Rel24(void *pRelocAddress, void *pSymbolAddress);
void ApplyCodePatches();

// Function Implementations
void Relocate_Addr32(void *pRelocAddress, void *pSymbolAddress)
{
	uint32 *pReloc = (uint32*) pRelocAddress;
	*pReloc = (uint32) pSymbolAddress;
}

void Relocate_Rel24(void *pRelocAddress, void *pSymbolAddress)
{
	uint32 *pReloc = (uint32*) pRelocAddress;
	uint32 instruction = *pReloc;
	uint32 AA = (instruction >> 1) & 0x1;
	*pReloc = (instruction & ~0x3FFFFFC) | (AA == 0 ? ((uint32) pSymbolAddress - (uint32) pRelocAddress) : (uint32) pSymbolAddress);
}

void ApplyCodePatches()
{%s
}
