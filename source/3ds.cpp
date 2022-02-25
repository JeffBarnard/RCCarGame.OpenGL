///////////////////////////////////////////////////////////////////////////////////////////
////
////   A Series of classes/structures to load and store a .3ds file
////
////   Coded by Ben Humphrey, used with permission.
////
///////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////
//
//  Information on the .3ds file format can be found in the 3DS document located
//  in this programs source directory.  Here is a quick overview.
//
//      MAIN3DS  (0x4D4D)
//     |
//     +--EDIT3DS  (0x3D3D)
//     |  |
//     |  +--EDIT_MATERIAL (0xAFFF)
//     |  |  |
//     |  |  +--MAT_NAME01 (0xA000) (See mli Doc) 
//     |  |
//     |  +--EDIT_CONFIG1  (0x0100)
//     |  +--EDIT_CONFIG2  (0x3E3D) 
//     |  +--EDIT_VIEW_P1  (0x7012)
//     |  |  |
//     |  |  +--TOP            (0x0001)
//     |  |  +--BOTTOM         (0x0002)
//     |  |  +--LEFT           (0x0003)
//     |  |  +--RIGHT          (0x0004)
//     |  |  +--FRONT          (0x0005) 
//     |  |  +--BACK           (0x0006)
//     |  |  +--USER           (0x0007)
//     |  |  +--CAMERA         (0xFFFF)
//     |  |  +--LIGHT          (0x0009)
//     |  |  +--DISABLED       (0x0010)  
//     |  |  +--BOGUS          (0x0011)
//     |  |
//     |  +--EDIT_VIEW_P2  (0x7011)
//     |  |  |
//     |  |  +--TOP            (0x0001)
//     |  |  +--BOTTOM         (0x0002)
//     |  |  +--LEFT           (0x0003)
//     |  |  +--RIGHT          (0x0004)
//     |  |  +--FRONT          (0x0005) 
//     |  |  +--BACK           (0x0006)
//     |  |  +--USER           (0x0007)
//     |  |  +--CAMERA         (0xFFFF)
//     |  |  +--LIGHT          (0x0009)
//     |  |  +--DISABLED       (0x0010)  
//     |  |  +--BOGUS          (0x0011)
//     |  |
//     |  +--EDIT_VIEW_P3  (0x7020)
//     |  +--EDIT_VIEW1    (0x7001) 
//     |  +--EDIT_BACKGR   (0x1200) 
//     |  +--EDIT_AMBIENT  (0x2100)
//     |  +--EDIT_OBJECT   (0x4000)
//     |  |  |
//     |  |  +--OBJ_TRIMESH   (0x4100)      
//     |  |  |  |
//     |  |  |  +--TRI_VERTEXL          (0x4110) 
//     |  |  |  +--TRI_VERTEXOPTIONS    (0x4111)
//     |  |  |  +--TRI_MAPPINGCOORS     (0x4140) 
//     |  |  |  +--TRI_MAPPINGSTANDARD  (0x4170)
//     |  |  |  +--TRI_FACEL1           (0x4120)
//     |  |  |  |  |
//     |  |  |  |  +--TRI_SMOOTH        (0x4150)   
//     |  |  |  |  +--TRI_MATERIAL      (0x4130)
//     |  |  |  |
//     |  |  |  +--TRI_LOCAL            (0x4160)
//     |  |  |  +--TRI_VISIBLE          (0x4165)
//     |  |  |
//     |  |  +--OBJ_LIGHT    (0x4600)
//     |  |  |  |
//     |  |  |  +--LIT_OFF              (0x4620)
//     |  |  |  +--LIT_SPOT             (0x4610) 
//     |  |  |  +--LIT_UNKNWN01         (0x465A) 
//     |  |  | 
//     |  |  +--OBJ_CAMERA   (0x4700)
//     |  |  |  |
//     |  |  |  +--CAM_UNKNWN01         (0x4710)
//     |  |  |  +--CAM_UNKNWN02         (0x4720)  
//     |  |  |
//     |  |  +--OBJ_UNKNWN01 (0x4710)
//     |  |  +--OBJ_UNKNWN02 (0x4720)
//     |  |
//     |  +--EDIT_UNKNW01  (0x1100)
//     |  +--EDIT_UNKNW02  (0x1201) 
//     |  +--EDIT_UNKNW03  (0x1300)
//     |  +--EDIT_UNKNW04  (0x1400)
//     |  +--EDIT_UNKNW05  (0x1420)
//     |  +--EDIT_UNKNW06  (0x1450)
//     |  +--EDIT_UNKNW07  (0x1500)
//     |  +--EDIT_UNKNW08  (0x2200)
//     |  +--EDIT_UNKNW09  (0x2201)
//     |  +--EDIT_UNKNW10  (0x2210)
//     |  +--EDIT_UNKNW11  (0x2300)
//     |  +--EDIT_UNKNW12  (0x2302)
//     |  +--EDIT_UNKNW13  (0x2000)
//     |  +--EDIT_UNKNW14  (0xAFFF)
//     |
//     +--KEYF3DS (0xB000)
//        |
//        +--KEYF_UNKNWN01 (0xB00A)
//        +--............. (0x7001) ( viewport, same as editor )
//        +--KEYF_FRAMES   (0xB008)
//        +--KEYF_UNKNWN02 (0xB009)
//        +--KEYF_OBJDES   (0xB002)
//           |
//           +--KEYF_OBJHIERARCH  (0xB010)
//           +--KEYF_OBJDUMMYNAME (0xB011)
//           +--KEYF_OBJUNKNWN01  (0xB013)
//           +--KEYF_OBJUNKNWN02  (0xB014)
//           +--KEYF_OBJUNKNWN03  (0xB015)  
//           +--KEYF_OBJPIVOT     (0xB020)  
//           +--KEYF_OBJUNKNWN04  (0xB021)  
//           +--KEYF_OBJUNKNWN05  (0xB022)  
//
///////////////////////////////////////////////////////

#include "3ds.h"


///////////////////////////////// Load3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This constructor initializes the tChunk data
/////
///////////////////////////////// Load3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
Load3DS::Load3DS() {

	m_CurrentChunk = new c3dsChunk;				// Initialize and allocate our current chunk
	m_TempChunk = new c3dsChunk;					// Initialize and allocate a temporary chunk
}


///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This is called by the client to open the .3ds file, read it, then clean up
/////
///////////////////////////////// IMPORT 3DS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool Load3DS::Import3DS(c3dsModel *pModel, char *strFileName) {

	char strMessage[255] = {0};

	m_FilePointer = fopen(strFileName, "rb");

	if(!m_FilePointer)  {
		sprintf(strMessage, "Unable to find the file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// If it is a 3DS file, then the first chunk ID will be equal to PRIMARY (some hex num)
	ReadChunk(m_CurrentChunk);

	// Make sure this is a 3DS file
	if (m_CurrentChunk->ID != PRIMARY) {
	
		sprintf(strMessage, "Unable to load PRIMARY chuck from file: %s!", strFileName);
		MessageBox(NULL, strMessage, "Error", MB_OK);
		return false;
	}

	// Now we actually start reading in the data.  ProcessNextChunk() is recursive
	ProcessNextChunk(pModel, m_CurrentChunk);

	ComputeNormals(pModel);

	CleanUp();

	return true;
}

///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function cleans up our allocated memory and closes the file
/////
///////////////////////////////// CLEAN UP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::CleanUp() {
	fclose(m_FilePointer);						// Close the current file pointer
	delete m_CurrentChunk;						// Free the current chunk
	delete m_TempChunk;							// Free our temporary chunk
}


///////////////////////////////// PROCESS NEXT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function reads the main sections of the .3DS file, then dives deeper with recursion
/////
///////////////////////////////// PROCESS NEXT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::ProcessNextChunk(c3dsModel *pModel, c3dsChunk *pPreviousChunk) {

	c3dsObject newObject = {0};					// This is used to add to our object list
	c3dsMaterialInfo newTexture = {0};				// This is used to add to our material list
	unsigned int version = 0;					// This will hold the file version
	int buffer[50000] = {0};					// This is used to read past unwanted data

	m_CurrentChunk = new c3dsChunk;				// Allocate a new chunk				

	
	// Continue to read the sub chunks until we have reached the length.
	// After we read ANYTHING we add the bytes read to the chunk and then check against length.
	while (pPreviousChunk->bytesRead < pPreviousChunk->length) {
		// Read next Chunk
		ReadChunk(m_CurrentChunk);

		// Check the chunk ID
		switch (m_CurrentChunk->ID) {
		
		case VERSION:							// This holds the version of the file
			
			// This chunk has an unsigned short that holds the file version.
			// Since there might be new additions to the 3DS file format in 4.0,
			// we give a warning to that problem.

			// Read the file version and add the bytes read to our bytesRead variable
			m_CurrentChunk->bytesRead += (unsigned)fread(&version, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);

			// If the file version is over 3, give a warning that there could be a problem
			if (version > 0x03)
				MessageBox(NULL, "This 3DS file is over version 3 so it may load incorrectly", "Warning", MB_OK);
			break;

		case OBJECTINFO:						// This holds the version of the mesh
			
			// This chunk holds the version of the mesh.  It is also the head of the MATERIAL
			// and OBJECT chunks.  From here on we start reading in the material and object info.

			// Read the next chunk
			ReadChunk(m_TempChunk);

			// Get the version of the mesh
			m_TempChunk->bytesRead += (unsigned)fread(&version, 1, m_TempChunk->length - m_TempChunk->bytesRead, m_FilePointer);

			// Increase the bytesRead by the bytes read from the last chunk
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;

			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT.
			ProcessNextChunk(pModel, m_CurrentChunk);
			break;

		case MATERIAL:							// This holds the material information

			// This chunk is the header for the material info chunks

			// Increase the number of materials
			pModel->numOfMaterials++;

			// Add a empty texture structure to our texture list.
			pModel->pMaterials.push_back(newTexture);

			// Proceed to the material loading function
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case OBJECT:							// This holds the name of the object being read
				
			// This chunk is the header for the object info chunks.  It also
			// holds the name of the object.

			// Increase the object count
			pModel->numOfObjects++;
		
			// Add a new tObject node to our list of objects (like a link list)
			pModel->pObject.push_back(newObject);
			
			// Initialize the object and all it's data members
			memset(&(pModel->pObject[pModel->numOfObjects - 1]), 0, sizeof(c3dsObject));

			// Get the name of the object and store it, then add the read bytes to our byte counter.
			m_CurrentChunk->bytesRead += GetString(pModel->pObject[pModel->numOfObjects - 1].strName);
			
			// Now proceed to read in the rest of the object information
			ProcessNextObjectChunk(pModel, &(pModel->pObject[pModel->numOfObjects - 1]), m_CurrentChunk);
			break;

		case EDITKEYFRAME:
			//ProcessNextKeyFrameChunk(pModel, m_CurrentChunk);
			// Read past this chunk and add the bytes read to the byte counter
			m_CurrentChunk->bytesRead += (unsigned)fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		default: 
			
			// read past the unknown or ignored chunk and add the bytes read to the byte counter.
			m_CurrentChunk->bytesRead += (unsigned)fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function handles all the information about the objects in the file
/////
///////////////////////////////// PROCESS NEXT OBJECT CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Load3DS::ProcessNextObjectChunk(c3dsModel *pModel, c3dsObject *pObject, c3dsChunk *pPreviousChunk) {

	int buffer[50000] = {0};					// This is used to read past unwanted data

	// Allocate a new chunk to work with
	m_CurrentChunk = new c3dsChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		
		ReadChunk(m_CurrentChunk);

		switch (m_CurrentChunk->ID) {
		
		case OBJECT_MESH:					// This lets us know that we are reading a new object
		
			// We found a new object, so let's read in it's info using recursion
			ProcessNextObjectChunk(pModel, pObject, m_CurrentChunk);
			break;

		case OBJECT_VERTICES:				// This is the objects vertices
			ReadVertices(pObject, m_CurrentChunk);
			break;

		case OBJECT_FACES:					// This is the objects face information
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;

		case OBJECT_MATERIAL:				// This holds the material name that the object has
			
			// the faces that the texture is assigned to (In the case that there is multiple
			// textures assigned to one object, or it just has a texture on a part of the object.
			// Since most of my game objects just have the texture around the whole object, and 
			// they aren't multitextured, I just want the material name.

			ReadObjectMaterial(pModel, pObject, m_CurrentChunk);			
			break;

		case OBJECT_UV:						// This holds the UV texture coordinates for the object

			// This chunk holds all of the UV coordinates for our object.  Let's read them in.
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;

		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += (unsigned)fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function handles all the information about the material (Texture)
/////
///////////////////////////////// PROCESS NEXT MATERIAL CHUNK \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Load3DS::ProcessNextMaterialChunk(c3dsModel *pModel, c3dsChunk *pPreviousChunk) {

	int buffer[50000] = {0};					// This is used to read past unwanted data

	// Allocate a new chunk to work with
	m_CurrentChunk = new c3dsChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read in
		switch (m_CurrentChunk->ID)
		{
		case MATNAME:							// This chunk holds the name of the material
			
			// Here we read in the material name
			m_CurrentChunk->bytesRead += (unsigned)fread(pModel->pMaterials[pModel->numOfMaterials - 1].strName, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;

		case MATDIFFUSE:						// This holds the R G B color of our object
			ReadColorChunk(&(pModel->pMaterials[pModel->numOfMaterials - 1]), m_CurrentChunk);
			break;
		
		case MATMAP:							// This is the header for the texture info
			
			// Proceed to read in the material information
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case MATMAPFILE:						// This stores the file name of the material

			// Here we read in the material's file name
			m_CurrentChunk->bytesRead += (unsigned)fread(pModel->pMaterials[pModel->numOfMaterials - 1].strFile, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		
		default:  

			// Read past the ignored or unknown chunks
			m_CurrentChunk->bytesRead += (unsigned)fread(buffer, 1, m_CurrentChunk->length - m_CurrentChunk->bytesRead, m_FilePointer);
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}



///////////////////////////////// ReadChunk \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function reads in a chunk ID and it's length in bytes
/////
///////////////////////////////// ReadChunk \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::ReadChunk(c3dsChunk *pChunk) {
	// This reads the chunk ID which is 2 bytes.
	// The chunk ID is like OBJECT or MATERIAL.  
	pChunk->bytesRead = (unsigned)fread(&pChunk->ID, 1, 2, m_FilePointer);

	// Then, we read the length of the chunk which is 4 bytes.
	// This is how we know how much to read in, or read past.
	pChunk->bytesRead += (unsigned)fread(&pChunk->length, 1, 4, m_FilePointer);
}



///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
/////
/////	This function reads in a string of characters
/////
///////////////////////////////// GET STRING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
int Load3DS::GetString(char *pBuffer) {

	int index = 0;

	// Read 1 byte of data which is the first letter of the string
	fread(pBuffer, 1, 1, m_FilePointer);

	// Loop until we get NULL
	while (*(pBuffer + index++) != 0) {

		// Read in a character at a time until we hit NULL.
		fread(pBuffer + index, 1, 1, m_FilePointer);
	}

	return (int)strlen(pBuffer) + 1;
}


///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the RGB color data
/////
///////////////////////////////// READ COLOR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Load3DS::ReadColorChunk(c3dsMaterialInfo *pMaterial, c3dsChunk *pChunk) {
	
	ReadChunk(m_TempChunk);

	// Read in the R G B color (3 bytes - 0 through 255)
	m_TempChunk->bytesRead += (unsigned)fread(pMaterial->color, 1, m_TempChunk->length -
												m_TempChunk->bytesRead, m_FilePointer);

	// Add the bytes read to our chunk
	pChunk->bytesRead += m_TempChunk->bytesRead;
}



///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the indices for the vertex array
/////
///////////////////////////////// READ VERTEX INDECES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::ReadVertexIndices(c3dsObject *pObject, c3dsChunk *pPreviousChunk) {

	unsigned short index = 0;					// This is used to read in the current face index

	// Read in the number of faces that are in this object (int)
	pPreviousChunk->bytesRead += (unsigned)fread(&pObject->numOfFaces, 1, 2, m_FilePointer);

	// Alloc enough memory for the faces and initialize the structure
	pObject->pFaces = new c3dsFace [pObject->numOfFaces];
	memset(pObject->pFaces, 0, sizeof(c3dsFace) * pObject->numOfFaces);

	// Go through all of the faces in this object
	for(int i = 0; i < pObject->numOfFaces; i++) {
		// Next, we read in the A then B then C index for the face, but ignore the 4th value.
		// The fourth value is a visibility flag for 3D Studio Max
		for(int j = 0; j < 4; j++) {
		
			// Read the first vertice index for the current face 
			pPreviousChunk->bytesRead += (unsigned)fread(&index, 1, sizeof(index), m_FilePointer);
			
			if(j < 3) // Store the index in our face structure.
				pObject->pFaces[i].vertIndex[j] = index;
			
		}
	}
}


///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the UV coordinates for the object
/////
///////////////////////////////// READ UV COORDINATES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Load3DS::ReadUVCoordinates(c3dsObject *pObject, c3dsChunk *pPreviousChunk) {
	// Read in the number of UV coordinates there are (int)
	pPreviousChunk->bytesRead += (unsigned)fread(&pObject->numTexVertex, 1, 2, m_FilePointer);

	// Allocate memory to hold the UV coordinates
	pObject->pTexVerts = new Vector2 [pObject->numTexVertex];

	// Read in the texture coodinates (an array 2 float)
	pPreviousChunk->bytesRead += (unsigned)fread(pObject->pTexVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}


///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the vertices for the object
/////
///////////////////////////////// READ VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::ReadVertices(c3dsObject *pObject, c3dsChunk *pPreviousChunk) {

	// Like most chunks, before we read in the actual vertices, we need
	// to find out how many there are to read in.  Once we have that number
	// we then fread() them into our vertice array.

	// Read in the number of vertices (int)
	pPreviousChunk->bytesRead += (unsigned)fread(&(pObject->numOfVerts), 1, 2, m_FilePointer);

	// Allocate the memory for the verts and initialize the structure
	pObject->pVerts = new Vector3 [pObject->numOfVerts];
	memset(pObject->pVerts, 0, sizeof(Vector3) * pObject->numOfVerts);

	// Read in the array of vertices (an array of 3 floats)
	pPreviousChunk->bytesRead += (unsigned)fread(pObject->pVerts, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);

	// Now we should have all of the vertices read in.  Because 3D Studio Max
	// Models with the Z-Axis pointing up, we need
	// to flip the y values with the z values in our vertices.  That way it
	// will be normal, with Y pointing up.  If you prefer to work with Z pointing
	// up, then just delete this next loop.  Also, because we swap the Y and Z
	// we need to negate the Z to make it come out correctly.

	// Go through all of the vertices that we just read and swap the Y and Z values
	for(int i = 0; i < pObject->numOfVerts; i++) {
		
		float fTempY = pObject->pVerts[i].y;

		// Set the Y value to the Z value
		pObject->pVerts[i].y = pObject->pVerts[i].z;

		// Set the Z value to the Y value, 
		pObject->pVerts[i].z = -fTempY;
	}
}


///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function reads in the material name assigned to the object and sets the materialID
/////
///////////////////////////////// READ OBJECT MATERIAL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::ReadObjectMaterial(c3dsModel *pModel, c3dsObject *pObject, c3dsChunk *pPreviousChunk) {

	char strMaterial[255] = {0};
	int buffer[50000] = {0};				// This is used to read past unwanted data

	// strMaterial should now have a string of the material name, like "Material #2" etc..
	pPreviousChunk->bytesRead += GetString(strMaterial);

	// Now that we have a material name, we need to go through all of the materials
	// and check the name against each material.  When we find a material in our material
	// list that matches this name we just read in, then we assign the materialID
	// of the object to that material index.  

	// Go through all of the textures
	for(int i = 0; i < pModel->numOfMaterials; i++){
		// If the material we just read in matches the current texture name
		if(strcmp(strMaterial, pModel->pMaterials[i].strName) == 0){
	
			// Set the material ID to the current index 'i' and stop checking
			pObject->materialID = i;

			// If the strFile has a string length of 1 and over it's a texture
			if(strlen(pModel->pMaterials[i].strFile) > 0) {

				// Set the object's flag to say it has a texture map to bind.
				pObject->bHasTexture = true;
			}	
			break;
		}
		else{
			// Set the ID to -1 to show there is no material for this object
			pObject->materialID = -1;
		}
	}

	// Read past the rest of the chunk since we don't care about shared vertices
	// You will notice we subtract the bytes already read in this chunk from the total length.
	pPreviousChunk->bytesRead += (unsigned)fread(buffer, 1, pPreviousChunk->length - pPreviousChunk->bytesRead, m_FilePointer);
}			



///////////////////////////////// ComputeNormals \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function computes the normals and vertex normals of the objects
/////
///////////////////////////////// ComputeNormals \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Load3DS::ComputeNormals(c3dsModel *pModel) {

	Vector3 vVector1, vVector2, vNormal, vPoly[3];
	
	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

	// Go through each of the objects to calculate their normals
	for(int index = 0; index < pModel->numOfObjects; index++) {

		// Get the current object
		c3dsObject *pObject = &(pModel->pObject[index]);

		// Here we allocate all the memory we need to calculate the normals
		Vector3 *pNormals		= new Vector3 [pObject->numOfFaces];
		Vector3 *pTempNormals	= new Vector3 [pObject->numOfFaces];
		pObject->pNormals		= new Vector3 [pObject->numOfVerts];
		//pObject->pVerts			= new Vector3 [pObject->numOfVerts];

		// Go though all of the faces of this object
		for(int i=0; i < pObject->numOfFaces; i++) {
													
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)
			vVector1 = Algebra::calculateVector(vPoly[0], vPoly[2]);		// Get the Vector3 of the polygon (we just need 2 sides for the normal)
			vVector2 = Algebra::calculateVector(vPoly[2], vPoly[1]);		// Get a second Vector3 of the polygon

			vNormal  = Algebra::cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize Vector3, but not a unit Vector3)
			pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
			vNormal  = Algebra::normalize3(vNormal);				// Normalize the cross product to give us the polygons normal

			pNormals[i] = vNormal;						// Assign the normal to the list of normals
		}

		//////////////// Get The Vertex Normals /////////////////

		Vector3 vSum(0.0, 0.0, 0.0);
		Vector3 vZero = vSum;
		int shared=0;

		for (i = 0; i < pObject->numOfVerts; i++) {			// Go through all of the vertices
		
			for (int j = 0; j < pObject->numOfFaces; j++) {	// Go through all of the triangles
															// Check if the vertex is shared by another face
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i) {
				
					vSum = Algebra::addVector(vSum, pTempNormals[j]);// Add the un-normalized normal of the shared face
					shared++;								// Increase the number of shared triangles
				}
			}      
			
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			pObject->pNormals[i] = Algebra::divideVectorByScaler(vSum, float(-shared));

			// Normalize the normal for the final vertex normal
			pObject->pNormals[i] = Algebra::normalize3(pObject->pNormals[i]);	

			vSum = vZero;
			shared = 0;	
		}
	
		delete [] pTempNormals;
		delete [] pNormals;
	}
}

