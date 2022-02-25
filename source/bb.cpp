//////////////////////////////////////////////////////////////////////////////
////																			
////  Bounding Volumes
////  
////  Jeff Barnard  2004
////	
//////////////////////////////////////////////////////////////////////////////

#include "bb.h"
using namespace Algebra;

/////////////////////////////// BoundingEllipsoid ////////////////////////////////////
/////
/////  Bounding Ellipsoid Implementation
/////
/////////////////////////////// BoundingEllipsoid ////////////////////////////////////
void BoundingVolume::Ellipsoid::setRadius(Vector3 x, Vector3 y, Vector3 z){
	// defines points to be oriented
	xradius = x;
	yradius = y;
	zradius = z;

	// defines shape of ellipsoid
	radiusVector.x = x.x;
	radiusVector.y = y.y;
	radiusVector.z = z.z;
}

void BoundingVolume::Ellipsoid::updateRadius(void) {
	
	newXradius = 
				transformation.GetTranslatedVector3D(transformation.GetRotatedVector3D(xradius));
	newYradius = 
				transformation.GetTranslatedVector3D(transformation.GetRotatedVector3D(yradius));
	newZradius = 
				transformation.GetTranslatedVector3D(transformation.GetRotatedVector3D(zradius));
}

// translate polygon to ellipsoid space (origin transformation)
void BoundingVolume::Ellipsoid::ellipsoidSpace(Vector3 v[], int j){
	for (int i = 0; i < j; i ++){
		v[i] = transformation.GetInverseRotatedVector3D(transformation.GetInverseTranslatedVector3D(v[i]));
	}
}
void BoundingVolume::Ellipsoid::ellipsoidSpace(Vector3 &v){
	v = transformation.GetInverseRotatedVector3D(transformation.GetInverseTranslatedVector3D(v));
}

// convert it back to world space
void BoundingVolume::Ellipsoid::inverseEllipsoidSpace(Vector3 v[], int j){
	for (int i = 0; i < j; i ++){
		v[i] = transformation.GetTranslatedVector3D(transformation.GetRotatedVector3D(v[i]));
	}
}
void BoundingVolume::Ellipsoid::inverseEllipsoidSpace(Vector3 &v){
	v = transformation.GetTranslatedVector3D(transformation.GetRotatedVector3D(v));
}

// convert to unit sphere space
void BoundingVolume::Ellipsoid::sphereSpace(Vector3 v[], int j){
	for (int i = 0; i < j; i++)
		v[i] = v[i]/radiusVector;
}
void BoundingVolume::Ellipsoid::sphereSpace(Vector3 &v){
	v = v/radiusVector;
}
// revert from unit sphere space
void BoundingVolume::Ellipsoid::inverseSphereSpace(Vector3 v[], int j){    
	for (int i = 0; i < j; i++)
		v[i] = v[i] * radiusVector;
}
void BoundingVolume::Ellipsoid::inverseSphereSpace(Vector3 &v){    
	v = v * radiusVector;
}

void BoundingVolume::Ellipsoid::setCenter(Vector2 &v){
	center.x = v.x;
	center.y = 0.1f;
	center.z = v.y;
}

Vector3 BoundingVolume::Ellipsoid::getCenter(void){
	return center;
}

void BoundingVolume::Ellipsoid::drawEllipsoid(void){
	//glDisable(GL_LIGHTING);
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_FOG);

	//glColor3f(1.0f,1.0f,1.0f);

	// right
	glBegin(GL_LINE_LOOP);					
		glVertex3f( center.x, center.y, center.z);
		glVertex3f( newXradius.x, newXradius.y, newXradius.z);
	glEnd();	
	// down
	glBegin(GL_LINE_LOOP);					
		glVertex3f( center.x, center.y, center.z);
		glVertex3f( newZradius.x, newZradius.y, newZradius.z);
	glEnd();	
	// below
	glBegin(GL_LINE_LOOP);					
		glVertex3f( center.x, center.y, center.z);
		glVertex3f( newYradius.x, newYradius.y, newYradius.z);				
	glEnd();

	//glEnable(GL_FOG);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
}



//////////////////////////////////// OBB3D ////////////////////////////////////
/////
/////  Oriented Bounding Box Implementation	
/////
//////////////////////////////////// OBB3D ////////////////////////////////////
void BoundingVolume::OBB3D::setCenter(Vector2 &v){
	center.x = v.x;
	center.y = 0.1f;
	center.z = v.y;
}

// half size along each axis
void BoundingVolume::OBB3D::setHalfSizes(float x, float y, float z){
	this->halfSize.x = x;
	this->halfSize.y = y;
	this->halfSize.z = z;
}

void BoundingVolume::OBB3D::setAxis(Vector3 &x, Vector3 &y, Vector3 &z){
	origAxis[0] = x;
	origAxis[1] = y;
	origAxis[2] = z;
}

void BoundingVolume::OBB3D::updateAxis(void){
	xAxis = rotationMatrix.GetRotatedVector3D(this->origAxis[0]);
	yAxis = rotationMatrix.GetRotatedVector3D(this->origAxis[1]);
	zAxis = rotationMatrix.GetRotatedVector3D(this->origAxis[2]);
	rotationMatrix.TranslateVector3D(xAxis);
	rotationMatrix.TranslateVector3D(yAxis);
	rotationMatrix.TranslateVector3D(zAxis);
}

void BoundingVolume::OBB3D::setVerts(void){
	// sets the intial corner vertices
	verts[0].x = -halfSize.x;	verts[1].x = halfSize.x;
	verts[0].y = halfSize.y;	verts[1].y = halfSize.y;
	verts[0].z = -halfSize.z;	verts[1].z = -halfSize.z;

	verts[2].x = -halfSize.x;	verts[3].x = halfSize.x;
	verts[2].y = -halfSize.y;	verts[3].y = -halfSize.y;
	verts[2].z = -halfSize.z;	verts[3].z = -halfSize.z;

	verts[4].x = -halfSize.x;	verts[5].x = halfSize.x;
	verts[4].y = halfSize.y;	verts[5].y = halfSize.y;
	verts[4].z = halfSize.z;	verts[5].z = halfSize.z;

	verts[6].x = -halfSize.x;	verts[7].x = halfSize.x;
	verts[6].y = -halfSize.y;	verts[7].y = -halfSize.y;
	verts[6].z = halfSize.z;	verts[7].z = halfSize.z;
}

void BoundingVolume::OBB3D::updateVerts(void){
	for (int i = 0; i < 8; i++)
		newVerts[i] = rotationMatrix.GetTranslatedVector3D(rotationMatrix.GetRotatedVector3D(verts[i]));
}

void BoundingVolume::OBB3D::drawBox(void){
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3ub(255, 255, 255);

	glBegin(GL_LINE_LOOP);					
		glVertex3f( center.x, center.y, center.z);
		glVertex3f( xAxis.x, xAxis.y, xAxis.z);
	glEnd();	
	//down
	glBegin(GL_LINE_LOOP);					
		glVertex3f( center.x, center.y, center.z);
		glVertex3f( yAxis.x, yAxis.y, yAxis.z);
	glEnd();	
	// below
	glBegin(GL_LINE_LOOP);					
		glVertex3f( center.x, center.y, center.z);
		glVertex3f( zAxis.x, zAxis.y, zAxis.z);				
	glEnd();
	glEnable(GL_LIGHTING);
}




/////////////////////////////// calculateBoundingSphere ////////////////////////////////////
///
///	Given a 3d object, this function calculates the tightest fitting sphere
///   WARNING: NOT COMPLETE
///
/////////////////////////////// calculateBoundingSphere ////////////////////////////////////
bool BoundingVolume::calculateBoundingSphere(c3dsObject *thisObject, AABB3D &bb, BoundingSphere &sphere) {

	sphere.center.x = bb.center.x;
	sphere.center.y = bb.center.y;
	sphere.center.z = bb.center.z;

	if (bb.width > bb.length)
		sphere.radius = bb.width/2;
	else
		sphere.radius = bb.length/2;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////
////
////  Given a polygonal mesh, calculates an axis aligned bounding box which
////  minimally surrounds the object
////
//////////////////////////////////////////////////////////////////////////////////////
bool BoundingVolume::calculateAABB3D(c3dsObject *thisObject, int size, AABB3D &bb){

	bb.min.x =  9999;
	bb.max.x = -9999;
	bb.min.y =  9999;
	bb.max.y = -9999;
	bb.min.z =  9999;
	bb.max.z = -9999;
	
	for(int j = 0; j < thisObject->numOfFaces; j++) {

		for(int whichVertex = 0; whichVertex < 3; whichVertex++) {
		
			int index = thisObject->pFaces[j].vertIndex[whichVertex];
			
			if(thisObject->pVerts[index].x < bb.min.x) 
				bb.min.x = thisObject->pVerts[index].x;
			if(thisObject->pVerts[index].x > bb.max.x) 
				bb.max.x = thisObject->pVerts[index].x;

			if(thisObject->pVerts[index].y < bb.min.y) 
				bb.min.y = thisObject->pVerts[index].y;
			if(thisObject->pVerts[index].y > bb.max.y) 
				bb.max.y = thisObject->pVerts[index].y;

			if(thisObject->pVerts[index].z < bb.min.z) 
				bb.min.z = thisObject->pVerts[index].z;
			if(thisObject->pVerts[index].z > bb.max.z) 
				bb.max.z = thisObject->pVerts[index].z;
		}
	}
	
	bb.length = bb.max.x - bb.min.x;
	bb.width  = bb.max.z - bb.min.z;
	bb.height = bb.max.y - bb.min.y;

	bb.center.x = bb.min.x + bb.length/2;
	bb.center.y = bb.min.y + bb.height/2;
	bb.center.z = bb.min.z + bb.width /2;
	
	return true;
}



///////////////////////////////// calculateAABB2D ////////////////////////////////////
///
///  Given an array of 2D vertices, calculates an axis aligned bounding box which
///  minimally surrounds the object	
///
///////////////////////////////// calculateAABB2D ////////////////////////////////////
void BoundingVolume::calculateAABB2D(AABB2D &bb , Vector2 *objcoords) {

	// Initialize the bounding box's min and max values
	bb.min.x = objcoords[0].x;
	bb.max.x = objcoords[0].x;
	bb.min.y = objcoords[0].y;
	bb.max.y = objcoords[0].y;
	
	for(int i = 0; i < 4; i++) {

		if(objcoords[i].x < bb.min.x) 
			bb.min.x = objcoords[i].x;
		if(objcoords[i].x > bb.max.x) 
			bb.max.x = objcoords[i].x;

		if(objcoords[i].y < bb.min.y) 
			bb.min.y = objcoords[i].y;
		if(objcoords[i].y > bb.max.y) 
			bb.max.y = objcoords[i].y;
    }

    // Increase the bounding box so we can see it when we render the box.  This is just for
    // display and has nothing to do with bounding boxes per say.
    bb.max.x += 0.08f;  bb.max.y += 0.08f;
    bb.min.x -= 0.08f;  bb.min.y -= 0.08f;
}



///////////////////////////////// drawAABB3D ////////////////////////////////////
///
///	Simply draws the bounding box to the screen, immediate mode which is slow
///
///////////////////////////////// drawAABB3D ////////////////////////////////////
void BoundingVolume::drawAABB3D(const AABB3D bb) {
 
	//glPushAttrib(GL_COLOR_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glColor3ub(255, 255, 255);

	glBegin(GL_LINE_LOOP);

		glColor3f(1.0f,1.0f,1.0f);	
		glVertex3f( bb.max.x, bb.max.y, bb.min.z); //top
		glVertex3f( bb.min.x, bb.max.y, bb.min.z);
		glVertex3f( bb.min.x, bb.max.y, bb.max.z);
		glVertex3f( bb.max.x, bb.max.y, bb.max.z);

		glVertex3f( bb.max.x, bb.min.y, bb.max.z); //bottom
		glVertex3f( bb.min.x, bb.min.y, bb.max.z);
		glVertex3f( bb.min.x, bb.min.y, bb.min.z);
		glVertex3f( bb.max.x, bb.min.y, bb.min.z);

		glVertex3f( bb.max.x, bb.max.y, bb.max.z); //front
		glVertex3f( bb.min.x, bb.max.y, bb.max.z);
		glVertex3f( bb.min.x, bb.min.y, bb.max.z);
		glVertex3f( bb.max.x, bb.min.y, bb.max.z);

		glVertex3f( bb.max.x, bb.min.y, bb.min.z); //back
		glVertex3f( bb.min.x, bb.min.y, bb.min.z);
		glVertex3f( bb.min.x, bb.max.y, bb.min.z);
		glVertex3f( bb.max.x, bb.max.y, bb.min.z);

		glVertex3f( bb.min.x, bb.max.y, bb.max.z); //left
		glVertex3f( bb.min.x, bb.max.y, bb.min.z);
		glVertex3f( bb.min.x, bb.min.y, bb.min.z);
		glVertex3f( bb.min.x, bb.min.y, bb.max.z);
  
		glVertex3f( bb.max.x, bb.max.y, bb.min.z); //right
		glVertex3f( bb.max.x, bb.max.y, bb.max.z);
		glVertex3f( bb.max.x, bb.min.y, bb.max.z);
		glVertex3f( bb.max.x, bb.min.y, bb.min.z);
	glEnd();	

	//glPopAttrib();
	glEnable(GL_LIGHTING);
 	
}
