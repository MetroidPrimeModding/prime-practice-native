#include <TextRenderer.hpp>
#include "WarpMenu.h"
#include "Menus.h"
#include "NewPauseScreen.hpp"

const WarpArea AREAS_FRIGATE[] = {
    {.name= "Air Lock", .area= 0x07640602},
    {.name= "Biohazard Containment", .area= 0xD16B26D0},
    {.name= "Biotech Research Area 1", .area= 0x85578E54},
    {.name= "Biotech Research Area 2", .area= 0xC5DE3C06},
    {.name= "Cargo Freight Lift to Deck Gamma", .area= 0x6FF0FD62},
    {.name= "Connection Elevator to Deck Alpha", .area= 0xAE1EC8BD},
    {.name= "Connection Elevator to Deck Beta", .area= 0x31C44B23},
    {.name= "Connection Elevator to Deck Beta [2]", .area= 0x6ED3231B},
    {.name= "Deck Alpha Access Hall", .area= 0x624F493A},
    {.name= "Deck Alpha Mech Shaft", .area= 0xC8971E99},
    {.name= "Deck Alpha Umbilical Hall", .area= 0xEE21C026},
    {.name= "Deck Beta Conduit Hall", .area= 0xA8813FB6},
    {.name= "Deck Beta Security Hall", .area= 0x49C59925},
    {.name= "Deck Beta Transit Hall", .area= 0x2CA2A263},
    {.name= "Deck Gamma Monitor Hall", .area= 0xE667B605},
    {.name= "Emergency Evacuation Area", .area= 0x20E48216},
    {.name= "Exterior Docking Hangar", .area= 0xD1241219},
    {.name= "Main Ventilation Shaft A", .area= 0x758C4F1C},
    {.name= "Main Ventilation Shaft B", .area= 0x4CF4E25C},
    {.name= "Main Ventilation Shaft C", .area= 0x5BDC869C},
    {.name= "Main Ventilation Shaft D", .area= 0x3E05B8DC},
    {.name= "Main Ventilation Shaft E", .area= 0x292DDC1C},
    {.name= "Main Ventilation Shaft F", .area= 0x1055715C},
    {.name= "Map Facility", .area= 0xCDE604F0},
    {.name= "Reactor Core", .area= 0x87452DC1},
    {.name= "Reactor Core Entrance", .area= 0x3EA190EE},
    {.name= "Subventilation Shaft Section A", .area= 0x093500E4},
    {.name= "Subventilation Shaft Section B", .area= 0xC0BBB28A}
};
const WarpWorld WARP_WORLD_FRIGATE{
    .name="Frigate Orpheon",
    .world=0x158EFE17,
    .areas=AREAS_FRIGATE,
    .areaCount=sizeof(AREAS_FRIGATE)/sizeof(AREAS_FRIGATE[0])
};

const WarpArea AREAS_TALLON[] = {
    {.name="Alcove", .area=0xC44E7A07},
    {.name="Arbor Chamber", .area=0x24F8AFF3},
    {.name="Artifact Temple", .area=0x2398E906},
    {.name="Biohazard Containment", .area=0xAC2C58FE},
    {.name="Biotech Research Area 1", .area=0x5F2EB7B6},
    {.name="Canyon Cavern", .area=0xEE209548},
    {.name="Cargo Freight Lift to Deck Gamma", .area=0x37B3AFE6},
    {.name="Connection Elevator to Deck Beta", .area=0xE47228EF},
    {.name="Deck Beta Conduit Hall", .area=0xC3D44A6E},
    {.name="Deck Beta Security Hall", .area=0x76F6E356},
    {.name="Deck Beta Transit Hall", .area=0x4A96005E},
    {.name="Frigate Access Tunnel", .area=0xBB158C7E},
    {.name="Frigate Crash Site", .area=0xB9ABCD56},
    {.name="Great Tree Chamber", .area=0xC5D6A597},
    {.name="Great Tree Hall", .area=0xF47DBE5B},
    {.name="Gully", .area=0x7B143499},
    {.name="Hydro Access Tunnel", .area=0xFFB4A966},
    {.name="Landing Site", .area=0xB2701146},
    {.name="Life Grove", .area=0x86EB2E02},
    {.name="Life Grove Tunnel", .area=0xB4FBBEF5},
    {.name="Main Ventilation Shaft Section A", .area=0x66CBE887},
    {.name="Main Ventilation Shaft Section B", .area=0xAFD4E038},
    {.name="Main Ventilation Shaft Section C", .area=0x5E0EE592},
    {.name="Overgrown Cavern", .area=0xCEA263E3},
    {.name="Reactor Access", .area=0xFB427580},
    {.name="Reactor Core", .area=0xEE09629A},
    {.name="Root Cave", .area=0xBD8C8625},
    {.name="Root Tunnel", .area=0x404804D9},
    {.name="Savestation", .area=0xF0594C6D},
    {.name="Tallon Canyon", .area=0x2043C96E},
    {.name="Temple Hall", .area=0x5B4E38F5},
    {.name="Temple Lobby", .area=0x234762BE},
    {.name="Temple Security Station", .area=0xBDB1FCAC},
    {.name="Transport Tunnel A", .area=0x13D96D3D},
    {.name="Transport Tunnel B", .area=0xC7E821BA},
    {.name="Transport Tunnel C", .area=0x85CA08AB},
    {.name="Transport Tunnel D", .area=0x1A932F64},
    {.name="Transport Tunnel E", .area=0x9D330A07},
    {.name="Transport to Chozo Ruins East", .area=0x8A31665E},
    {.name="Transport to Chozo Ruins South", .area=0x0CA514F0},
    {.name="Transport to Chozo Ruins West", .area=0x11A02448},
    {.name="Transport to Magmoor Caverns East", .area=0x15D6FF8B},
    {.name="Transport to Phazon Mines East", .area=0x7D106670},
    {.name="Waterfall Cavern", .area=0xE76AD711},
};
const WarpWorld WARP_WORLD_TALLON{
    .name="Tallon Overworld",
    .world=0x39F2DE28,
    .areas=AREAS_TALLON,
    .areaCount=sizeof(AREAS_TALLON)/sizeof(AREAS_TALLON[0])
};

const WarpArea AREAS_CHOZO[] = {
    {.name="Antechamber", .area=0xAFEFE677},
    {.name="Arboretum", .area=0x18AB6106},
    {.name="Arboretum Access", .area=0x870B0525},
    {.name="Burn Dome", .area=0x4148F7B0},
    {.name="Burn Dome Access", .area=0xEF7EB590},
    {.name="Crossway", .area=0x13FFF119},
    {.name="Crossway Access South", .area=0x675A297F},
    {.name="Crossway Access West", .area=0xD9E78EB0},
    {.name="Dynamo", .area=0x04D6C285},
    {.name="Dynamo Access", .area=0x0F403B07},
    {.name="East Atrium", .area=0x713600E3},
    {.name="East Furnace Access", .area=0x44E528F6},
    {.name="Elder Chamber", .area=0xE1981EFC},
    {.name="Elder Hall Access", .area=0xE1CE5BD1},
    {.name="Energy Core", .area=0xC9D52BBC},
    {.name="Energy Core Access", .area=0x463D0D2E},
    {.name="Eyon Tunnel", .area=0xCB1E8A0B},
    {.name="Furnace", .area=0x2E318473},
    {.name="Gathering Hall", .area=0x47E73BC5},
    {.name="Gathering Hall Access", .area=0x95F2019E},
    {.name="Hall of the Elders", .area=0xFB54A0CB},
    {.name="Hive Totem", .area=0xC8309DF6},
    {.name="Magma Pool", .area=0x491BFABA},
    {.name="Main Plaza", .area=0xD5CDB809},
    {.name="Map Station", .area=0x1A5B2E16},
    {.name="Meditation Fountain", .area=0x282B16B4},
    {.name="North Atrium", .area=0x46295CA0},
    {.name="Nursery Access", .area=0x092D89FD},
    {.name="Piston Tunnel", .area=0x2B3F1CEE},
    {.name="Plaza Access", .area=0x53359457},
    {.name="Reflecting Pool", .area=0x361ECAAC},
    {.name="Reflecting Pool Access", .area=0x9D516D9D},
    {.name="Ruined Fountain", .area=0x165A4DE9},
    {.name="Ruined Fountain Access", .area=0x560DBE38},
    {.name="Ruined Gallery", .area=0xE34FD92B},
    {.name="Ruined Nursery", .area=0xC2576E4D},
    {.name="Ruined Shrine", .area=0x3C785450},
    {.name="Ruined Shrine Access", .area=0xDF746AE0},
    {.name="Ruins Entrance", .area=0xB7F1952A},
    {.name="Save Station 1", .area=0x1D5E482C},
    {.name="Save Station 2", .area=0xF7D8954E},
    {.name="Save Station 3", .area=0x188A23AF},
    {.name="Sun Tower", .area=0xDE161372},
    {.name="Sun Tower Access", .area=0x41CC90EC},
    {.name="Sunchamber", .area=0x9A0A03EB},
    {.name="Sunchamber Access", .area=0x54C40995},
    {.name="Sunchamber Lobby", .area=0x3D238FCD},
    {.name="Totem Access", .area=0xEA8A4073},
    {.name="Tower Chamber", .area=0x11BD63B7},
    {.name="Tower of Light", .area=0x0D72F1F7},
    {.name="Tower of Light Access", .area=0x59E0184E},
    {.name="Training Chamber", .area=0x3F04F304},
    {.name="Training Chamber Access", .area=0x18D186BB},
    {.name="Transport Access North", .area=0x3AD2120F},
    {.name="Transport Access South", .area=0xA2F90C53},
    {.name="Transport to Magmoor Caverns North", .area=0x8316EDF5},
    {.name="Transport to Tallon Overworld East", .area=0xA5FA69A1},
    {.name="Transport to Tallon Overworld North", .area=0x3E6B2BB7},
    {.name="Transport to Tallon Overworld South", .area=0x236E1B0F},
    {.name="Vault", .area=0xEF069019},
    {.name="Vault Access", .area=0xA5089191},
    {.name="Watery Hall", .area=0x492CBF4A},
    {.name="Watery Hall Access", .area=0xEEEC837D},
    {.name="West Furnace Access", .area=0xC2715A58}
};
const WarpWorld WARP_WORLD_CHOZO{
    .name="Chozo Ruins",
    .world=0x83F6FF6F,
    .areas=AREAS_CHOZO,
    .areaCount=sizeof(AREAS_CHOZO)/sizeof(AREAS_CHOZO[0])
};

const WarpArea AREAS_MAGMOOR[] = {
    {.name="Burning Trail", .area=0x6D434F4E},
    {.name="Fiery Shores", .area=0xF5EF1862},
    {.name="Geothermal Core", .area=0xC0498676},
    {.name="Lake Tunnel", .area=0x79784D3D},
    {.name="Lava Lake", .area=0xA4719C6A},
    {.name="Magmoor Workstation", .area=0x8ABEB3C3},
    {.name="Monitor Station", .area=0x0C57A641},
    {.name="Monitor Tunnel", .area=0x0DCC4BCC},
    {.name="North Core Tunnel", .area=0xA73BD0E0},
    {.name="Pit Tunnel", .area=0xDA2ECB94},
    {.name="Plasma Processing", .area=0x4CC18E5A},
    {.name="Save Station Magmoor A", .area=0x09B3E01C},
    {.name="Save Station Magmoor B", .area=0x7F56D921},
    {.name="Shore Tunnel", .area=0x901040DF},
    {.name="South Core Tunnel", .area=0x70D950B8},
    {.name="Storage Cavern", .area=0xADEF843E},
    {.name="Transport Tunnel A", .area=0x47F2C087},
    {.name="Transport Tunnel B", .area=0x3346C676},
    {.name="Transport Tunnel C", .area=0xD38FD611},
    {.name="Transport to Chozo Ruins North", .area=0x3BEAADC9},
    {.name="Transport to Phazon Mines West", .area=0xEF2F1440},
    {.name="Transport to Phendrana Drifts North", .area=0xDCA9A28B},
    {.name="Transport to Phendrana Drifts South", .area=0xC1AC9233},
    {.name="Transport to Tallon Overworld West", .area=0x4C3D244C},
    {.name="Triclops Pit", .area=0xBAD9EDBF},
    {.name="Twin Fires", .area=0x4C784BEA},
    {.name="Twin Fires Tunnel", .area=0xE4A4462E},
    {.name="Warrior Shrine", .area=0x89A6CB8D},
    {.name="Workstation Tunnel", .area=0x046D5649}
};
const WarpWorld WARP_WORLD_MAGMOOR{
    .name="Magmoor Caverns",
    .world=0x3EF8237C,
    .areas=AREAS_MAGMOOR,
    .areaCount=sizeof(AREAS_MAGMOOR)/sizeof(AREAS_MAGMOOR[0])
};

const WarpArea AREAS_PHEN[] = {
    {.name="Aether Lab Entryway", .area=0x98DCC321},
    {.name="Canyon Entryway", .area=0x034D8137},
    {.name="Chamber Access", .area=0xCA6CC052},
    {.name="Chapel Tunnel", .area=0xEF674B4C},
    {.name="Chapel of the Elders", .area=0x40C548E9},
    {.name="Chozo Ice Temple", .area=0x6655F51E},
    {.name="Control Tower", .area=0xB3C33249},
    {.name="Courtyard Entryway", .area=0xCFB8ABD1},
    {.name="East Tower", .area=0x51091931},
    {.name="Frost Cave", .area=0x4C6F7773},
    {.name="Frost Cave Access", .area=0x39C70FB9},
    {.name="Frozen Pike", .area=0xD79EE805},
    {.name="Gravity Chamber", .area=0x49175472},
    {.name="Hunter Cave", .area=0x1EC7951A},
    {.name="Hunter Cave Access", .area=0x20EA1D30},
    {.name="Hydra Lab Entryway", .area=0x3947E047},
    {.name="Ice Ruins Access", .area=0x4E85203C},
    {.name="Ice Ruins East", .area=0xDAFCC26F},
    {.name="Ice Ruins West", .area=0xB33A0620},
    {.name="Lake Tunnel", .area=0x89D7A0A6},
    {.name="Lower Edge Tunnel", .area=0x53801084},
    {.name="Map Station", .area=0x83151B33},
    {.name="North Quarantine Tunnel", .area=0x05E1962E},
    {.name="Observatory", .area=0x3FB4A34E},
    {.name="Observatory Access", .area=0x37BBB33C},
    {.name="Phendrana Canyon", .area=0xA20A7455},
    {.name="Phendrana Shorelines", .area=0xF7285979},
    {.name="Phendrana's Edge", .area=0x54DEF128},
    {.name="Pike Access", .area=0x760E731A},
    {.name="Plaza Walkway", .area=0xC8115292},
    {.name="Quarantine Access", .area=0xEAB320CF},
    {.name="Quarantine Cave", .area=0x70181194},
    {.name="Quarantine Monitor", .area=0x2191A05D},
    {.name="Research Core", .area=0xA49B2544},
    {.name="Research Core Access", .area=0xD8E905DD},
    {.name="Research Entrance", .area=0xB51FCE29},
    {.name="Research Lab Aether", .area=0x21B4BFF6},
    {.name="Research Lab Hydra", .area=0x43E4CC25},
    {.name="Ruined Courtyard", .area=0x1921876D},
    {.name="Ruins Entryway", .area=0x3C13643A},
    {.name="Save Station A", .area=0x5694A06B},
    {.name="Save Station B", .area=0x0581699D},
    {.name="Save Station C", .area=0xCEDDBA38},
    {.name="Save Station D", .area=0x715C31EE},
    {.name="Security Cave", .area=0x3C9490E5},
    {.name="Shoreline Entrance", .area=0xC4107CD7},
    {.name="South Quarantine Tunnel", .area=0x0035FDAD},
    {.name="Specimen Storage", .area=0xD341D2DB},
    {.name="Storage Cave", .area=0xF7C84340},
    {.name="Temple Entryway", .area=0x85D9F399},
    {.name="Transport Access", .area=0xD695B958},
    {.name="Transport to Magmoor Caverns South", .area=0xDD0B0739},
    {.name="Transport to Magmoor Caverns West", .area=0xC00E3781},
    {.name="Upper Edge Tunnel", .area=0x253E76B3},
    {.name="West Tower ", .area=0xD79D6B9F},
    {.name="West Tower Entrance", .area=0x1E48B18F}
};
const WarpWorld WARP_WORLD_PHEN{
    .name="Phendrana Drifts",
    .world=0xA8BE6291,
    .areas=AREAS_PHEN,
    .areaCount=sizeof(AREAS_PHEN)/sizeof(AREAS_PHEN[0])
};

const WarpArea AREAS_MINES[] = {
    {.name="Central Dynamo", .area=0xFEA372E2},
    {.name="Dynamo Access", .area=0xF517A1EA},
    {.name="Elevator A", .area=0x0146ED43},
    {.name="Elevator Access A", .area=0x26219C01},
    {.name="Elevator Access B", .area=0x3FD9D766},
    {.name="Elevator B", .area=0xE87957E0},
    {.name="Elite Control", .area=0xC50AF17A},
    {.name="Elite Control Access", .area=0x8988D1CB},
    {.name="Elite Quarters", .area=0x3953C353},
    {.name="Elite Quarters Access", .area=0x71343C3F},
    {.name="Elite Research", .area=0x8A97BB54},
    {.name="Fungal Hall A", .area=0x0F5277D1},
    {.name="Fungal Hall Access", .area=0xDE9D71F5},
    {.name="Fungal Hall B", .area=0xEC47C242},
    {.name="Main Quarry", .area=0x643D038F},
    {.name="Maintenance Tunnel", .area=0xECEFEA8D},
    {.name="Map Station Mines", .area=0x198FF5DC},
    {.name="Metroid Quarantine A", .area=0xFB051F5A},
    {.name="Metroid Quarantine B", .area=0xBB3AFC4E},
    {.name="Mine Security Station", .area=0x956F1552},
    {.name="Missile Station Mines", .area=0xB089331E},
    {.name="Omega Research", .area=0x3F375ECC},
    {.name="Ore Processing", .area=0x97D2B2F6},
    {.name="Phazon Mining Tunnel", .area=0xBBFA4AB3},
    {.name="Phazon Processing Center", .area=0xAD2E7EB9},
    {.name="Processing Center Access", .area=0xED6DE73B},
    {.name="Quarantine Access A", .area=0x5ABEEC20},
    {.name="Quarantine Access B", .area=0x14530779},
    {.name="Quarry Access", .area=0x68CC7758},
    {.name="Research Access", .area=0x4346A747},
    {.name="Save Station Mines A", .area=0x361D41B0},
    {.name="Save Station Mines B", .area=0x7BD5E0BB},
    {.name="Save Station Mines C", .area=0x66D0D003},
    {.name="Security Access A", .area=0xC7653A92},
    {.name="Security Access B", .area=0xA20201D4},
    {.name="Storage Depot A", .area=0x35C5D736},
    {.name="Storage Depot B", .area=0xE39C342B},
    {.name="Transport Access", .area=0x42C4AAF1},
    {.name="Transport to Magmoor Caverns South", .area=0xE2C2CF38},
    {.name="Transport to Tallon Overworld South", .area=0x430E999C},
    {.name="Ventilation Shaft", .area=0x90709AAC},
    {.name="Waste Disposal", .area=0x27A391B7}
};
const WarpWorld WARP_WORLD_MINES{
    .name="Phazon Mines",
    .world=0xB1AC4D65,
    .areas=AREAS_MINES,
    .areaCount=sizeof(AREAS_MINES)/sizeof(AREAS_MINES[0])
};

const WarpArea AREAS_CRATER[] = {
    {.name="Crater Entry Point", .area=0x93668996},
    {.name="Crater Missile Station", .area=0x4D446C3F},
    {.name="Crater Tunnel A", .area=0x49CB2363},
    {.name="Crater Tunnel B", .area=0x32D5A180},
    {.name="Metroid Prime Lair", .area=0x1A666C55},
    {.name="Phazon Core", .area=0xBD946AC3},
    {.name="Phazon Infusion Chamber", .area=0x67156A0D},
    {.name="Subchamber Five", .area=0x77714498},
    {.name="Subchamber Four", .area=0xA7AC009B},
    {.name="Subchamber One", .area=0xDADF06C3},
    {.name="Subchamber Three", .area=0x7A3AD91E},
    {.name="Subchamber Two", .area=0x0749DF46}
};
const WarpWorld WARP_WORLD_CRATER{
    .name="Impact Crater",
    .world=0xC13B09D1,
    .areas=AREAS_CRATER,
    .areaCount=sizeof(AREAS_CRATER)/sizeof(AREAS_CRATER[0])
};

void WarpMainMenu::render(int x, int y) const {
  TextRenderer::SetColor(1, 1, 1, 1);
  TextRenderer::RenderText("Warps Menu", (float) x, (float) y);

  Menu::render(x, y);
}

int WarpMainMenu::itemCount() const {
  return static_cast<int>(WarpMainMenuItem::END);
}

void WarpMainMenu::renderItem(int index, int x, int y) const {
  auto item = WarpMainMenuItem{index};
  switch (item) {
    case WarpMainMenuItem::FRIGATE:
      TextRenderer::RenderText(WARP_WORLD_FRIGATE.name, x, y);
      break;
    case WarpMainMenuItem::TALLON:
      TextRenderer::RenderText(WARP_WORLD_TALLON.name, x, y);
      break;
    case WarpMainMenuItem::CHOZO:
      TextRenderer::RenderText(WARP_WORLD_CHOZO.name, x, y);
      break;
    case WarpMainMenuItem::MAGMOOR:
      TextRenderer::RenderText(WARP_WORLD_MAGMOOR.name, x, y);
      break;
    case WarpMainMenuItem::PHEN:
      TextRenderer::RenderText(WARP_WORLD_PHEN.name, x, y);
      break;
    case WarpMainMenuItem::MINES:
      TextRenderer::RenderText(WARP_WORLD_MINES.name, x, y);
      break;
    case WarpMainMenuItem::CRATER:
      TextRenderer::RenderText(WARP_WORLD_CRATER.name, x, y);
      break;
    case WarpMainMenuItem::END:
      break;
  }
}

void WarpMainMenu::clickItem(int index) {
  auto item = WarpMainMenuItem{index};
  switch (item) {
    case WarpMainMenuItem::FRIGATE:
      break;
    case WarpMainMenuItem::TALLON:
      break;
    case WarpMainMenuItem::CHOZO:
      break;
    case WarpMainMenuItem::MAGMOOR:
      break;
    case WarpMainMenuItem::PHEN:
      break;
    case WarpMainMenuItem::MINES:
      break;
    case WarpMainMenuItem::CRATER:
      break;
    case WarpMainMenuItem::END:
      break;
  }
}

constexpr WarpMenu::WarpMenu(const WarpWorld *world) : world(world) {
}

void WarpMenu::render(int x, int y) const {
  TextRenderer::SetColor(1, 1, 1, 1);
  TextRenderer::RenderText(this->world->name, (float) x, (float) y);

  Menu::render(x, y);
}

int WarpMenu::itemCount() const {
  return this->world->areaCount;
}

void WarpMenu::renderItem(int index, int x, int y) const {
  TextRenderer::RenderText(this->world->areas[index].name, x, y);
}

void WarpMenu::clickItem(int index) {
  warp(this->world->world, this->world->areas[this->currentCursor()].area);
}

int WarpMenu::getWidthInCharacters() {
  return 20;
}

WarpMainMenu MENU_WARP_MAIN;
WarpMenu MENU_WARP_FRIGATE{&WARP_WORLD_FRIGATE};
WarpMenu MENU_WARP_TALLON{&WARP_WORLD_TALLON};
WarpMenu MENU_WARP_CHOZO{&WARP_WORLD_CHOZO};
WarpMenu MENU_WARP_MAGMOOR{&WARP_WORLD_MAGMOOR};
WarpMenu MENU_WARP_PHEN{&WARP_WORLD_PHEN};
WarpMenu MENU_WARP_MINES{&WARP_WORLD_MINES};
WarpMenu MENU_WARP_CRATER{&WARP_WORLD_CRATER};
