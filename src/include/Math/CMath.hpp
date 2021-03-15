#ifndef CMATH_H
#define CMATH_H

class CVector3f;

class CMath
{
public:
	static int FloorPowerOfTwo(int);
	static float FastArcCosR(float);
	static float FastCosR(float);
	static float FastSinR(float);
	static CVector3f BaryToWorld(const CVector3f&, const CVector3f&, const CVector3f&, const CVector3f&);
	static CVector3f GetBezierPoint(const CVector3f&, const CVector3f&, const CVector3f&, const CVector3f&, float);
	static float GetCatmullRomSplinePoint(float, float, float, float, float);
	static CVector3f GetCatmullRomSplinePoint(const CVector3f&, const CVector3f&, const CVector3f&, const CVector3f&, float);
	static float CeilingF(float);
	static float FloorF(float);
	static float SlowTangentR(float);
	static float SlowCosineR(float);
	static float SlowSineR(float);
	static float PowF(float float);
	static float ArcTangentR(float);
	static float ArcCosineR(float);
	static float ArcSineR(float);
	static float InvSqrtF(float);
	static double SqrtD(double);
	static float SqrtF(float);
};

#endif // CMATH_H