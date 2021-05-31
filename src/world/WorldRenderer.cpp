#include <settings.hpp>
#include <prime/CStateManager.hpp>
#include <prime/CScriptTrigger.hpp>
#include <prime/CScriptRelay.hpp>
#include <prime/CScriptDock.hpp>
#include <prime/CScriptDoor.hpp>
#include <prime/CScriptCameraHint.hpp>
#include "WorldRenderer.hpp"

namespace WorldRenderer {
  void drawTrigger(CObjectList *list, CScriptTrigger *trigger);
  ETriggerType determineTriggerType(CObjectList *list, CScriptTrigger *trigger);

  void RenderWorld() {
    // TODO: re-implement this
    if (!SETTINGS.TRIGGER_anyOn()) {
      return;
    }

    CStateManager *mgr = CStateManager_INSTANCE;
    CObjectList *list = mgr->GetAllObjs();
    if (list == nullptr) return;

    SViewport backupViewport = *SVIEWPORT_GLOBAL;
    mgr->SetupViewForDraw(backupViewport);

    CGraphics::SetCullMode(ERglCullMode_Back);
    CGX::SetBlendMode(GxBlendMode_BLEND, GxBlendFactor_SRCALPHA, GxBlendFactor_INVSRCALPHA, GxLogicOp_OR);
    CGX::SetZMode(true, GxCompare_LEQUAL, false);
    CGraphics::SetAlphaCompare(ERglAlphaFunc_GREATER, 0, ERglAlphaOp_OR, ERglAlphaFunc_GREATER, 0);
    CGraphics::DisableAllLights();

    CGX::SetNumTevStages(1);
    CGX::SetTevOrder(
        GXTevStage0,
        GXTexCoordNull,
        GXTexMapNull,
        GXChannelColor0A0
    );
    CGX::SetTevColorIn(GXTevStage0, GxTevColorArg_ZERO, GxTevColorArg_ZERO, GxTevColorArg_ZERO, GxTevColorArg_RASC);
    CGX::SetTevAlphaIn(GXTevStage0, GxTevAlphaArg_ZERO, GxTevAlphaArg_ZERO, GxTevAlphaArg_ZERO, GxTevAlphaArg_RASA);
    CGX::SetTevColorOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);
    CGX::SetTevAlphaOp(GXTevStage0, GxTevOp_ADD, GxTevBias_ZERO, GxTevScale_SCALE_1, GX_TRUE, GxTevRegID_TEVPREV);

    CGraphics::StreamBegin(ERglPrimitive_QUADS);
    int visited = 0;
    int id = list->first;
    while (id != 0xFFFF && visited < list->count) {
      SObjectListEntry entry = list->entries[id & 0x3FF];
      if (!VALID_PTR(entry.entity)) {
        break;
      }
      CEntity *entity = entry.entity;
      if (entity->getVtablePtr() == CScriptTrigger::VTABLE_ADDR) {
        CScriptTrigger *trigger = reinterpret_cast<CScriptTrigger *>(entity);
        drawTrigger(list, trigger);
      }
      visited++;
      id = entry.next;
    }

    CGraphics::StreamEnd();
  }

  void drawTrigger(CObjectList *list, CScriptTrigger *trigger) {
    if ((trigger->getStatus() & CEntity::ACTIVE_MASK) == 0) {
      return;
    }

    if (*trigger->getTriggerType() == ETriggerType::NotYetDetermined) {
      *trigger->getTriggerType() = determineTriggerType(list, trigger);
    }

    bool render;
    float alpha = 0.05f;
    switch (*trigger->getTriggerType()) {
      case ETriggerType::Load:
        render = SETTINGS.TRIGGER_renderLoad;
        CGraphics::StreamColor(0.2f, 0.6f, 0.3f, alpha);
        break;
      case ETriggerType::Door:
        render = SETTINGS.TRIGGER_renderDoor;
        CGraphics::StreamColor(0.55f, 0.91f, 0.97f, alpha);
        break;
      case ETriggerType::Force:
        render = SETTINGS.TRIGGER_renderForce;
        CGraphics::StreamColor(0.75f, 0.75f, 0.25f, alpha);
        break;
      case ETriggerType::CameraHint:
        render = SETTINGS.TRIGGER_renderCameraHint;
        CGraphics::StreamColor(0.95, 0.6, 0.91, alpha);
        break;
      case ETriggerType::Unknown:
        render = SETTINGS.TRIGGER_renderUnknown;
        CGraphics::StreamColor(0.8f, 0.8f, 0.8f, alpha);
        break;
      case ETriggerType::NotYetDetermined:
        render = true; // should never happen
        CGraphics::StreamColor(0.8f, 0.0f, 0.0f, alpha);
        break;
      default:
        // Should never happen
        render = true;
        CGraphics::StreamColor(0.8f, 0.0f, 0.0f, alpha);
    }

    if (!render) {
      return;
    }

    CTransform4f *transform = trigger->getTransform();
    CVector3f origin = transform->origin();
    CAABox *aabb = trigger->getBounds();
    CVector3f min(
        origin.x + aabb->min.x,
        origin.y + aabb->min.y,
        origin.z + aabb->min.z
    );
    CVector3f max(
        origin.x + aabb->max.x,
        origin.y + aabb->max.y,
        origin.z + aabb->max.z
    );

    //-z
    CGraphics::StreamVertex(max.x, min.y, min.z);
    CGraphics::StreamVertex(max.x, max.y, min.z);
    CGraphics::StreamVertex(min.x, max.y, min.z);
    CGraphics::StreamVertex(min.x, min.y, min.z);

    //+z
    CGraphics::StreamVertex(min.x, min.y, max.z);
    CGraphics::StreamVertex(min.x, max.y, max.z);
    CGraphics::StreamVertex(max.x, max.y, max.z);
    CGraphics::StreamVertex(max.x, min.y, max.z);

    //-x
    CGraphics::StreamVertex(min.x, min.y, min.z);
    CGraphics::StreamVertex(min.x, max.y, min.z);
    CGraphics::StreamVertex(min.x, max.y, max.z);
    CGraphics::StreamVertex(min.x, min.y, max.z);

    //+x
    CGraphics::StreamVertex(max.x, min.y, max.z);
    CGraphics::StreamVertex(max.x, max.y, max.z);
    CGraphics::StreamVertex(max.x, max.y, min.z);
    CGraphics::StreamVertex(max.x, min.y, min.z);

    //-y
    CGraphics::StreamVertex(min.x, min.y, max.z);
    CGraphics::StreamVertex(max.x, min.y, max.z);
    CGraphics::StreamVertex(max.x, min.y, min.z);
    CGraphics::StreamVertex(min.x, min.y, min.z);

    //+y
    CGraphics::StreamVertex(min.x, max.y, min.z);
    CGraphics::StreamVertex(max.x, max.y, min.z);
    CGraphics::StreamVertex(max.x, max.y, max.z);
    CGraphics::StreamVertex(min.x, max.y, max.z);

    CGraphics::FlushStream();
  }

  CEntity *getEntityWithEditorID(CObjectList *list, u32 objid) {
    int visited = 0;
    u16 uniqueID = list->first;
    while (uniqueID != 0xFFFF && visited < list->count) {
      SObjectListEntry entry = list->entries[uniqueID & 0x3FF];
      if (entry.entity->getEditorID() == objid) {
        return entry.entity;
      }
      visited++;
      uniqueID = entry.next;
    }
    return nullptr;
  }

  bool entityHasVtableOrIsRelayThatPointsAtVtable(CObjectList *list, u32 objectID, u32 vtable, u32 depth = 0) {
    CEntity *entity = getEntityWithEditorID(list, objectID);
    if (!entity) return false;
    // found the object
    // If we're a relay, recurse
    if (entity->getVtablePtr() == vtable) return true;
    if (entity->getVtablePtr() == CScriptRelay::VTABLE_ADDR & depth < 2) {
      auto connections = entity->getConnections();
      for (u32 i = 0; i < connections->len; i++) {
        auto conn = &connections->ptr[i];
        if (entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, vtable, depth + 1)) {
          return ETriggerType::Load;
        }
      }
    }
    return false;

  }

  ETriggerType determineTriggerType(CObjectList *list, CScriptTrigger *trigger) {
    auto connections = trigger->getConnections();
    for (u32 i = 0; i < connections->len; i++) {
      auto conn = &connections->ptr[i];
      if (conn->x0_state == EScriptObjectState::Entered) {
        if (entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, CScriptDock::VTABLE_ADDR)) {
          return ETriggerType::Load;
        }
      }
      if (conn->x0_state == EScriptObjectState::Inside && conn->x4_msg == EScriptObjectMessage::Open) {
        if (!entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, CScriptDoor::VTABLE_ADDR)) {
          return ETriggerType::NotYetDetermined;
        }
        return ETriggerType::Door;
      }
      if (trigger->getForceMagnitude() > 1 || trigger->getForceMagnitude() < -1) {
        return ETriggerType::Force;
      }
      float forceVec = trigger->getForceField()->x * trigger->getForceField()->x
                       + trigger->getForceField()->y * trigger->getForceField()->y
                       + trigger->getForceField()->z * trigger->getForceField()->z;
      if (forceVec > 1) {
        return ETriggerType::Force;
      }
      if (entityHasVtableOrIsRelayThatPointsAtVtable(list, conn->x8_objId, CScriptCameraHint::VTABLE_ADDR)) {
        return ETriggerType::CameraHint;
      }
    }
    return ETriggerType::Unknown;
  }
}
