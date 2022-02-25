//////////////////////////////////////////////////////////////////////////////
////																			
////  Model class - handles rendering
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
// April 14 2004
//        Checking if extensions exists, if not then disable the support.  This is for
//        compatibility on older hardware which may not support say cube mapping.
//
//

#include "model.h"
#include <string>
#include <vector>
#include <new>

using namespace Algebra;

// VBO Extension Definitions
//#define GL_ARRAY_BUFFER_ARB 0x8892
//#define GL_STATIC_DRAW_ARB 0x88E4
// VBO Extension Function Pointers
//typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
//typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
//typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
//typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, int size, const GLvoid *data, GLenum usage);

Model::Model(){
	multiTextureEnabled = false;
	reflectAmount = 0.0f;
	alphaTest = false;
	noDrawDistance = false;
}

Model::Model(char *file){
	fileName = file;
	multiTextureEnabled = false;
	reflectAmount = 0.0f;
	alphaTest = false;
	noDrawDistance = false;
}

void Model::setMeshFile(char *file){
	fileName = file;

	/*
	for(int i = 0; i < myModel.numOfObjects; i++) {
		myModel.pObject[i].pFaces = new c3dsFace();
		myModel.pObject[i].pNormals = new Vector3();
		myModel.pObject[i].pVerts = new Vector3();
		myModel.pObject[i].pTexVerts = new Vector2();
	}
	*/
}

bool Model::checkSupport(void){

	// load CVA extensions
	PFNGLLOCKARRAYSEXTPROC			 glLockArraysEXT;
	PFNGLUNLOCKARRAYSEXTPROC		 glUnlockArraysEXT;
	// load the extensions for multitexturing
	PFNGLMULTITEXCOORD2FARBPROC		 glMultiTexCoord2fARB;
	PFNGLACTIVETEXTUREARBPROC		 glActiveTextureARB;
	PFNGLCLIENTACTIVETEXTUREARBPROC	 glClientActiveTextureARB;

	glLockArraysEXT			 = (PFNGLLOCKARRAYSEXTPROC)			 wglGetProcAddress("glLockArraysEXT");
    glUnlockArraysEXT		 = (PFNGLUNLOCKARRAYSEXTPROC)		 wglGetProcAddress("glUnlockArraysEXT");
	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)		 wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC)	 wglGetProcAddress("glMultiTexCoord2fARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

	cubeMappingSupported = false;
	multitexturingSupported = false;

	// check for extensions availability
	getExtensions = glGetString( GL_EXTENSIONS );
	char extensionList[4000];

	int i = 0;
	while(getExtensions[i]){
		extensionList[i] = getExtensions[i];
		i++;
	}
	extensionList[i] = '\0';
	string temp;
	for (unsigned int i=0; i < strlen(extensionList); i++){        
		if (getExtensions[i] == ' '){
			//if (temp == "GL_ARB_texture_cube_map")
			//	cubeMappingSupported = true;			
			if (temp == "GL_ARB_multitexture")
				multitexturingSupported = true;	
			if (temp == "GL_EXT_texture_cube_map")
				cubeMappingSupported = true;
			temp.clear();
		}
		else{
			temp += (extensionList[i]);
		}		
	}


	if(!glActiveTextureARB || !glMultiTexCoord2fARB || !multitexturingSupported){
		MessageBox(NULL, "MultiTexturing Not Supported!", "Update Video Drivers", MB_OK);
		return false;
	}
	if (!glLockArraysEXT){
		MessageBox(NULL, "Compiled Vertex Arrays Not Supported!", "Update Video Drivers", MB_OK);
		return false;
	}
	if (!this->cubeMappingSupported){
		MessageBox(NULL, "Cube Mapping Not Supported, Disabling Reflections.", "Update Video Drivers", MB_OK);
		//return false;
	}

	return true;

}


////////////////////////////////// enableMultiTexture \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  Must be called before Model::load()
///  Enables the use of multitexturing, currently only one object supported!!!
///
////////////////////////////////// enableMultiTexture \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Model::enableMultiTexture(char *mt){

	multiTextureEnabled = true;
	if (mt){
		multiTexture[0] = Texture::TextureManager::getSingleton().loadTexture(mt);    
	}
}


/////////////////////////////////////// load \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This method loads the mesh into local memory, returns false if extension not supported
/////
/////////////////////////////////////// load \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Model::load(void){

	// load CVA extensions
	PFNGLLOCKARRAYSEXTPROC			 glLockArraysEXT;
	PFNGLUNLOCKARRAYSEXTPROC		 glUnlockArraysEXT;
	// load the extensions for multitexturing
	PFNGLMULTITEXCOORD2FARBPROC		 glMultiTexCoord2fARB;
	PFNGLACTIVETEXTUREARBPROC		 glActiveTextureARB;
	PFNGLCLIENTACTIVETEXTUREARBPROC	 glClientActiveTextureARB;

	glLockArraysEXT			 = (PFNGLLOCKARRAYSEXTPROC)			 wglGetProcAddress("glLockArraysEXT");
    glUnlockArraysEXT		 = (PFNGLUNLOCKARRAYSEXTPROC)		 wglGetProcAddress("glUnlockArraysEXT");
	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)		 wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC)	 wglGetProcAddress("glMultiTexCoord2fARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

		
	Load3DS modelLoad;
	myModel.clear();
	modelLoad.Import3DS(&myModel, fileName);
    
	TRACE("Loading mesh: %s\n", fileName);
    
	for(int i = 0; i < myModel.numOfMaterials; i++){
		
		if(strlen(myModel.pMaterials[i].strFile) > 0) {
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// Pass in the texture array, the name of the texture, and an ID to reference it.	
			string passValue = "../textures/track/";
			passValue.append(myModel.pMaterials[i].strFile);
			
			//Texture::createTexture(modelTextures, passValue.c_str(), i);	
			
			modelTextures[i] = 
				Texture::TextureManager::getSingleton().loadTexture(passValue.c_str(), -1, 1, NICE ,1);

			TRACE("modelTextures[%d] is %d  ", i, modelTextures[i]);
			TRACE("Texture name is %s\n\n", passValue.c_str());
		}

		// Set the texture ID for this material
		//myModel.pMaterials[i].texureId = modelTextures[i];
		
	}
	
	objects = new modelObject[myModel.numOfObjects];
	// Create bounding volumes
	///////////////////////////////////////////////////
	for(int i = 0; i < myModel.numOfObjects; i++) {
		
		c3dsObject *thisObject = &myModel.pObject[i];
		calculateAABB3D(thisObject, 0, this->objects[i].boundingBox);
		calculateBoundingSphere(thisObject, this->objects[i].boundingBox, this->objects[i].boundingSphere);

	}
		
	//
	///////////////////////////////////////////////////
	createArrays();
	
	// compile the display lists
	///////////////////////////////////////////////////
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );

	displayList = new GLuint[myModel.numOfObjects];
	
	for (int i = 0; i < myModel.numOfObjects; i++){


		if (this->multiTextureEnabled){

			// we are doing automatic texture coordinate generation so we cannot
			// stick the GL calls into a compiled display list
            
		}else if (!this->multiTextureEnabled && !this->alphaTest){

			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glVertexPointer    (3, GL_FLOAT, 0, vaData[i].vertices);	
			glNormalPointer    (   GL_FLOAT, 0, vaData[i].normals); 
			glTexCoordPointer  (3, GL_FLOAT, 0, vaData[i].texCoords);

			displayList[i] = glGenLists(1);
			glNewList(displayList[i], 0x1300);
				
				if (!vaData[i].hasTexture){
					glDisable(GL_TEXTURE_2D);					
					// TODO: this doesnt work in RELEASE mode
					glColor3ub(vaData[i].color[0], vaData[i].color[1], vaData[i].color[2]);
				}
				else{
					glEnable(GL_TEXTURE_2D);
					glColor3ub(255, 255, 255);
					glBindTexture(GL_TEXTURE_2D, vaData[i].textureID);
					glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
				}

				glDrawArrays(GL_TRIANGLES, 0, vaData[i].numVertices / 3);
				//glDrawElements(GL_TRIANGLES, vaData[i].numVertices, GL_UNSIGNED_BYTE, vaData[i].indices);
				//glLockArraysEXT(0, vaData[i].numVertices / 3);
				//glUnlockArraysEXT();

			glEndList();
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );			
	
		}else if (!this->multiTextureEnabled && this->alphaTest){

			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			glVertexPointer    (3, GL_FLOAT, 0, vaData[i].vertices);	
			glNormalPointer    (   GL_FLOAT, 0, vaData[i].normals); 
			glTexCoordPointer  (3, GL_FLOAT, 0, vaData[i].texCoords);

			displayList[i] = glGenLists(1);
			glNewList(displayList[i], 0x1300);
				
				glEnable(GL_TEXTURE_2D);
				glColor3ub(255, 255, 255);
				glEnable(GL_ALPHA_TEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA );
				glBindTexture(GL_TEXTURE_2D, vaData[i].textureID);
				
				glDrawArrays(GL_TRIANGLES, 0, vaData[i].numVertices / 3);

				glDisable(GL_ALPHA_TEST);
				glDisable(GL_BLEND);
				glBlendFunc (GL_SRC_ALPHA, GL_ONE);

			glEndList();

			glDisableClientState( GL_TEXTURE_COORD_ARRAY );		

		}
	}

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	
	return true;
}


//////////////////////////////////// createArrays \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	
/////
//////////////////////////////////// createArrays \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Model::createArrays(void){
	int   index = 0;		// counter
	float VACount = 0;		// counter
	int   vCount = 0;		// counter
	int   indiceCount = 0;  // indice counter
	
	// generate the vertex arrays
	/////////////////////////////////////////////////////////////
	vaData = new VAdata[ myModel.numOfObjects ];
	for(int i = 0; i < myModel.numOfObjects; i++) {

		if(myModel.pObject.size() <= 0)
			return false;

		vCount = 0;
		c3dsObject *thisObject = &myModel.pObject[i];
	
		// make room
		vaData[i].numVertices = thisObject->numOfFaces * 9;
		vaData[i].vertices    = new GLfloat[vaData[i].numVertices];
		vaData[i].normals     = new GLfloat[vaData[i].numVertices];
		vaData[i].texCoords   = new GLfloat[vaData[i].numVertices];
			
		if(thisObject->bHasTexture) {
		
			if(thisObject->pTexVerts) {

				vaData[i].hasTexture = true;
				vaData[i].textureID = modelTextures[thisObject->materialID];
			
				for(int j = 0; j < thisObject->numOfFaces; j++) {

					for(int whichVertex = 0; whichVertex < 3; whichVertex++) {
					
						index = thisObject->pFaces[j].vertIndex[whichVertex];
						
						vaData[i].indices[indiceCount] = index;

						vaData[i].normals[vCount]   = thisObject->pNormals[index].x;
						vaData[i].normals[vCount+1] = thisObject->pNormals[index].y;
						vaData[i].normals[vCount+2] = thisObject->pNormals[index].z;

						vaData[i].texCoords[vCount]   = thisObject->pTexVerts[index].x;
						vaData[i].texCoords[vCount+1] = thisObject->pTexVerts[index].y;
						vaData[i].texCoords[vCount+2] = NULL;
						
						vaData[i].vertices[vCount]   = thisObject->pVerts[index].x;
						vaData[i].vertices[vCount+1] = thisObject->pVerts[index].y;
						vaData[i].vertices[vCount+2] = thisObject->pVerts[index].z;
						vCount+=3;
						indiceCount++;
					}
				}
			}
		}
		else if(myModel.pMaterials.size() && thisObject->materialID >= 0) {
			
			vaData[i].hasTexture = false;
			vaData[i].color = myModel.pMaterials[thisObject->materialID].color;
			
			for(int j = 0; j < thisObject->numOfFaces; j++){

				for(int whichVertex = 0; whichVertex < 3; whichVertex++){

					index = thisObject->pFaces[j].vertIndex[whichVertex];

					vaData[i].indices[indiceCount] = index;

					vaData[i].normals[vCount]   = thisObject->pNormals[index].x;
					vaData[i].normals[vCount+1] = thisObject->pNormals[index].y;
					vaData[i].normals[vCount+2] = thisObject->pNormals[index].z;

					vaData[i].vertices[vCount]   = thisObject->pVerts[index].x;
					vaData[i].vertices[vCount+1] = thisObject->pVerts[index].y;
					vaData[i].vertices[vCount+2] = thisObject->pVerts[index].z;

					vaData[i].texCoords[vCount]   = NULL;
					vaData[i].texCoords[vCount+1] = NULL;
					vaData[i].texCoords[vCount+2] = NULL;
					vCount += 3;
					indiceCount++;
				}
			}
		}
	}
	return true;
}

/////////////////////////////////////// render \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This method renders the model to the screen by calling the display list
/////
/////////////////////////////////////// render \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Model::render(float t,float rot, Vector2 pos) {

	// load the extensions for multitexturing
	PFNGLMULTITEXCOORD2FARBPROC		 glMultiTexCoord2fARB = NULL;
	PFNGLACTIVETEXTUREARBPROC		 glActiveTextureARB = NULL;
	PFNGLCLIENTACTIVETEXTUREARBPROC	 glClientActiveTextureARB = NULL;

	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)		 wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC)	 wglGetProcAddress("glMultiTexCoord2fARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");

	// load the extensions for VBO's
	//PFNGLGENBUFFERSARBPROC		glGenBuffersARB = NULL;		// VBO name generation procedure
	//PFNGLBINDBUFFERARBPROC		glBindBufferARB = NULL;		// VBO bind procedure
	//PFNGLBUFFERDATAARBPROC		glBufferDataARB = NULL;		// VBO data loading procedure
	//PFNGLDELETEBUFFERSARBPROC	glDeleteBuffersARB = NULL;		// VBO deletion procedure

	reflectAmount += 0.0005f;

	bool culling = false;
	if (cullingTests){
		frustrum.updateFrustrum();
	}

	for (int i = 0; i < this->myModel.numOfObjects; i++){

		if(drawBB)
			drawAABB3D(this->objects[i].boundingBox);

		if (cullingTests) {
			float distance = Algebra::magnitude((Algebra::calculateVector(Vector3(pos.x, 0.0f, pos.y), objects[i].boundingBox.center)));
			
			if (distance < 200.0f) { // check draw distance
				if (frustrum.containsAABB(objects[i].boundingBox) != Algebra::OUTSIDEPLANE) {
					
					glCallList(displayList[i]);						
					//if (!noDrawDistance){	
					//}
					//else{
					//	glCallList(displayList[i]);
					//}
				}
			}	
		}
		else{
			if (!multiTextureEnabled){
				glCallList(displayList[i]);
			}
			else if (multiTextureEnabled){
				//////////////////////////////////////////////////////
				// Render and Cube Map the car
				//////////////////////////////////////////////////////

				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableClientState( GL_NORMAL_ARRAY );

				glVertexPointer(3, GL_FLOAT,  0, vaData[i].vertices);
				glNormalPointer(GL_FLOAT,     0, vaData[i].normals); 

				glClientActiveTextureARB(GL_TEXTURE0_ARB);
				//glEnable(GL_TEXTURE_2D);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(3, GL_FLOAT, 0, vaData[i].texCoords);

				glClientActiveTextureARB(GL_TEXTURE1_ARB);
				//glEnable(GL_TEXTURE_2D);
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);
				glTexCoordPointer(3, GL_FLOAT, 0, vaData[i].texCoords);

					glPushAttrib(GL_TEXTURE_BIT); // save the previous settings
						glActiveTextureARB(GL_TEXTURE0_ARB);
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, vaData[i].textureID);
						glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
											
						glActiveTextureARB(GL_TEXTURE1_ARB);
						glColor3ub(255, 255, 255);
						
						if (cubeMappingSupported){
							glEnable(GL_TEXTURE_CUBE_MAP_EXT); 	
							glDisable(GL_TEXTURE_2D);
							
							//glMatrixMode(GL_TEXTURE);
							//	glLoadIdentity();
							//	glRotatef(-rot, 0, 0, 1);
							//glMatrixMode(GL_MODELVIEW);
							
							glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
							glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
							glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
							glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
							glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_EXT);
							glEnable(GL_TEXTURE_GEN_S);
							glEnable(GL_TEXTURE_GEN_T);
							glEnable(GL_TEXTURE_GEN_R);
						}

												
						glDrawArrays (GL_TRIANGLES, 0, vaData[i].numVertices / 3);
						
						glDisable(GL_TEXTURE_CUBE_MAP_EXT);
						glDisable(GL_TEXTURE_GEN_S);
						glDisable(GL_TEXTURE_GEN_T);
						glDisable(GL_TEXTURE_GEN_R);
		
					glPopAttrib(); // restore the previous settings

				glClientActiveTextureARB (GL_TEXTURE0_ARB);
				glDisableClientState (GL_TEXTURE_COORD_ARRAY);
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_NORMAL_ARRAY );

				glClientActiveTextureARB (GL_TEXTURE1_ARB);
				glDisableClientState (GL_TEXTURE_COORD_ARRAY);
				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_NORMAL_ARRAY );
				
			}
			else{
				// this is a shadow
				glCallList(displayList[i]);				
			}
		}
	}
	return true;
}


/////////////////////////////////////// DESTROY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	Frees memory used for storing the model.  
/////
/////////////////////////////////////// DESTROY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Model::destroy(void) {

	TRACE("\n\nDeleting Model Data: %s", this->fileName);

	for (int i=0;i<myModel.numOfObjects; i++){
		if (!this->multiTextureEnabled){
			delete[] vaData[i].vertices;
			delete[] vaData[i].texCoords;
			delete[] vaData[i].normals;
		}
	}
	delete [] vaData;

	// Go through all the objects in the model
	for(int i = 0; i < myModel.numOfObjects; i++) {
		// Free the faces, normals, vertices, and texture coordinates.
		delete [] myModel.pObject[i].pFaces;
		delete [] myModel.pObject[i].pNormals;
		delete [] myModel.pObject[i].pVerts;
		delete [] myModel.pObject[i].pTexVerts;
	}

	return true;
}

///////////////////////////////////// DESTRUCTOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	Simply calls ::destroy
/////
///////////////////////////////////// DESTRUCTOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Model::~Model(){
	
}