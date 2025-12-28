# Metroid Prime Practice Mod
This project can produce a release & patch a metroid prime iso to be the practice mod

## How to install (Windows)
- Get a metroid prime .iso (i.e. using a hacked wii and cleanrip)
- Copy a metroid prime iso to 'prime.iso' in this directory
- Run `patch.bat`

## How to install (Non-windows)
- Make sure you have python3 installed (it should come with most linux distros/macOS)
- Get a metroid prime .iso (i.e. using a hacked wii and cleanrip)
- Copy a metroid prime iso to 'prime.iso' in this directory
- Run `patch.sh` from a terminal
  - You can drag and drop the script into a terminal to get the full path

## Usage
- Launch the iso in Dolphin
- Copy to your wii, and launch using Nintendont
  - If your Nintendont is really old, update Nintendont
  - Note this mod is not currently compatible with some settings, at the very least:
     - Cheats
     - Force progressive
     - Force Widescreen
  - There may be others, not a lot of testing has been done yet
  
## Controls
- `Z` on pause screen - Open menu
- `left analog`/`dpad` - Navigate menu
- `c-stick` - Scroll
- `a` - Select item in menu
- `b` - Go back a menu
- `x` - (on map) warp to selected room

## Known Issues
- Rare crashes (so far, inconsistent)
- Warps to a couple rooms will put you out of bounds

## Menus
- Player
  - Save/load position
  - Edit position
  - Light Show/IS toggling
  - Toggle Floaty Jump
- Inventory
  - Edit your item inventory
- Settings
  - On screen display settings
  - Bomb jump helper
  - Trigger display settings
  - RNG manipulation (be careful, this can crash the game)
  - RAM Dump to screen (no decoder yet)
- Warps
  - Warp to any room
  - You can also press 'x' on the map screen to warp
- Rooms
  - Modify room visibility
  - Load/unload rooms
- Version number
  - QR code links to releases and the discord
  
## OSD
- Current IGT
- Input Display
- Player Information
  - Current position (x, y, z)
  - Current speed (x, y, z, horizontal total)
  - Currrent rotational speed (x, y, z, total)
- Room timers
  - Current room IGT, in seconds
  - Current room IGT, in frames
  - Previous room IGT, in seconds
  - Previous room IGT, in frames
  - By default updates when you enter a new room
  - Can be configured to update when a room begins loading
- Frame time graph, as measured by the game
- Memory usage graph
- Current RNG value
- Incinerator Drone fight details

## Changelog

### 2.8.0
- Add option to show targeted object info (including hp, asset id, etc)
- Add option for infinite scans
- Add option to complete/reset all scans

### 2.7.0
- Add incinerator drone fight details to OSD
- RNG value display and locking
- Room timer can be set to update on room load instead of room transition
- Experimental memory dumper to screen (no decoder yet)

### 2.6.0/2.6.1
- Press 'x' to warp on map
- Added link to releases and the discord

### 2.5.0
- Configurable maximum bomb count
- Infinite bombs cheat
- All cutscenes may be skipped (as if they had been seen already)
- Reduce memory footprint by ~64k by using unused sections of bss

### 2.4.1
- Add option to occlude/show rooms (e.g. "where's the room")
- Add load/unload room option (not perfect)

### 2.4.0
- New option to show current loads in bottom left

### 2.3.4
- Floaty jump added to menu
- Split out warp from load pos
- Hotkey: dpad-up x3 to load pos
- Hotkey: dpad-down x3 to save pos
- Move warps menu to bottom (to make scrolling easier)
- Add option to toggle current and previous room timers individually
- Set default custom bomb jump to UBJ

### 2.3.3
- Fix the bomb jump timing lines

### 2.3.2
- Add rotational speed back as OSD option
- Swap current and last room ordering
- Bomb jump display/helper
- Automatically show crash screen on crash
- Move the UI a bit further towards the edges

### 2.3.1 (beta)
- Add IS & lightshowfix to player menu
- Add version to main menu

### 2.3.0 (beta)
- Compile statically & link directly into default.dol
- This reduces the size of the mod by ~100k

### 2.2.4 (beta)
- Fix running in Nintendont 
- Revert from direct GX rendering back to Stream rendering, until I can figure out why it crashes nintendont

### 2.2.3 (beta)
- Switch to a more efficient renderer (and fix some rendering bugs)
- Add item presets for Start, Any%, and 21%
- Save position now saves orientation and velocity
- Save position now saves room and position
- Unfortunately, it can currently only load room *or* position. Hopefully can fix this later.

### 2.2.2 (beta)
- Save menu input location across menu opening/closing

### 2.2.1 (beta)
- Fix texture corruption
- Reduce size by another ~65k
- Make the menu fit on the screen properly at all times
- Remove window border and shrink window padding
- Move input to bottom right & default to on
- Back due to popular demand, Magmoor has been added to the warp menu

### 2.2.0 (beta)
- Reduce footprint by about ~200k
- Fix font rendering to be far more legible
- Fix room timer bug

### 2.1.0 (beta)
- Add pyinstaller pre-built windows .exe file
- Add trigger viewing
- Add save/load position
- Add IGT and room time display
- Add pos/velocity display

### 2.0.0 (beta)
- Complete rewrite! Not feature complete
- Press Z with pause menu open to access the menu
- Currently requires installed python3.7+

### 1.2.5
- Fix warps, so they warp you to the destination, instead of the credits

### 1.2.4
- Remove ZOOM from next to the IGT on save select
- Allow IGT and input to be configured separately

### 1.2.3
- Add position/precise position displays
- Change file select to show mm:ss instead of hh:mm

### 1.2.2
- Rewrote patcher to native (much smaller, faster)

### 1.2.1
- IS enabler (requires morph)
- Lightshow Fix (requires unmorphed)
- Update patcher
- May have fixed soft-reset crash

### 1.2.0
- Add trigger rendering

### 1.1.2
- Minor performance improvements
- Internal rework of how native is built

### 1.1.1
- Minor fixes

### 1.1.0
- Add Player menu
- Add FPS counter

### 1.0.0
 - Initial Release
