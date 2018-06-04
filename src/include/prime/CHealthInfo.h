#ifndef CHEALTHINFO_H
#define CHEALTHINFO_H

#include "PrimeAPI.h"

class CHealthInfo
{
	float mHealth;
	float mKnockbackResistance;
	
public:
	inline CHealthInfo()
		: mHealth(0.f), mKnockbackResistance(0.f)
	{
	}
	
	CHealthInfo(float health, float resistance);
	
	// Accessors
	inline void SetHealth(float health)						{ mHealth = health; }
	inline void SetKnockbackResistance(float resistance)	{ mKnockbackResistance = resistance; }
	inline float GetHealth() const							{ return mHealth; }
	inline float GetKnockbackResistance() const				{ return mKnockbackResistance; }
};

#endif