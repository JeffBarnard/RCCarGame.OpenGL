//////////////////////////////////////////////////////////////////////////////
////																			
////  Bounding Volumes
////
////  Jeff Barnard  2004
////	
//////////////////////////////////////////////////////////////////////////////

#ifndef _BOUNDING_H
#define _BOUNDING_H

#include "algebra.h"
#include "matrix4.h"
#include "glsetup.h"
#include "3ds.h"

namespace BoundingVolume {

	//////////////////////////////////////////////////////
	///
	/// 3D Oriented Bounding Box
	///
	//////////////////////////////////////////////////////
	class OBB3D {
	public:
		Vector3 center;
		// orientation matrices
		Algebra::Matrix4 rotationMatrix;
		Algebra::Matrix4 inverseMatrix;

		// 3 axes of orientation
		Vector3 origAxis[3];
		Vector3 xAxis, yAxis, zAxis;
		
		Vector3 verts[8]; // corner vertices
		Vector3 newVerts[8];
		
		// half-size of the box along each axes of orientation
		Vector3 halfSize;

		void setHalfSizes(float x, float y, float z);

		void setAxis(Vector3 &x, Vector3 &y, Vector3 &z);

		void setCenter(Vector2 &v);

		void setVerts(void);

		void updateVerts(void);

		void updateAxis(void);

		void drawBox(void);
	};

	//////////////////////////////////////////////////////
	///
	/// 3D Oriented Bounding Ellipsoid
	///
	//////////////////////////////////////////////////////
	class Ellipsoid {
	public:
		// orientation matrices
		Algebra::Matrix4 transformation;

		// radius vectors
		Vector3 radiusVector;
		Vector3 InvRadiusVector;

		// orientation vectors
        Vector3 center;
		Vector3 xradius, newXradius;
		Vector3 yradius, newYradius;
		Vector3 zradius, newZradius;
        
		void setCenter(Vector2 &v);
		Vector3 getCenter(void);

        void setRadius(Vector3 x, Vector3 y, Vector3 z);
		void updateRadius(void);

		void ellipsoidSpace(Vector3 v[], int j);
		void ellipsoidSpace(Vector3 &v);

		void sphereSpace(Vector3 v[], int j);
		void sphereSpace(Vector3 &v);

		void inverseSphereSpace(Vector3 v[], int j);
		void inverseSphereSpace(Vector3 &v);

		void inverseEllipsoidSpace(Vector3 v[], int j);
		void inverseEllipsoidSpace(Vector3 &v);

		void drawEllipsoid(void);
 	};

	//////////////////////////////////////////////////////
	///
	/// Axis Aligned Bounding Box
	///
	//////////////////////////////////////////////////////
	struct AABB3D {
		Vector3 min;
		Vector3 max;
		Vector3 center;
		float length;
		float width;
		float height;
	};

	// 2D axis aligned bounding box
	struct AABB2D {
		Vector2 min;
		Vector2 max;
	};

	//////////////////////////////////////////////////////
	///
	/// Bounding Sphere
	///
	//////////////////////////////////////////////////////
	class BoundingSphere {		
	public:
		Vector3 center;
		float radius;

		void setCenter(Vector2 &v){
			center.x = v.x;
			center.y = 0.1f;
			center.z = v.y;
		}

		bool sphereCollide(BoundingSphere *s){
			Vector3 relPos = center - s->center;
			float dist = relPos.x * relPos.x + relPos.y * relPos.y + relPos.z * relPos.z;
			float minDist = radius + s->radius;
			return dist <= minDist * minDist;
		}
	};

	bool calculateBoundingSphere(c3dsObject *thisObject, AABB3D &bb, BoundingSphere &sphere);

	bool calculateAABB3D(c3dsObject *thisObject, int size, AABB3D &bb);

	void calculateAABB2D(AABB2D &, Vector2 *);

	void drawAABB3D(const AABB3D);
}

#endif  // _BOUNDING_H

