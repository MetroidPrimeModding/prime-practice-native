/******************************************************
 *** ApplyCodePatches_Template.cpp                  ***
 *** This is a template file used by BuildModule.py ***
 *** to generate DOL patching code.                 ***
 ******************************************************/
#include <PrimeAPI.h>

// Generated Forward Decls
class CPauseScreen;
class CPauseScreen;
class CStateManager;
class CFinalInput;
class CMainFlow;
class CArchitectureMessage;
class CArchitectureQueue;
class CStateManager;
void RenderHook();
void PauseScreenDrawReplacement(CPauseScreen*);
void PauseControllerInputHandler(CPauseScreen*,CStateManager&,const CFinalInput&);
void IOWinMessageHook(CMainFlow*,const CArchitectureMessage&,CArchitectureQueue&);
void drawDebugStuff(CStateManager*);

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
{
	Relocate_Rel24((void*) 0x80005734, reinterpret_cast<void*>(&RenderHook));
	Relocate_Rel24((void*) 0x800061F4, reinterpret_cast<void*>(&RenderHook));
	Relocate_Rel24((void*) 0x802BDC5C, reinterpret_cast<void*>(&RenderHook));
	Relocate_Rel24((void*) 0x80108DB4, reinterpret_cast<void*>(&PauseScreenDrawReplacement));
	Relocate_Rel24((void*) 0x80107A28, reinterpret_cast<void*>(&PauseControllerInputHandler));
	Relocate_Addr32((void*) 0x803D9934, reinterpret_cast<void*>(&IOWinMessageHook));
	Relocate_Rel24((void*) 0x80046B88, reinterpret_cast<void*>(&drawDebugStuff));
}