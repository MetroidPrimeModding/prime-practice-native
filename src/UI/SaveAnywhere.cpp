#include "SaveAnywhere.hpp"
#include "prime/CCameraManager.hpp"
#include "system/CardGate.hpp"
#include "prime/CGameState.hpp"
#include "prime/CPlayer.hpp"
#include "prime/CStateManager.hpp"

namespace SaveAnywhere {
  namespace {
    enum class State { Idle, Closing, Waiting };
    State state = State::Idle;
    int waitFrames = 0;
    // Gives up if the pause screen never finishes closing
    constexpr int kMaxWaitFrames = 300;
  }

  const char *whyNot() {
    CPlayer *player = g_StateManager.Player();
    if (!gpGameState || !player || g_StateManager.GetInitPhase() != CStateManager::kInit_Done) {
      return "No game in progress";
    }
    if (gpGameState->CardSerial() == 0) return "No memory card to save to";
    if (player->getDeathTime() > 0.f) return "Samus is dead";
    if (g_StateManager.GetGameState() != CStateManager::EGameState::Running) return "A scan is being shown";
    if (g_StateManager.x870_cameraManager()->IsInCinematicCamera()) return "A cutscene is playing";
    if (g_StateManager.GetEscapeSequenceTimer() > 0.f) return "The escape sequence is under way";
    auto morph = player->getMorphBallState();
    if (morph == CPlayer::EMorphBallState::Morphing || morph == CPlayer::EMorphBallState::Unmorphing) {
      return "Samus is morphing";
    }
    return nullptr;
  }

  void request() {
    if (state == State::Idle && !whyNot()) state = State::Closing;
  }

  bool consumeCloseRequest() {
    if (state != State::Closing) return false;
    state = State::Waiting;
    waitFrames = 0;
    return true;
  }

  void update() {
    if (state != State::Waiting) return;
    // The pause screen holds the deferred transition until the game has been unpaused
    if (g_StateManager.GetDeferredStateTransition() != EStateManagerTransition::InGame) {
      if (++waitFrames > kMaxWaitFrames) state = State::Idle;
      return;
    }
    state = State::Idle;
    if (whyNot()) return;
    CardGate::saveLocationWithGame();
    g_StateManager.DeferStateTransition(EStateManagerTransition::SaveGame);
  }
}
