//////////////////////////////////////////////////////////////////////////////
////																			
////  Quaternion Class (Used Only by Camera Class For Now)
////
////  Jeff Barnard  2004
////
////  Thanks to Nick Bobick (nb@netcom.ca) for his excellent article on
////  quaternions posted at Gamasutra, July 3 1998
////
//////////////////////////////////////////////////////////////////////////////

#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "algebra.h"
#include "math.h"

namespace Algebra{
	
	namespace Quaternion{

		////////////////////////////////////////////////////////////////////////////////
		///
		/// Quaternion Class
		///
		/// Defined by [w, v] (where v = (x, y, z) is a vector and w is a scalar)
		///
		////////////////////////////////////////////////////////////////////////////////
		class Quat{

		public:
			float w;	     // scaler
			float x,y,z;     // vector

		public:
			// construction
			///////////////////////////////////////////
			Quat(){}
			Quat(float scaler, Vector3 v){
				w = scaler;
				x = v.x;
				y = v.y;
				z = v.z;
			}
					
			// public methods
			///////////////////////////////////////////
			void add(Quat q){
				w = w + q.w;
				x = x + q.x;
				y = y + q.y;
				z = z + q.z;
			}

			float length(Quat quat) {
				return sqrt(quat.x * quat.x + quat.y * quat.y +
							quat.z * quat.z + quat.w * quat.w);
			}

			Quat normalize(Quat quat) {
				float L = length(quat);
				quat.x /= L;
				quat.y /= L;
				quat.z /= L;
				quat.w /= L;

				return quat;
			}

			void operator=(Quat q){
				x = q.x;
				y = q.y;
				z = q.z;
				w = q.w;
			}
		};

		Quat quatMultiply(Quat A, Quat B);

		Quat quatConjugate(Quat q);
		
		void matrixToQuat(float m[4][4], Quat *quat);
 
		void quatToMatrix(Quat *quat, float m[4][4]); 

		void eulerToQuat(float roll, float pitch, float yaw, Quat *quat);

		void quaternionSlerp(Quat *from, Quat *to, float t, Quat *res);

		void quatToAxisAngle(Quat *quat, float *x, float *y, float *z, float *radians);
	    
		void axisAngleToQuat(Quat *quat, float x, float y, float z, float angle);

	}
}

#endif