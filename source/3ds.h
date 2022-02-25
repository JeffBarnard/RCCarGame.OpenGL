///////////////////////////////////////////////////////////////////////////////////////////
////
////   A Series of classes/structures to load and store a .3ds file
////
////   Coded by Ben Humphrey, used with permission.
////
///////////////////////////////////////////////////////////////////////////////////////////

#ifndef _3DS_H
#define _3DS_H

#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <Vector>
#include "algebra.h"
using namespace std;

//------ Primary Chunk, at the beginning of each file
#define PRIMARY       0x4D4D

//------ Main Chunks
#define OBJECTINFO    0x3D3D				// This gives the version of the mesh and is found right before the material and object information
#define VERSION       0x0002				// This gives the version of the .3ds file
#define EDITKEYFRAME  0xB000				// This is the header for all of the key frame info

//------ sub defines of OBJECTINFO
#define MATERIAL	  0xAFFF				// This stored the texture info
#define OBJECT		  0x4000				// This stores the faces, vertices, etc...

//------ sub defines of MATERIAL
#define MATNAME       0xA000				// This holds the material name
#define MATDIFFUSE    0xA020				// This holds the color of the object/material
#define MATMAP        0xA200				// This is a header for a new material
#define MATMAPFILE    0xA300				// This holds the file name of the texture

#define OBJECT_MESH   0x4100				// This lets us know that we are reading a new object

//------ sub defines of OBJECT_MESH
#define OBJECT_VERTICES     0x4110			// The objects vertices
#define OBJECT_FACES		0x4120			// The objects faces
#define OBJECT_MATERIAL		0x4130			// This is found if the object has a material, either texture map or color
#define OBJECT_UV			0x4140			// The UV texture coordinates


#define MAX_TEXTURES 200					// The maximum amount of textures to load


// This is our face structure.  This is is used for indexing into the vertex 
// and texture coordinate arrays.  From this information we know which vertices
// from our vertex array go to which face, along with the correct texture coordinates.

namespace c3ds{

struct c3dsFace {

	int vertIndex[3];			// indicies for the verts that make up this triangle
	int coordIndex[3];			// indicies for the tex coords to texture this face
};

// This holds the information for a material.  It may be a texture map of a color.
struct c3dsMaterialInfo {

	char  strName[255];			// The texture name
	char  strFile[255];			// The texture file name (If this is set it's a texture map)
	BYTE  color[3];				// The color of the object (R, G, B)
	int   texureId;				// the texture ID (assigned by openGL?)
	float uTile;				// u tiling of texture  (Currently not used)
	float vTile;				// v tiling of texture	(Currently not used)
	float uOffset;			    // u offset of texture	(Currently not used)
	float vOffset;				// v offset of texture	(Currently not used)
} ;

// This holds all the information for the model/scene. 
// Eventually this should turn into a class that 
struct c3dsObject {

	int  numOfVerts;			// The number of verts in the model
	int  numOfFaces;			// The number of faces in the model
	int  numTexVertex;			// The number of texture coordinates
	int  materialID;			// The texture ID to use, which is the index into our texture array
	bool bHasTexture;			// This is TRUE if there is a texture map for this object
	char strName[255];			// The name of the object

	Vector3  *pVerts;			// The object's vertices
	Vector3  *pNormals;			// The object's normals
	Vector2  *pTexVerts;		// The texture's UV coordinates
	c3dsFace *pFaces;			// The faces information of the object
};

// This holds the model information.
struct c3dsModel {

	int numOfObjects;					 // The number of objects in the model
	int numOfMaterials;					 // The number of materials for the model
	vector<c3dsMaterialInfo> pMaterials; // The list of material information (Textures and colors)
	vector<c3dsObject> pObject;			 // The object list for our model

	void clear(void){
		numOfObjects = 0;
		numOfMaterials = 0;
		pMaterials.clear();
		pObject.clear();
		//pObject.resize(0);
	}
};


// Here is a structure for the 3DS indicies (since .3DS stores 4 unsigned shorts)
struct c3dsIndices {							
	unsigned short a, b, c, bVisible;		// This will hold point1, 2, and 3 index's into the vertex array plus a visible flag
};

// This holds the chunk info
struct c3dsChunk {
	unsigned short int ID;					// The chunk's ID		
	unsigned int length;					// The length of the chunk
	unsigned int bytesRead;					// The amount of bytes read within that chunk
};

/////////////////////////////////////////////////////////////////////////////////////////////////
////
////   This class handles all of the loading code
////
/////////////////////////////////////////////////////////////////////////////////////////////////
class Load3DS {

public:
	Load3DS();

	bool Import3DS(c3dsModel *pModel, char *strFileName);


private:
	// This reads in a string and saves it in the char array passed in
	int GetString(char *);

	// This reads the next chunk
	void ReadChunk(c3dsChunk *);

	// This reads the next large chunk
	void ProcessNextChunk(c3dsModel *pModel, c3dsChunk *);

	// This reads the object chunks
	void ProcessNextObjectChunk(c3dsModel *pModel, c3dsObject *pObject, c3dsChunk *);

	// This reads the material chunks
	void ProcessNextMaterialChunk(c3dsModel *pModel, c3dsChunk *);

	// This reads the RGB value for the object's color
	void ReadColorChunk(c3dsMaterialInfo *pMaterial, c3dsChunk *pChunk);

	// This reads the objects vertices
	void ReadVertices(c3dsObject *pObject, c3dsChunk *);

	// This reads the objects face information
	void ReadVertexIndices(c3dsObject *pObject, c3dsChunk *);

	// This reads the texture coodinates of the object
	void ReadUVCoordinates(c3dsObject *pObject, c3dsChunk *);

	// This reads in the material name assigned to the object and sets the materialID
	void ReadObjectMaterial(c3dsModel *pModel, c3dsObject *pObject, c3dsChunk *pPreviousChunk);
	
	// This computes the vertex normals for the object (used for lighting)
	void ComputeNormals(c3dsModel *pModel);

	void CleanUp();
	
	FILE *m_FilePointer;
	
	// These are used through the loading process to hold the chunk information
	c3dsChunk *m_CurrentChunk;
	c3dsChunk *m_TempChunk;
};


}
using namespace c3ds;


#endif // _3DS_H