//
// vector.h
//
// vector header file
//

#ifndef __VECTOR_H__
#define __VECTOR_H__

// includes
#include "base.h"

class Vector;

#include "matrix.h"

// structs

// vars

// funcs/classes

class Vector
{
public:
	float	x, y, z;

	// constructors
	Vector()								{ Set(0, 0, 0); }
	Vector(float _x, float _y, float _z)	{ Set(_x, _y, _z); }
	Vector(Vector &v)						{ Set(v.x, v.y, v.z); }

	// operators
	Vector operator +(Vector &v);	// return VectorAdd(this, v)
	void operator +=(Vector &v);	// this = VectorAdd(this, v)

	Vector operator -(Vector &v);	// return VectorSub(this, v)
	void operator -=(Vector &v);	// this = VectorSub(this, v)

	Vector operator *(Matrix &m);	// return MatrixMulVector(this, m)
	Vector operator *(Vector &v);	// return VectorMul(this, v)
	Vector operator *(float f);		// return VectorScale(this, f)
	void operator *=(Matrix &m);	// this = MatrixMulVector(this, m)
	void operator *=(Vector &v);	// this = VectorMul(this, v)
	void operator *=(float f);		// this = VectorScale(this, f)

	Vector operator ^(Vector &v);	// return VectorCross(this, v)
	void operator ^=(Vector &v);	// this = VectorCross(this, v)

	Vector operator !();			// return VectorNegate(this)

	// functions
	void	Set(float _x, float _y, float _z);
	float	Dot(Vector &v);
	float	SelfDot();
	Vector	Normalize();
	void	SelfNormalize();
	float	Length();
	float	Distance(Vector &v);
	Vector	Lerp(Vector &v, float f);
	void	SelfLerp(Vector &v, float f);

	float	*GetPtr() { return (float*)this; };
};


#endif //__VECTOR_H__