#include <include/os.h>
#include <include/prime/CGraphics.hpp>
#include <include/NewPauseScreen.hpp>
#include <include/prime/CSfxManager.hpp>
#include <include/prime/CMain.hpp>
//#include "include/practice_mod_rust.h"
#include "include/TextRenderer.hpp"
#include "include/prime/CWorld.hpp"

void draw_text(const uint8_t *str, uint32_t len, float x, float y) {
  TextRenderer::RenderText((const char *) str, len, x, y);
}

void text_color(float r, float g, float b, float a) {
  TextRenderer::SetColor(r, g, b, a);
}

void rust_error(const uint8_t *fmt, uint32_t len) {
  char *res = new char[len + 1];;
  for (uint32_t i = 0; i < len; i++) {
    res[i] = fmt[i];
  }
  res[len] = '\0';
  OSReport(res);
  delete res;
}

float get_fps() {
  return CGraphics::GetFPS();
}

bool is_pause_screen() {
  return NewPauseScreen::instance->active;
}

void warp(uint32_t world, uint32_t area) {
  CAssetId worldID = (CAssetId) (world);
  CAssetId areaID = (CAssetId) (area);

  CStateManager *mgr = ((CStateManager *) 0x8045A1A8);
  mgr->GetWorld()->SetPauseState(true);

  CGameState *gameState = *((CGameState **) (0x80457798 + 0x134));
  CSfxManager::SfxStart(0x59A, 0x7F, 0x40, false, 0x7F, false, kInvalidAreaId);
  gameState->SetCurrentWorldId(worldID);
  gameState->CurrentWorldState().SetDesiredAreaAssetId(areaID);

  CMain *cmain = *((CMain **) 0x805A8C38);
  cmain->SetFlowState(EFlowState_None);

  mgr->SetShouldQuitGame(true);

  NewPauseScreen::instance->hide();

}
