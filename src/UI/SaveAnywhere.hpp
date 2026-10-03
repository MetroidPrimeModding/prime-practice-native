#pragma once

// Opens the game's own save screen from any room. Loading the save puts the player at the room's default spawn;
// the exact position isn't stored.
namespace SaveAnywhere {
  // Why the game can't be saved right now, or nullptr. Ignores the pause screen the request is made from.
  const char *whyNot();

  // Closes the pause screen and opens the save screen once gameplay resumes.
  void request();

  // Pause screen hook: true once, when the pause screen should close for a request.
  bool consumeCloseRequest();

  // Per game tick.
  void update();
}
