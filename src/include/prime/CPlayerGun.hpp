#ifndef CPLAYERGUN_H
#define CPLAYERGUN_H

#include "PrimeAPI.h"

class CPlayerGun
{
public:
	void ChangeWeapon(const CPlayerState& rPlayerState, CStateManager& rStateMgr);
	void StopChargeSound(CStateManager& rStateMgr);
	void DropPowerBomb(CStateManager& rStateMgr) const;
	void FireSecondary(CStateManager& rStateMgr);
};

#endif