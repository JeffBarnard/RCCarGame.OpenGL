//////////////////////////////////////////////////////////////////////////////
////																			
////  Quaternion Class
////
////  Jeff Barnard  2004
////
////  Thanks to Nick Bobick (nb@netcom.ca) for his excellent article on
////  quaternions posted at Gamasutra, July 3 1998
////
//////////////////////////////////////////////////////////////////////////////

#include "quaternion.h"

using namespace Algebra::Quaternion;

#define DELTA 1e-6


////////////////////////////// quatMultiply \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
/// Multiplies two Quaternions together, returns result
///
////////////////////////////// quatMultiply \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Quat Algebra::Quaternion::quatMultiply(Quat A, Quat B) {
	Quat C;

	C.x = A.w*B.x + A.x*B.w + A.y*B.z - A.z*B.y;
	C.y = A.w*B.y - A.x*B.z + A.y*B.w + A.z*B.x;
	C.z = A.w*B.z + A.x*B.y - A.y*B.x + A.z*B.w;
	C.w = A.w*B.w - A.x*B.x - A.y*B.y - A.z*B.z;

	return C;
}


////////////////////////////// quatConjugate \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
/// Returns the conjugate of q
///
////////////////////////////// quatConjugate \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Quat Algebra::Quaternion::quatConjugate(Quat q){
	Quat result;

	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w =  q.w;

	return result;
}


////////////////////////////// quaternionSlerp \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
/// Performs Spherical Linear Interpolation on two Quaternions
///
/// t=0 is "from", t=1 is "to", t=0.x is a SLERPed angle in between
///
////////////////////////////// quaternionSlerp \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::Quaternion::quaternionSlerp(Quat *from, Quat *to, float t, Quat *res) {

	float  to1[4];
    float  omega, 
		   cosom, 
		   sinom, 
		   scale0, 
		   scale1;

    // calc cosine
	///////////////////////////////////
    cosom = from->x * to->x + 
			from->y * to->y + 
			from->z * to->z + 
			from->w * to->w;

    // adjust signs (if necessary)
	//////////////////////////////////
    if ( cosom < 0.0 ){ 
		cosom = -cosom; 
		to1[0] = - to->x;
		to1[1] = - to->y;
		to1[2] = - to->z;
		to1[3] = - to->w;
	}else  {
		to1[0] = to->x;
		to1[1] = to->y;
		to1[2] = to->z;
		to1[3] = to->w;
    }

    // calculate coefficients
	////////////////////////////////////
    if ( (1.0 - cosom) > DELTA ) {
        // standard case (slerp)
        omega = acos(cosom);
        sinom = sin(omega);
        scale0 = sin((1.0f - t) * omega) / sinom;
        scale1 = sin(t * omega) / sinom;

	} else {        
		// very close together, so do a linear interpolation
        scale0 = 1.0f - t;
        scale1 = t;
    }

	// calculate final values
	////////////////////////////////
	res->x = scale0 * from->x + scale1 * to1[0];
	res->y = scale0 * from->y + scale1 * to1[1];
	res->z = scale0 * from->z + scale1 * to1[2];
	res->w = scale0 * from->w + scale1 * to1[3];
}


////////////////////////////// MatrixToQuat \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Converts a matrix (containing rotations) to a Quaternion representation
///
////////////////////////////// MatrixToQuat \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::Quaternion::matrixToQuat(float m[4][4], Quat *quat) {
  
	float tr, s, q[4];
	int i, j, k;

	int nxt[3] = {1, 2, 0};

	tr = m[0][0] + m[1][1] + m[2][2];

	// check the diagonal
	if (tr > 0.0) {
		s = sqrt (tr + 1.0f);
		quat->w = s / 2.0f;
		s = 0.5f / s;
		quat->x = (m[1][2] - m[2][1]) * s;
		quat->y = (m[2][0] - m[0][2]) * s;
		quat->z = (m[0][1] - m[1][0]) * s;
	} else {		
		// diagonal is negative
    	i = 0;
        if (m[1][1] > m[0][0]) i = 1;
	    if (m[2][2] > m[i][i]) i = 2;
        
		j = nxt[i];
        k = nxt[j];
        s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);
        
		q[i] = s * 0.5f;
            
        if (s != 0.0f) 
			s = 0.5f / s;

	    q[3] = (m[j][k] - m[k][j]) * s;
        q[j] = (m[i][j] + m[j][i]) * s;
        q[k] = (m[i][k] + m[k][i]) * s;

		quat->x = q[0];
		quat->y = q[1];
		quat->z = q[2];
		quat->w = q[3];
	}
}


////////////////////////////// QuatToMatrix \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Converts a Quaternion to a Matrix representation
///
////////////////////////////// QuatToMatrix \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::Quaternion::quatToMatrix(Quat *quat, float m[4][4]){

	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2; 


	// calculate coefficients
	x2 = quat->x + quat->x; y2 = quat->y + quat->y; 
	z2 = quat->z + quat->z;
	xx = quat->x * x2; xy = quat->x * y2; xz = quat->x * z2;
	yy = quat->y * y2; yz = quat->y * z2; zz = quat->z * z2;
	wx = quat->w * x2; wy = quat->w * y2; wz = quat->w * z2;


	m[0][0] = 1.0f - (yy + zz); m[1][0] = xy - wz;
	m[2][0] = xz + wy; m[3][0] = 0.0;

	m[0][1] = xy + wz; m[1][1] = 1.0f - (xx + zz);
	m[2][1] = yz - wx; m[3][1] = 0.0f;


	m[0][2] = xz - wy; m[1][2] = yz + wx;
	m[2][2] = 1.0f - (xx + yy); m[3][2] = 0.0f;


	m[0][3] = 0; m[1][3] = 0;
	m[2][3] = 0; m[3][3] = 1;
} 


////////////////////////////// EulerToQuat \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Converts an angle represented by Euler Form to a Quaternion
///
////////////////////////////// EulerToQuat \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::Quaternion::eulerToQuat(float roll, float pitch, float yaw, Quat *quat){
	float cr, cp, cy, sr, sp, sy, cpcy, spsy;

	// calculate trig identities
	cr = cos(roll/2);

	cp = cos(pitch/2);
	cy = cos(yaw/2);

	sr = sin(roll/2);
	sp = sin(pitch/2);
	sy = sin(yaw/2);
	
	cpcy = cp * cy;
	spsy = sp * sy;

	quat->w = cr * cpcy + sr * spsy;
	quat->x = sr * cpcy - cr * spsy;
	quat->y = cr * sp * cy + sr * cp * sy;
	quat->z = cr * cp * sy - sr * sp * cy;
}


////////////////////////////// QuatFromAxisAngle \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Disassembles quaternion to Axis Angle representation
///
/// Note: Angle is in Rads
///
////////////////////////////// QuatFromAxisAngle \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::Quaternion::quatToAxisAngle(Quat *quat, float *x, float *y, float *z, float *radians){
    float len;
    float tx, ty, tz;

	// cache variables
	tx = quat->x;
	ty = quat->y;
	tz = quat->z;

	len = tx * tx + ty * ty + tz * tz;

    if (len > DELTA){
		*x = tx * (1.0f / len);
		*y = ty * (1.0f / len);
		*z = tz * (1.0f / len);
	    *radians = (float)(2.0 * acos(quat->w));
    }
    else {
		*x = 0.0;
		*y = 0.0;
		*z = 1.0;
	    *radians = 0.0;
    }
}


////////////////////////////// AxisAngleToQuat \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Assembles quaternion from Axis Angle
///
/// Note: Angle has to be in Rads
///
////////////////////////////// AxisAngleToQuat \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Algebra::Quaternion::axisAngleToQuat(Quat *quat, float x, float y, float z, float angleRads) {
	
	/*float temp, dist;

	// normalize
	temp = x*x + y*y + z*z;

    dist = (float)(1.0 / sqrt(temp));

    x *= dist;
    y *= dist;
    z *= dist;

	quat->x = x;
	quat->y = y;
	quat->z = z;

	quat->w = (float)cos(angleRads / 2.0f);	*/

	Vector3 axis(x, y, z);
	axis = Algebra::normalize3(axis);

	float sin_a = sin( angleRads / 2 );
    float cos_a = cos( angleRads / 2 );

    quat->x    = axis.x * sin_a;
    quat->y    = axis.y * sin_a;
    quat->z    = axis.z * sin_a;

    quat->w    = cos_a;

}