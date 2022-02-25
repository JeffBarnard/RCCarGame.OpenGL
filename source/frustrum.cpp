//////////////////////////////////////////////////////////////////////////////
////																			
////  View Frustrum
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

#include "frustrum.h"

using namespace Algebra;


/////////////////////////////////////// updateFrustrum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
////
////  Thanks to Gil Gribb and Klaus Hartmann for their article on plane extraction
////  http://www2.ravensoft.com/users/ggribb/plane%20extraction.pdf
////
/////////////////////////////////////// updateFrustrum \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Frustrum::updateFrustrum(void){

	float   projection[16];		// projection matrix
	float   modelview[16];		// modelview matrix
	float   clip[16];			// clipping planes

	// extract GL_PROJECTION_MATRIX
	glGetFloatv( GL_PROJECTION_MATRIX, projection );

	// extract GL_MODELVIEW_MATRIX
	glGetFloatv( GL_MODELVIEW_MATRIX, modelview );

	// multiply both and get clipping planes
	clip[0] = modelview[ 0] * projection[ 0] + modelview[ 1] * projection[ 4] + modelview[ 2] * projection[ 8] + modelview[ 3] * projection[12];
	clip[1] = modelview[ 0] * projection[ 1] + modelview[ 1] * projection[ 5] + modelview[ 2] * projection[ 9] + modelview[ 3] * projection[13];
	clip[2] = modelview[ 0] * projection[ 2] + modelview[ 1] * projection[ 6] + modelview[ 2] * projection[10] + modelview[ 3] * projection[14];
	clip[3] = modelview[ 0] * projection[ 3] + modelview[ 1] * projection[ 7] + modelview[ 2] * projection[11] + modelview[ 3] * projection[15];

	clip[4] = modelview[ 4] * projection[ 0] + modelview[ 5] * projection[ 4] + modelview[ 6] * projection[ 8] + modelview[ 7] * projection[12];
	clip[5] = modelview[ 4] * projection[ 1] + modelview[ 5] * projection[ 5] + modelview[ 6] * projection[ 9] + modelview[ 7] * projection[13];
	clip[6] = modelview[ 4] * projection[ 2] + modelview[ 5] * projection[ 6] + modelview[ 6] * projection[10] + modelview[ 7] * projection[14];
	clip[7] = modelview[ 4] * projection[ 3] + modelview[ 5] * projection[ 7] + modelview[ 6] * projection[11] + modelview[ 7] * projection[15];

	clip[8] = modelview[ 8] * projection[ 0] + modelview[ 9] * projection[ 4] + modelview[10] * projection[ 8] + modelview[11] * projection[12];
	clip[9] = modelview[ 8] * projection[ 1] + modelview[ 9] * projection[ 5] + modelview[10] * projection[ 9] + modelview[11] * projection[13];
	clip[10] = modelview[ 8] * projection[ 2] + modelview[ 9] * projection[ 6] + modelview[10] * projection[10] + modelview[11] * projection[14];
	clip[11] = modelview[ 8] * projection[ 3] + modelview[ 9] * projection[ 7] + modelview[10] * projection[11] + modelview[11] * projection[15];

	clip[12] = modelview[12] * projection[ 0] + modelview[13] * projection[ 4] + modelview[14] * projection[ 8] + modelview[15] * projection[12];
	clip[13] = modelview[12] * projection[ 1] + modelview[13] * projection[ 5] + modelview[14] * projection[ 9] + modelview[15] * projection[13];
	clip[14] = modelview[12] * projection[ 2] + modelview[13] * projection[ 6] + modelview[14] * projection[10] + modelview[15] * projection[14];
	clip[15] = modelview[12] * projection[ 3] + modelview[13] * projection[ 7] + modelview[14] * projection[11] + modelview[15] * projection[15];
	
	// extract sides from clipping planes
	///////////////////////////////////////////////
	myFrustrum[RIGHT][A] = clip[ 3] - clip[ 0];
	myFrustrum[RIGHT][B] = clip[ 7] - clip[ 4];
	myFrustrum[RIGHT][C] = clip[11] - clip[ 8];
	myFrustrum[RIGHT][D] = clip[15] - clip[12];
	normalizePlane(myFrustrum, RIGHT);

	myFrustrum[LEFT][A] = clip[ 3] + clip[ 0];
	myFrustrum[LEFT][B] = clip[ 7] + clip[ 4];
	myFrustrum[LEFT][C] = clip[11] + clip[ 8];
	myFrustrum[LEFT][D] = clip[15] + clip[12];
	normalizePlane(myFrustrum, LEFT);

	myFrustrum[BOTTOM][A] = clip[ 3] + clip[ 1];
	myFrustrum[BOTTOM][B] = clip[ 7] + clip[ 5];
	myFrustrum[BOTTOM][C] = clip[11] + clip[ 9];
	myFrustrum[BOTTOM][D] = clip[15] + clip[13];
	normalizePlane(myFrustrum, BOTTOM);

	myFrustrum[TOP][A] = clip[ 3] - clip[ 1];
	myFrustrum[TOP][B] = clip[ 7] - clip[ 5];
	myFrustrum[TOP][C] = clip[11] - clip[ 9];
	myFrustrum[TOP][D] = clip[15] - clip[13];
	normalizePlane(myFrustrum, TOP);

	myFrustrum[BACK][A] = clip[ 3] - clip[ 2];
	myFrustrum[BACK][B] = clip[ 7] - clip[ 6];
	myFrustrum[BACK][C] = clip[11] - clip[10];
	myFrustrum[BACK][D] = clip[15] - clip[14];
	normalizePlane(myFrustrum, BACK);

	myFrustrum[FRONT][A] = clip[ 3] + clip[ 2];
	myFrustrum[FRONT][B] = clip[ 7] + clip[ 6];
	myFrustrum[FRONT][C] = clip[11] + clip[10];
	myFrustrum[FRONT][D] = clip[15] + clip[14];
	normalizePlane(myFrustrum, FRONT);

	return true;
}


/////////////////////////////////////// containsSphere \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////   Tests if a sphere is within the frustrum
/////
/////   Note, work in progress, its not being used.  It does not work.
/////
/////////////////////////////////////// containsSphere \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int Frustrum::containsSphere(BoundingVolume::BoundingSphere& refSphere) {
	float distance;

	// calculate our distances to each of the planes
	for(int i = 0; i < 6; i++) {

		distance = myFrustrum[i][A] * refSphere.center.x + myFrustrum[i][B] * 
					refSphere.center.y + myFrustrum[i][C] * refSphere.center.z + myFrustrum[i][D];
        
    	//distance = m_plane[i].normal().dotProduct(refSphere.center())+m_plane[i].distance();

		// if this distance is < -sphere.radius, then its outside
		if(distance < -refSphere.radius)
			return OUTSIDEPLANE;

		// else if the distance is between +- radius, then its intersecting
		if((float)fabs(distance) < refSphere.radius)
			return INTERSECTPLANE;
	}

	return INSIDEPLANE;
}


////////////////////////////////// containsAABB \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////   Tests if an AABB is within the frustrum
/////
////////////////////////////////// containsAABB \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int Frustrum::containsAABB(BoundingVolume::AABB3D &bb) {

	Vector3 corners[8];
	//int totalIn = 0;

	// get the corners of the box 
	corners[0].x = bb.min.x;  corners[1].x = bb.max.x;  corners[2].x = bb.min.x;
	corners[0].y = bb.min.y;  corners[1].y = bb.max.y;  corners[2].y = bb.max.y;
	corners[0].z = bb.min.z;  corners[1].z = bb.max.z;  corners[2].z = bb.max.z;
	
	corners[3].x = bb.min.x;  corners[4].x = bb.max.x;  corners[5].x = bb.min.x;
	corners[3].y = bb.min.y;  corners[4].y = bb.min.y;  corners[5].y = bb.max.y;
	corners[3].z = bb.max.z;  corners[4].z = bb.min.z;  corners[5].z = bb.min.z;

	corners[6].x = bb.max.x;  corners[7].x = bb.max.x;
	corners[6].y = bb.min.y;  corners[7].y = bb.max.y;
	corners[6].z = bb.max.z;  corners[7].z = bb.min.z;

	// test all 8 corners against the 6 sides 
	// if all points are behind 1 specific plane, its out of view
	for(int j = 0; j < 6; j++) {
		int inCount = 8;
		//int pointsIn = 1;

		for(int i = 0; i < 8; i++) {

			// test this point against the planes
			if (myFrustrum[j][A] * corners[i].x + myFrustrum[j][B] * corners[i].y + 
				myFrustrum[j][C] * corners[i].z + myFrustrum[j][D] <= 0) {

				//pointsIn = 0;
				--inCount;
			}
		}

		// were all the points outside of the plane?
		if(inCount == 0)
			return OUTSIDEPLANE;

		// check if they were all on the proper side of the plane
		//totalIn += pointsIn;
	}

	//if(totalIn == 6)
	//	return INSIDE;

	return INTERSECTPLANE;
}



 


 
