///////////////////////////////////////////////////////////////////
////
////  Algrebra Functions
////
////  Jeff Barnard  2004
////
///////////////////////////////////////////////////////////////////

#ifndef _ALGEBRA_H
#define _ALGEBRA_H

#define PI 3.1415926535897932
#define SGN(x) ((x<0)?-1:((x>0)?1:0))  // returns normalized sign
#define TORADIAN( degree ) ((degree) * (PI / 180.0f))
#define TODEGREE( radian ) ((radian) * (180.0f / PI))

class Vector4;

class Vector3 {
	// A 3D vector class
    // This will be used to store vertices, points, directions etc.
public:
	float x, y, z;
	
	Vector3() {}

	Vector3(float X, float Y, float Z) {
		x = X; y = Y; z = Z;
	}

	void setValues(float X, float Y, float Z){
		x = X; y = Y; z = Z;
	}
    
	Vector3 operator+(Vector3 vVector) {
		return Vector3(vVector.x + x, vVector.y + y, vVector.z + z);
	}

	Vector3 operator-(Vector3 vVector) {
		return Vector3(x - vVector.x, y - vVector.y, z - vVector.z);
	}
	
	Vector3 operator*(float num) {
		return Vector3(x * num, y * num, z * num);
	}
	
	Vector3 operator/(Vector3 v) {
		return Vector3(x / v.x, y / v.y, z / v.z);
	}

	Vector3 operator/(float v) {
		return Vector3(x / v, y / v, z / v);
	}

	Vector3 operator*(Vector3 v) {
		return Vector3(x * v.x, y * v.y, z * v.z);
	}

	bool operator!=(Vector3 v){
		bool retval = true;
		if(x == v.x){
			if(y == v.y){
				if(z == v.z){
					retval = false;
				}
			}
		}		
		return retval;
	}

	//float operator*(Vector3 v) {		
	//	return Algebra::dot(*this, v);
	//}

    // reflect this Vector3 off surface with normal Vector3
	//const Vector3 inline Reflection(const Vector3& normal) const {
    //     const Vector3 vec(*this | 1);     // normalize
    //     return (vec - normal * 2.0 * (vec % normal)) * ! *this;
	//}
};

class Vector2 {
	public:
		// This will be used to store 2D vertices and texture coordinates.
		float x, y;

		Vector2() {}

		Vector2(float X, float Y) {
			x = X; y = Y;
		}

		Vector2 operator+(Vector2 vVector) {
			return Vector2(vVector.x + x, vVector.y + y);
		}

		Vector2 operator+(Vector3 vVector){
			return Vector2(vVector.x + x, vVector.z + y);
		}

		Vector2 operator-(Vector2 vVector) {
			return Vector2(x - vVector.x, y - vVector.y);
		}

		Vector2 operator*(float num) {
			return Vector2(x * num, y * num);
		}

		Vector2 operator/(float v) {
			return Vector2(x / v, y / v);
		}

		Vector2 operator/(Vector2 v){
			return Vector2(x/v.x, y/v.y);
		}
};

class Vector2d { // double precision
	public:
		double x, y;

		Vector2d(){}
		Vector2d(double X, double Y) {
			x = X; y = Y;
		}

		Vector2d operator+(Vector2d vVector) {
			return Vector2d(vVector.x + x, vVector.y + y);
		}

		Vector2d operator-(Vector2d vVector) {
			return Vector2d(x - vVector.x, y - vVector.y);
		}

		Vector2d operator*(double num) {
			return Vector2d(x * num, y * num);
		}

		Vector2d operator/(double v) {
			return Vector2d(x / v, y / v);
		}
};



namespace Algebra {

	class Vector4 {
		// A 4D vector class
		// Used for matrices to store row/col coordinates
	public:
		float x,y,z,w;

		Vector4 () {}

		Vector4(float X, float Y, float Z, float W) {
			x = X; y = Y; z = Z; w = W;
		}

		Vector4 operator+(Vector4 v) {
			return Vector4(v.x + x, v.y + y, v.z + z, v.w + w);
		}

		Vector4 operator-(Vector4 v) {
			return Vector4(x - v.x, y - v.y, z - v.z, w- v.w);
		}
		
		Vector4 operator*(float num) {
			return Vector4(x * num, y * num, z * num, w * num);
		}
	};

	struct Polygon {
		// just to clean things up a bit
	public:
		Vector3 verts[3];
		void setVerts(Vector3 a, Vector3 b, Vector3 c){
			verts[0] = a;
			verts[1] = b;
			verts[2] = c;
		}
		Vector3 * getPoly(void){
			return verts;
		}
	};

	enum position {
		BEHINDPLANE,
		INSIDEPLANE,
		OUTSIDEPLANE,
		INTERSECTPLANE
	};

	enum plane {
		A,			// the X value of the plane's normal
		B,			// the Y value of the plane's normal
		C,			// the Z value of the plane's normal
		D			// the distance from plane to origin
	};

	Vector3 rotateVector(Vector3 v, float theta, Vector3 around);

	Vector3 calculateVector(Vector3, Vector3);

	Vector2 calculateVector2D(Vector2, Vector2);

	Vector3 addVector(Vector3, Vector3);

	Vector3 divideVectorByScaler(Vector3, float);

	Vector3 cross(Vector3, Vector3 );

	Vector3 normalize3(Vector3);

	Vector2 normalize2(Vector2 vNormal);

	Vector3 normal(Vector3 *);

	float absolute(float);

	float magnitude(Vector3);

	float planeDistanceFromOrigin(Vector3, Vector3);

	void normalizePlane(float [6][4], int side);

	float dot(Vector3, Vector3);

	float getDistance(Vector3, Vector3);
	
	float getDistance2D(Vector3, Vector3);

	float getDistance2D2(Vector2 vPoint1, Vector2 vPoint2);

	double angleBetweenVectors(Vector3, Vector3);	

}

#endif