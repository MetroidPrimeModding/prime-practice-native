#pragma once

#include "types.h"

// Owns every memory card access the mod makes (a separate "PrimePractice" file in slot A).
//
// The game's CMemoryCardDriver polls the global per-channel CARD result code, so our commands must never overlap
// its. We therefore only touch the card while no driver exists, and drain any command in flight before a driver is
// constructed. See CARD_SETTINGS_PLAN.md.
namespace CardGate {
  /** Once per frame, from the render path. Never blocks. */
  void tick();

  /** Writes SETTINGS to the card as soon as no CMemoryCardDriver exists. */
  void requestSave();
  bool saveRequested();
  /** Remembers the player's position for the game save that is about to be made, and writes it once the card is free */
  void saveLocationWithGame();
  /** True while we are reading or writing the card */
  bool busy();
  /** SETTINGS differs from what is on the card (or what was last loaded) */
  bool dirty();
  /** Outcome of the last load/save for the UI, or null */
  const char *message();

  // Called by the CMemoryCardDriver constructor/destructor hooks
  void driverConstructing();
  void driverDestroyed();
} // namespace CardGate
