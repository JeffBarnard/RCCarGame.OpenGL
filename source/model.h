//////////////////////////////////////////////////////////////////////////////
////																			
////  Model class - handles loading/rendering
////
////  Jeff Barnard  2004
////
//////////////////////////////////////////////////////////////////////////////

// December 15 2003
//        I'm implementing vertex arrays (VA's), i'm going to try to compile the VA call into
//        the existing Display List code, that should be very fast if the drivers cache
//        the data in the AGP memory.
//
// January 08 2004
//		  I'm enabling multitexturing for car reflections etc, not the ground though!
//		  currently only one object per model class supported i think.
//
// February 08 2004
//		  Implemented Cube Mapping
//        Implemented Texture Managment class
//
//
// March 15 2004
//		  Experimenting with VBO's (Virtual Buffer Objects) which basically store vertex
//        data on the server (GPU) = very fast.  Basically a whole new way to transfer
//        vertex data to openGL and the GPU.
//

#ifndef _MODEL_H
#define _MODEL_H

#include "3ds.h"
#include "algebra.h"
#include "texture.h"
#include "glext.h"
#include "bb.h"
#include "frustrum.h"

#include <gl\glaux.h>	

struct modelObject{

	// TODO: everything should be grouped here, like VAdata etc.
	BoundingVolume::AABB3D boundingBox;
	BoundingVolume::BoundingSphere boundingSphere;
};

struct VAdata{
	// holds all the data for the vertex arrays
	GLfloat *vertices;
	GLfloat *normals;
	GLfloat *texCoords;
	GLubyte indices[99999];
	BYTE	*color;
	UINT    textureID;
	UINT    numVertices;
	bool    hasTexture;
};


class Model {

private:

	c3dsModel myModel; 							 // This holds the 3D Model info that is loaded in.
	modelObject *objects;
	unsigned int modelTextures[MAX_TEXTURES];	 // This holds the texture info, referenced by an ID
	
	VAdata *vaData;
  	GLuint *displayList;

	// check for extensions
	const unsigned char *getExtensions;
	bool cubeMappingSupported;
	bool multitexturingSupported;

protected:
	
	bool cullingTests;
	bool multiTextureEnabled;
	bool noDrawDistance;
	bool alphaTest;
	unsigned int multiTexture[1];
	float reflectAmount;

public:
	
	Frustrum frustrum;

	char *fileName;
      
	bool drawBB;

	modelObject * getObjects(void){
		return objects;
	}

	c3dsModel * get3DModel(void){
		return &myModel;
	}

	Model();

	Model(char*);

	bool load(void);

	bool createArrays(void);

	void enableAlphaTest(void){
		alphaTest = true;
	}
	
	void setMeshFile(char *file);

	bool checkSupport(void);

	void enableMultiTexture(char *mt);

	void enableNoDrawDistance(void){
		noDrawDistance = true;
	}

	virtual bool render(float t, float rot, Vector2 pos);

	bool destroy(void);

	~Model();

};


#endif // _MODEL_H