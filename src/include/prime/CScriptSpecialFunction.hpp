#pragma once

#include "CStateManager.hpp"
#include "prime/CActor.hpp"

class CScriptSpecialFunction : public CActor {
public:
  bool ShouldSkipCinematic(CStateManager& stateMgr) const;
};
