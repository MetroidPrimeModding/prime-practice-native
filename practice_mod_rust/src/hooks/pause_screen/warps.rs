use crate::cpp_interface::text_renderer::{
    draw_text, input_down, input_down_fast, input_pressed_back, input_pressed_ok, input_up,
    input_up_fast, set_text_color, warp,
};
use crate::hooks::pause_screen::HandleInputResult::{Deselect, StopPropagation};
use crate::hooks::pause_screen::OnSelectResult::DoNothing;
use crate::hooks::pause_screen::{
    HandleInputResult, MenuItem, OnSelectResult, MENU_3_OFFSET_X, PAUSE_MENU_OFFSET,
};
use crate::hooks::LINE_HEIGHT;
use alloc::vec::Vec;

pub struct WarpWorldItem<'a> {
    pub name: &'a str,
    pub world_id: u32,
    pub warps: &'static [Warp],
    pub active: bool,
    pub cursor: i32,
    pub scroll_timer: i32,
}

impl<'a> WarpWorldItem<'a> {
    pub fn new(name: &'a str, world_id: u32, warps: &'static [Warp]) -> Self {
        WarpWorldItem {
            name,
            world_id,
            warps,
            active: false,
            cursor: 0,
            scroll_timer: 0,
        }
    }
}

impl<'a> MenuItem for WarpWorldItem<'a> {
    fn on_select(&mut self) -> OnSelectResult {
        self.active = true;
        DoNothing
    }

    fn handle_input(&mut self) -> HandleInputResult {
        if self.scroll_timer > 0 {
            self.scroll_timer -= 1;
        }
        if input_down() {
            if self.scroll_timer <= 0 {
                self.cursor += 1;
                self.scroll_timer = 8;
            }
        }
        if input_up() {
            if self.scroll_timer <= 0 {
                self.cursor -= 1;
                self.scroll_timer = 8;
            }
        }
        if input_down_fast() {
            if self.scroll_timer <= 0 {
                self.cursor += 10;
                self.scroll_timer = 15;
            }
        }
        if input_up_fast() {
            if self.scroll_timer <= 0 {
                self.cursor -= 10;
                self.scroll_timer = 15;
            }
        }

        self.cursor = self.cursor % self.warps.len() as i32;
        if self.cursor < 0 {
            self.cursor += self.warps.len() as i32;
        }

        if input_pressed_ok() {
            warp(self.world_id, self.warps[self.cursor as usize].area)
        }

        if input_pressed_back() {
            self.active = false;
            return Deselect;
        }

        StopPropagation
    }

    fn draw(&self, x: f32, y: f32) {
        draw_text(&self.name, x, y);
        if self.active {
            let mut y_off = 0f32;
            let scroll_off = 30i32;

            if self.cursor > scroll_off {
                y_off = (self.cursor - scroll_off) as f32 * -LINE_HEIGHT
            }

            for (i, warp) in self.warps.iter().enumerate() {
                let mut y = LINE_HEIGHT * i as f32 + y_off;
                if y < 0.0 || y > 400.0 {
                    continue;
                }
                if self.cursor == i as i32 {
                    set_text_color(1.0, 1.0, 1.0, 1.0);
                } else {
                    set_text_color(0.4, 0.4, 0.4, 1.0);
                }
                draw_text(warp.name, MENU_3_OFFSET_X, PAUSE_MENU_OFFSET + y);
            }
        }
    }
}

pub struct Warp {
    name: &'static str,
    area: u32,
}

#[rustfmt::skip]
pub const FRIGATE: [Warp; 28] = [
    Warp { name: "Air Lock", area: 0x07640602 },
    Warp { name: "Biohazard Containment", area: 0xD16B26D0 },
    Warp { name: "Biotech Research Area 1", area: 0x85578E54 },
    Warp { name: "Biotech Research Area 2", area: 0xC5DE3C06 },
    Warp { name: "Cargo Freight Lift to Deck Gamma", area: 0x6FF0FD62 },
    Warp { name: "Connection Elevator to Deck Alpha", area: 0xAE1EC8BD },
    Warp { name: "Connection Elevator to Deck Beta", area: 0x31C44B23 },
    Warp { name: "Connection Elevator to Deck Beta [2]", area: 0x6ED3231B },
    Warp { name: "Deck Alpha Access Hall", area: 0x624F493A },
    Warp { name: "Deck Alpha Mech Shaft", area: 0xC8971E99 },
    Warp { name: "Deck Alpha Umbilical Hall", area: 0xEE21C026 },
    Warp { name: "Deck Beta Conduit Hall", area: 0xA8813FB6 },
    Warp { name: "Deck Beta Security Hall", area: 0x49C59925 },
    Warp { name: "Deck Beta Transit Hall", area: 0x2CA2A263 },
    Warp { name: "Deck Gamma Monitor Hall", area: 0xE667B605 },
    Warp { name: "Emergency Evacuation Area", area: 0x20E48216 },
    Warp { name: "Exterior Docking Hangar", area: 0xD1241219 },
    Warp { name: "Main Ventilation Shaft A", area: 0x758C4F1C },
    Warp { name: "Main Ventilation Shaft B", area: 0x4CF4E25C },
    Warp { name: "Main Ventilation Shaft C", area: 0x5BDC869C },
    Warp { name: "Main Ventilation Shaft D", area: 0x3E05B8DC },
    Warp { name: "Main Ventilation Shaft E", area: 0x292DDC1C },
    Warp { name: "Main Ventilation Shaft F", area: 0x1055715C },
    Warp { name: "Map Facility", area: 0xCDE604F0 },
    Warp { name: "Reactor Core", area: 0x87452DC1 },
    Warp { name: "Reactor Core Entrance", area: 0x3EA190EE },
    Warp { name: "Subventilation Shaft Section A", area: 0x093500E4 },
    Warp { name: "Subventilation Shaft Section B", area: 0xC0BBB28A },
];

#[rustfmt::skip]
pub const TALLON: [Warp; 44] = [
    Warp { name: "Alcove", area: 0xC44E7A07 },
    Warp { name: "Arbor Chamber", area: 0x24F8AFF3 },
    Warp { name: "Artifact Temple", area: 0x2398E906 },
    Warp { name: "Biohazard Containment", area: 0xAC2C58FE },
    Warp { name: "Biotech Research Area 1", area: 0x5F2EB7B6 },
    Warp { name: "Canyon Cavern", area: 0xEE209548 },
    Warp { name: "Cargo Freight Lift to Deck Gamma", area: 0x37B3AFE6 },
    Warp { name: "Connection Elevator to Deck Beta", area: 0xE47228EF },
    Warp { name: "Deck Beta Conduit Hall", area: 0xC3D44A6E },
    Warp { name: "Deck Beta Security Hall", area: 0x76F6E356 },
    Warp { name: "Deck Beta Transit Hall", area: 0x4A96005E },
    Warp { name: "Frigate Access Tunnel", area: 0xBB158C7E },
    Warp { name: "Frigate Crash Site", area: 0xB9ABCD56 },
    Warp { name: "Great Tree Chamber", area: 0xC5D6A597 },
    Warp { name: "Great Tree Hall", area: 0xF47DBE5B },
    Warp { name: "Gully", area: 0x7B143499 },
    Warp { name: "Hydro Access Tunnel", area: 0xFFB4A966 },
    Warp { name: "Landing Site", area: 0xB2701146 },
    Warp { name: "Life Grove", area: 0x86EB2E02 },
    Warp { name: "Life Grove Tunnel", area: 0xB4FBBEF5 },
    Warp { name: "Main Ventilation Shaft Section A", area: 0x66CBE887 },
    Warp { name: "Main Ventilation Shaft Section B", area: 0xAFD4E038 },
    Warp { name: "Main Ventilation Shaft Section C", area: 0x5E0EE592 },
    Warp { name: "Overgrown Cavern", area: 0xCEA263E3 },
    Warp { name: "Reactor Access", area: 0xFB427580 },
    Warp { name: "Reactor Core", area: 0xEE09629A },
    Warp { name: "Root Cave", area: 0xBD8C8625 },
    Warp { name: "Root Tunnel", area: 0x404804D9 },
    Warp { name: "Savestation", area: 0xF0594C6D },
    Warp { name: "Tallon Canyon", area: 0x2043C96E },
    Warp { name: "Temple Hall", area: 0x5B4E38F5 },
    Warp { name: "Temple Lobby", area: 0x234762BE },
    Warp { name: "Temple Security Station", area: 0xBDB1FCAC },
    Warp { name: "Transport Tunnel A", area: 0x13D96D3D },
    Warp { name: "Transport Tunnel B", area: 0xC7E821BA },
    Warp { name: "Transport Tunnel C", area: 0x85CA08AB },
    Warp { name: "Transport Tunnel D", area: 0x1A932F64 },
    Warp { name: "Transport Tunnel E", area: 0x9D330A07 },
    Warp { name: "Transport to Chozo Ruins East", area: 0x8A31665E },
    Warp { name: "Transport to Chozo Ruins South", area: 0x0CA514F0 },
    Warp { name: "Transport to Chozo Ruins West", area: 0x11A02448 },
    Warp { name: "Transport to Magmoor Caverns East", area: 0x15D6FF8B },
    Warp { name: "Transport to Phazon Mines East", area: 0x7D106670 },
    Warp { name: "Waterfall Cavern", area: 0xE76AD711 },
];

#[rustfmt::skip]
pub const CHOZO: [Warp; 64] = [
    Warp { name: "Antechamber", area: 0xAFEFE677 },
    Warp { name: "Arboretum", area: 0x18AB6106 },
    Warp { name: "Arboretum Access", area: 0x870B0525 },
    Warp { name: "Burn Dome", area: 0x4148F7B0 },
    Warp { name: "Burn Dome Access", area: 0xEF7EB590 },
    Warp { name: "Crossway", area: 0x13FFF119 },
    Warp { name: "Crossway Access South", area: 0x675A297F },
    Warp { name: "Crossway Access West", area: 0xD9E78EB0 },
    Warp { name: "Dynamo", area: 0x04D6C285 },
    Warp { name: "Dynamo Access", area: 0x0F403B07 },
    Warp { name: "East Atrium", area: 0x713600E3 },
    Warp { name: "East Furnace Access", area: 0x44E528F6 },
    Warp { name: "Elder Chamber", area: 0xE1981EFC },
    Warp { name: "Elder Hall Access", area: 0xE1CE5BD1 },
    Warp { name: "Energy Core", area: 0xC9D52BBC },
    Warp { name: "Energy Core Access", area: 0x463D0D2E },
    Warp { name: "Eyon Tunnel", area: 0xCB1E8A0B },
    Warp { name: "Furnace", area: 0x2E318473 },
    Warp { name: "Gathering Hall", area: 0x47E73BC5 },
    Warp { name: "Gathering Hall Access", area: 0x95F2019E },
    Warp { name: "Hall of the Elders", area: 0xFB54A0CB },
    Warp { name: "Hive Totem", area: 0xC8309DF6 },
    Warp { name: "Magma Pool", area: 0x491BFABA },
    Warp { name: "Main Plaza", area: 0xD5CDB809 },
    Warp { name: "Map Station", area: 0x1A5B2E16 },
    Warp { name: "Meditation Fountain", area: 0x282B16B4 },
    Warp { name: "North Atrium", area: 0x46295CA0 },
    Warp { name: "Nursery Access", area: 0x092D89FD },
    Warp { name: "Piston Tunnel", area: 0x2B3F1CEE },
    Warp { name: "Plaza Access", area: 0x53359457 },
    Warp { name: "Reflecting Pool", area: 0x361ECAAC },
    Warp { name: "Reflecting Pool Access", area: 0x9D516D9D },
    Warp { name: "Ruined Fountain", area: 0x165A4DE9 },
    Warp { name: "Ruined Fountain Access", area: 0x560DBE38 },
    Warp { name: "Ruined Gallery", area: 0xE34FD92B },
    Warp { name: "Ruined Nursery", area: 0xC2576E4D },
    Warp { name: "Ruined Shrine", area: 0x3C785450 },
    Warp { name: "Ruined Shrine Access", area: 0xDF746AE0 },
    Warp { name: "Ruins Entrance", area: 0xB7F1952A },
    Warp { name: "Save Station 1", area: 0x1D5E482C },
    Warp { name: "Save Station 2", area: 0xF7D8954E },
    Warp { name: "Save Station 3", area: 0x188A23AF },
    Warp { name: "Sun Tower", area: 0xDE161372 },
    Warp { name: "Sun Tower Access", area: 0x41CC90EC },
    Warp { name: "Sunchamber", area: 0x9A0A03EB },
    Warp { name: "Sunchamber Access", area: 0x54C40995 },
    Warp { name: "Sunchamber Lobby", area: 0x3D238FCD },
    Warp { name: "Totem Access", area: 0xEA8A4073 },
    Warp { name: "Tower Chamber", area: 0x11BD63B7 },
    Warp { name: "Tower of Light", area: 0x0D72F1F7 },
    Warp { name: "Tower of Light Access", area: 0x59E0184E },
    Warp { name: "Training Chamber", area: 0x3F04F304 },
    Warp { name: "Training Chamber Access", area: 0x18D186BB },
    Warp { name: "Transport Access North", area: 0x3AD2120F },
    Warp { name: "Transport Access South", area: 0xA2F90C53 },
    Warp { name: "Transport to Magmoor Caverns North", area: 0x8316EDF5 },
    Warp { name: "Transport to Tallon Overworld East", area: 0xA5FA69A1 },
    Warp { name: "Transport to Tallon Overworld North", area: 0x3E6B2BB7 },
    Warp { name: "Transport to Tallon Overworld South", area: 0x236E1B0F },
    Warp { name: "Vault", area: 0xEF069019 },
    Warp { name: "Vault Access", area: 0xA5089191 },
    Warp { name: "Watery Hall", area: 0x492CBF4A },
    Warp { name: "Watery Hall Access", area: 0xEEEC837D },
    Warp { name: "West Furnace Access", area: 0xC2715A58 }
];

#[rustfmt::skip]
pub const MAGMOOR: [Warp; 29] = [
    Warp { name: "Burning Trail", area: 0x6D434F4E },
    Warp { name: "Fiery Shores", area: 0xF5EF1862 },
    Warp { name: "Geothermal Core", area: 0xC0498676 },
    Warp { name: "Lake Tunnel", area: 0x79784D3D },
    Warp { name: "Lava Lake", area: 0xA4719C6A },
    Warp { name: "Magmoor Workstation", area: 0x8ABEB3C3 },
    Warp { name: "Monitor Station", area: 0x0C57A641 },
    Warp { name: "Monitor Tunnel", area: 0x0DCC4BCC },
    Warp { name: "North Core Tunnel", area: 0xA73BD0E0 },
    Warp { name: "Pit Tunnel", area: 0xDA2ECB94 },
    Warp { name: "Plasma Processing", area: 0x4CC18E5A },
    Warp { name: "Save Station Magmoor A", area: 0x09B3E01C },
    Warp { name: "Save Station Magmoor B", area: 0x7F56D921 },
    Warp { name: "Shore Tunnel", area: 0x901040DF },
    Warp { name: "South Core Tunnel", area: 0x70D950B8 },
    Warp { name: "Storage Cavern", area: 0xADEF843E },
    Warp { name: "Transport Tunnel A", area: 0x47F2C087 },
    Warp { name: "Transport Tunnel B", area: 0x3346C676 },
    Warp { name: "Transport Tunnel C", area: 0xD38FD611 },
    Warp { name: "Transport to Chozo Ruins North", area: 0x3BEAADC9 },
    Warp { name: "Transport to Phazon Mines West", area: 0xEF2F1440 },
    Warp { name: "Transport to Phendrana Drifts North", area: 0xDCA9A28B },
    Warp { name: "Transport to Phendrana Drifts South", area: 0xC1AC9233 },
    Warp { name: "Transport to Tallon Overworld West", area: 0x4C3D244C },
    Warp { name: "Triclops Pit", area: 0xBAD9EDBF },
    Warp { name: "Twin Fires", area: 0x4C784BEA },
    Warp { name: "Twin Fires Tunnel", area: 0xE4A4462E },
    Warp { name: "Warrior Shrine", area: 0x89A6CB8D },
    Warp { name: "Workstation Tunnel", area: 0x046D5649 }
];

#[rustfmt::skip]
pub const PHENDRANA: [Warp; 56] = [
    Warp { name: "Aether Lab Entryway", area: 0x98DCC321 },
    Warp { name: "Canyon Entryway", area: 0x034D8137 },
    Warp { name: "Chamber Access", area: 0xCA6CC052 },
    Warp { name: "Chapel Tunnel", area: 0xEF674B4C },
    Warp { name: "Chapel of the Elders", area: 0x40C548E9 },
    Warp { name: "Chozo Ice Temple", area: 0x6655F51E },
    Warp { name: "Control Tower", area: 0xB3C33249 },
    Warp { name: "Courtyard Entryway", area: 0xCFB8ABD1 },
    Warp { name: "East Tower", area: 0x51091931 },
    Warp { name: "Frost Cave", area: 0x4C6F7773 },
    Warp { name: "Frost Cave Access", area: 0x39C70FB9 },
    Warp { name: "Frozen Pike", area: 0xD79EE805 },
    Warp { name: "Gravity Chamber", area: 0x49175472 },
    Warp { name: "Hunter Cave", area: 0x1EC7951A },
    Warp { name: "Hunter Cave Access", area: 0x20EA1D30 },
    Warp { name: "Hydra Lab Entryway", area: 0x3947E047 },
    Warp { name: "Ice Ruins Access", area: 0x4E85203C },
    Warp { name: "Ice Ruins East", area: 0xDAFCC26F },
    Warp { name: "Ice Ruins West", area: 0xB33A0620 },
    Warp { name: "Lake Tunnel", area: 0x89D7A0A6 },
    Warp { name: "Lower Edge Tunnel", area: 0x53801084 },
    Warp { name: "Map Station", area: 0x83151B33 },
    Warp { name: "North Quarantine Tunnel", area: 0x05E1962E },
    Warp { name: "Observatory", area: 0x3FB4A34E },
    Warp { name: "Observatory Access", area: 0x37BBB33C },
    Warp { name: "Phendrana Canyon", area: 0xA20A7455 },
    Warp { name: "Phendrana Shorelines", area: 0xF7285979 },
    Warp { name: "Phendrana's Edge", area: 0x54DEF128 },
    Warp { name: "Pike Access", area: 0x760E731A },
    Warp { name: "Plaza Walkway", area: 0xC8115292 },
    Warp { name: "Quarantine Access", area: 0xEAB320CF },
    Warp { name: "Quarantine Cave", area: 0x70181194 },
    Warp { name: "Quarantine Monitor", area: 0x2191A05D },
    Warp { name: "Research Core", area: 0xA49B2544 },
    Warp { name: "Research Core Access", area: 0xD8E905DD },
    Warp { name: "Research Entrance", area: 0xB51FCE29 },
    Warp { name: "Research Lab Aether", area: 0x21B4BFF6 },
    Warp { name: "Research Lab Hydra", area: 0x43E4CC25 },
    Warp { name: "Ruined Courtyard", area: 0x1921876D },
    Warp { name: "Ruins Entryway", area: 0x3C13643A },
    Warp { name: "Save Station A", area: 0x5694A06B },
    Warp { name: "Save Station B", area: 0x0581699D },
    Warp { name: "Save Station C", area: 0xCEDDBA38 },
    Warp { name: "Save Station D", area: 0x715C31EE },
    Warp { name: "Security Cave", area: 0x3C9490E5 },
    Warp { name: "Shoreline Entrance", area: 0xC4107CD7 },
    Warp { name: "South Quarantine Tunnel", area: 0x0035FDAD },
    Warp { name: "Specimen Storage", area: 0xD341D2DB },
    Warp { name: "Storage Cave", area: 0xF7C84340 },
    Warp { name: "Temple Entryway", area: 0x85D9F399 },
    Warp { name: "Transport Access", area: 0xD695B958 },
    Warp { name: "Transport to Magmoor Caverns South", area: 0xDD0B0739 },
    Warp { name: "Transport to Magmoor Caverns West", area: 0xC00E3781 },
    Warp { name: "Upper Edge Tunnel", area: 0x253E76B3 },
    Warp { name: "West Tower ", area: 0xD79D6B9F },
    Warp { name: "West Tower Entrance", area: 0x1E48B18F }
];

#[rustfmt::skip]
pub const MINES: [Warp; 42] = [
    Warp { name: "Central Dynamo", area: 0xFEA372E2 },
    Warp { name: "Dynamo Access", area: 0xF517A1EA },
    Warp { name: "Elevator A", area: 0x0146ED43 },
    Warp { name: "Elevator Access A", area: 0x26219C01 },
    Warp { name: "Elevator Access B", area: 0x3FD9D766 },
    Warp { name: "Elevator B", area: 0xE87957E0 },
    Warp { name: "Elite Control", area: 0xC50AF17A },
    Warp { name: "Elite Control Access", area: 0x8988D1CB },
    Warp { name: "Elite Quarters", area: 0x3953C353 },
    Warp { name: "Elite Quarters Access", area: 0x71343C3F },
    Warp { name: "Elite Research", area: 0x8A97BB54 },
    Warp { name: "Fungal Hall A", area: 0x0F5277D1 },
    Warp { name: "Fungal Hall Access", area: 0xDE9D71F5 },
    Warp { name: "Fungal Hall B", area: 0xEC47C242 },
    Warp { name: "Main Quarry", area: 0x643D038F },
    Warp { name: "Maintenance Tunnel", area: 0xECEFEA8D },
    Warp { name: "Map Station Mines", area: 0x198FF5DC },
    Warp { name: "Metroid Quarantine A", area: 0xFB051F5A },
    Warp { name: "Metroid Quarantine B", area: 0xBB3AFC4E },
    Warp { name: "Mine Security Station", area: 0x956F1552 },
    Warp { name: "Missile Station Mines", area: 0xB089331E },
    Warp { name: "Omega Research", area: 0x3F375ECC },
    Warp { name: "Ore Processing", area: 0x97D2B2F6 },
    Warp { name: "Phazon Mining Tunnel", area: 0xBBFA4AB3 },
    Warp { name: "Phazon Processing Center", area: 0xAD2E7EB9 },
    Warp { name: "Processing Center Access", area: 0xED6DE73B },
    Warp { name: "Quarantine Access A", area: 0x5ABEEC20 },
    Warp { name: "Quarantine Access B", area: 0x14530779 },
    Warp { name: "Quarry Access", area: 0x68CC7758 },
    Warp { name: "Research Access", area: 0x4346A747 },
    Warp { name: "Save Station Mines A", area: 0x361D41B0 },
    Warp { name: "Save Station Mines B", area: 0x7BD5E0BB },
    Warp { name: "Save Station Mines C", area: 0x66D0D003 },
    Warp { name: "Security Access A", area: 0xC7653A92 },
    Warp { name: "Security Access B", area: 0xA20201D4 },
    Warp { name: "Storage Depot A", area: 0x35C5D736 },
    Warp { name: "Storage Depot B", area: 0xE39C342B },
    Warp { name: "Transport Access", area: 0x42C4AAF1 },
    Warp { name: "Transport to Magmoor Caverns South", area: 0xE2C2CF38 },
    Warp { name: "Transport to Tallon Overworld South", area: 0x430E999C },
    Warp { name: "Ventilation Shaft", area: 0x90709AAC },
    Warp { name: "Waste Disposal", area: 0x27A391B7 }
];

#[rustfmt::skip]
pub const CRATER: [Warp; 12] = [
    Warp { name: "Crater Entry Point", area: 0x93668996 },
    Warp { name: "Crater Missile Station", area: 0x4D446C3F },
    Warp { name: "Crater Tunnel A", area: 0x49CB2363 },
    Warp { name: "Crater Tunnel B", area: 0x32D5A180 },
    Warp { name: "Metroid Prime Lair", area: 0x1A666C55 },
    Warp { name: "Phazon Core", area: 0xBD946AC3 },
    Warp { name: "Phazon Infusion Chamber", area: 0x67156A0D },
    Warp { name: "Subchamber Five", area: 0x77714498 },
    Warp { name: "Subchamber Four", area: 0xA7AC009B },
    Warp { name: "Subchamber One", area: 0xDADF06C3 },
    Warp { name: "Subchamber Three", area: 0x7A3AD91E },
    Warp { name: "Subchamber Two", area: 0x0749DF46 }
];
