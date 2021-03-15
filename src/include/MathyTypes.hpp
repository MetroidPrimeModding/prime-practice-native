#ifndef PRIME_PRACTICE_CMATRIX3F_HPP
#define PRIME_PRACTICE_CMATRIX3F_HPP

#include "types.h"

struct CVector3f {
public:
    float x;
    float y;
    float z;

    inline CVector3f() : x(0), y(0), z(0) {};

    inline CVector3f(float x, float y, float z) : x(x), y(y), z(z) {};
};

struct CQuaternion {
public:
    float w;
    float x;
    float y;
    float z;

    inline CQuaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {};
};

struct CAxisAngle : CVector3f {
    inline CAxisAngle(float x, float y, float z) : CVector3f(x, y, z) {}
};

class CTransform4f {
public:
    float matrix[12];

    static inline CTransform4f Identity() {
      CTransform4f res;

      float mat[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
      };

      res.matrix[0] = mat[0];
      res.matrix[1] = mat[1];
      res.matrix[2] = mat[2];
      res.matrix[3] = mat[3];
      res.matrix[4] = mat[4];
      res.matrix[5] = mat[5];
      res.matrix[6] = mat[6];
      res.matrix[7] = mat[7];
      res.matrix[8] = mat[8];
      res.matrix[9] = mat[9];
      res.matrix[10] = mat[10];
      res.matrix[11] = mat[11];
//      res.matrix[12] = mat[12];
//      res.matrix[13] = mat[13];
//      res.matrix[14] = mat[14];
//      res.matrix[15] = mat[15];

      return res;
    }

    static CTransform4f Translate(float x, float y, float z);

    inline CVector3f origin() { return CVector3f(matrix[3], matrix[3 + 4], matrix[3 + 8]); };
};

class CAABox {
public:
    CVector3f min;
    CVector3f max;
};

class CFrustum {
public:
    float data[16];
};

class CRelAngle
{
public:
	float mAngle;
	
	// All functions inlined in Metroid Prime
	inline CRelAngle () {}
	inline CRelAngle(float inAngle) : mAngle(inAngle) {}
	
	inline CRelAngle& operator=(float inAngle)
	{
		mAngle = inAngle;
		return *this;
	}
};

#endif //PRIME_PRACTICE_CMATRIX3F_HPP
