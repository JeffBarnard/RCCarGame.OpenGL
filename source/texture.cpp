//////////////////////////////////////////////////////////////////////////////
////
////  Texture Management				
////
////  Jeff Barnard  2004
////
////  Thanks to Christopher.Smith@Trinity.edu (www.cs.trinity.edu/~csmith8)
////  For his excellent Article on texture management posted at gamedev.net
////																			
//////////////////////////////////////////////////////////////////////////////

#include "texture.h"
using namespace Texture;

#define GL_CLAMP_TO_EDGE				    0x812F  
//#ifndef GL_EXT_texture_cube_map 
// EXT_texture_cube_map 
#define GL_EXT_texture_cube_map             1 
#define GL_NORMAL_MAP_EXT                   0x8511 
#define GL_REFLECTION_MAP_EXT               0x8512 
#define GL_TEXTURE_CUBE_MAP_EXT             0x8513 
#define GL_TEXTURE_BINDING_CUBE_MAP_EXT     0x8514 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT  0x8515 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT  0x8516 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT  0x8517 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT  0x8518 
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT  0x8519 
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT  0x851A 
#define GL_PROXY_TEXTURE_CUBE_MAP_EXT       0x851B 
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_EXT    0x851C 
//#endif

TextureManager * Texture::TextureManager::singleton = 0;

//////////////////////////// TextureManager \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
/// Initialize the Texture Manager object
///
//////////////////////////// TextureManager \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
TextureManager::TextureManager (void) {
	numTextures	   = 0;
	numAvailable   = 0;
	textIDs        = 0;
}

///////////////////////////// getSingleton \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Returns a pointer to the singleton object, every method MUST be
/// called using this, or the singleton order will be broken
///
/// If this is the first time using it, a new instance of TextureManager
/// will be created in memory.
///
///////////////////////////// getSingleton \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
TextureManager &TextureManager::getSingleton (void) {
	if (!singleton) {
		singleton = new TextureManager();
		initialize ();
	}

	return *singleton;
}

///////////////////////////// initialize \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Initialize the object, start with a predefined amount of space
///
///////////////////////////// initialize \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void TextureManager::initialize (void) {
	singleton->numTextures	 = 0;
	singleton->numAvailable	 = NUMTEXTURES;
	singleton->textIDs		 = new unsigned int [NUMTEXTURES];
	
	for (int i = 0; i < singleton->numAvailable; i++) {
		singleton->textIDs[i] = -1;
	}
}

//////////////////////////////// destroy \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Destroys the entire TextureManager.  Called on program exit
///
//////////////////////////////// destroy \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void TextureManager::destroy (void) {
	if (singleton) {
		delete [] singleton->textIDs;
		singleton->textIDs = 0;
	
		delete singleton;
		singleton = 0;
	}
}

//////////////////////////// getNewTextureID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Generates a new texture storage ID, if one has been specified this
/// will try to grant that request, otherwise openGL will assign an ID
///
//////////////////////////// getNewTextureID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
unsigned int TextureManager::getNewTextureID (int possibleTextureID) {

	// if the user has requested a specific texture ID
	//////////////////////////////////////////////////////////
	if (possibleTextureID != -1) {
		for (int i = 0; i < singleton->numAvailable; i++) {
			if (singleton->textIDs[i] == possibleTextureID) {
				freeTexture (possibleTextureID);
				singleton->numTextures--;	// since we will add the ID again
				break;
			}
		}
	}

	// Look for a new textureID to be used
	//////////////////////////////////////////////////////////
	unsigned int newTextureID;
	if (possibleTextureID == -1) {
		// allow openGL to get an unused textureID
		unsigned int GLID;	
		glGenTextures (1, &GLID);
		newTextureID = GLID;
	}
	else	// if the user has requested a specificID, give it to them
		newTextureID = (unsigned int)possibleTextureID;
	
	
	// find an empty slot in the TexID array
	//////////////////////////////////////////////////////////
	int index = 0;
	while (singleton->textIDs[index] != -1 && index < singleton->numAvailable)
		index++;

	// make more room in TextureManager if needed
	//////////////////////////////////////////////////////////
	if (index >= singleton->numAvailable) {

		unsigned int *newIDs = new unsigned int [singleton->numAvailable + TEXTURE_STEP];
		int i;
		
		// copy the old array
		for (i = 0; i < singleton->numAvailable; i++)
			newIDs [i] = singleton->textIDs [i];
		
		// set the last increment to the newest ID
		newIDs [singleton->numAvailable] = newTextureID;
		
		// set the new ones to -1
		for (i = 1; i < TEXTURE_STEP; i++)
			newIDs [i + singleton->numAvailable] = -1;

		singleton->numAvailable += TEXTURE_STEP;
		delete [] singleton->textIDs;
		singleton->textIDs = newIDs;
	}
	else{
		// since there is room, assign the new textureID to the TextureManager ID list
		singleton->textIDs[index] = newTextureID;
	}

	singleton->numTextures++;
	return newTextureID;
}


//////////////////////////////// loadTexture \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///  Loads a texture (BMP or TGA) from file.
///  Returns a reference number for that texture.
///
///  Parameter Reference:
///	 - textureID should be left at -1 to let openGL decide the texID
///  - mip is mipmapping, 1 is true, 0 is false
///  - quality is one of NICEST, NICE, or POOR
///  - repeat, 1 for texture repeating, 0 for clamping
///
//////////////////////////////// loadTexture \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
unsigned int TextureManager::loadTexture (const char *filename, int textureID, int mip, int quality, int repeat) {

	int imageWidth = 0, imageHeight = 0, imageBPP = 0;
	unsigned char *pData = 0;
	TGATexture tex;
			
	//////////////////////////////////////////////////////////////////
	// Determine the type and actually load the file
	//////////////////////////////////////////////////////////////////
	char capFilename [80];
	int len = (int)(strlen (filename));
	for (int c = 0; c <= len; c++)	// <= to include the NULL as well
		capFilename[c] = toupper(filename [c]);
	
	if (strcmp(capFilename + (len - 3), "BMP") == 0 ||
		strcmp(capFilename + (len - 3), "TGA") == 0) {
		
		// Do the BMP thing
		////////////////////////////////////
		if (strcmp (capFilename + (len - 3), "BMP") == 0) {

			TRACE("\nLoading BMP texture from file\n");

			AUX_RGBImageRec *pBitmap = NULL;
			pBitmap = auxDIBImageLoad(filename);	
			
			pData = pBitmap->data;
            imageWidth = pBitmap->sizeX;
			imageHeight = pBitmap->sizeY;
			imageBPP = 24;
			
			
			if (pData == 0) {
				MessageBox(NULL, "Could not read BMP Texture.","ERROR",MB_OK|MB_ICONEXCLAMATION);
				TRACE("\nBMP data is empty, invalid file found.\n");
				return -1;
			}
		}
		// Do the TGA thing
		////////////////////////////////////
		if (strcmp(capFilename + (len - 3), "TGA") == 0) {

			TRACE("\nLoading TGA texture from file\n");
			
			loadTGA(tex, filename);
			pData = tex.imageData;
            imageWidth = tex.width;
			imageHeight = tex.height;
			imageBPP = tex.bpp;
	
			if (pData == 0) {
				MessageBox(NULL, "Could not read TGA Texture.","ERROR",MB_OK|MB_ICONEXCLAMATION);
				TRACE("\nTGA data is empty, invalid file found.\n");
				return -1;
			}
		}
	}
	else {
		MessageBox(NULL, "Invalid Texture Found, must be 24bit BMP, or 24/32bit TGA","ERROR",MB_OK|MB_ICONEXCLAMATION);
		TRACE("\nInvalid Texture Found, must be BMP or TGA\n");
		return -1;
	}
	
	// assign a valid TextureID
	unsigned int newTextureID = getNewTextureID(textureID);

	// bind the texture, register with openGL (this tells GL what texture i'm working with)
	glBindTexture (GL_TEXTURE_2D, newTextureID);
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	//////////////////////////////////////////////////////////////////
	// Build textures based on preferences passed to function
	//////////////////////////////////////////////////////////////////
	if (repeat == CLAMP){
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else{
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	if (mip == NOMIPMAPS){	
		if (quality == NICEST){
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}
		else if (quality == NICE){
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);			
		}
		else{
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	
		}
		// these commands actually upload the texture data to video memory
		glTexImage2D (GL_TEXTURE_2D,  0, (imageBPP == 24 ? GL_RGB : GL_RGBA), imageWidth, imageHeight, 0, 
    				  (imageBPP == 24 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, pData);	
	}
	else{
		if (quality == NICEST){
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);			
		}
		else if (quality == NICE){
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);			
		}
		else{
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		}
		// these commands actually upload the texture data to video memory
		gluBuild2DMipmaps (GL_TEXTURE_2D, (imageBPP == 24 ? 3:4), imageWidth, imageHeight, 
						  (imageBPP == 24 ? GL_RGB : GL_RGBA), GL_UNSIGNED_BYTE, pData);
	}

	delete [] pData;
	return newTextureID;
}


///////////////////////////////////////// LoadTGA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Loads A TGA File Into Memory (supports alpha channel)
///
/// Thanks to Jeff Molofee 2000 for this TGA code
///
///////////////////////////////////////// LoadTGA \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
bool TextureManager::loadTGA(TGATexture &texture, const char *filename) {		
	 
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLubyte		temp;										// Temporary Variable

	FILE *file = fopen(filename, "rb");	

	if(	file==NULL ||
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL){									// Did The File Even Exist? *Added Jim Strong*
			return false;
		}else{
		
			fclose(file);									// If Anything Failed, Close The File
			return false;
		}
	}

	texture.width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture.height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture.width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texture.height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);
		return false;
	}

	texture.bpp	= header[4];								// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture.bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture.width*texture.height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture.imageData=(GLubyte *)malloc(imageSize);			// Reserve Memory To Hold The TGA Data

	if(	texture.imageData==NULL ||	
		fread(texture.imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture.imageData!=NULL)
			free(texture.imageData);

		fclose(file);
		return false;
	}

	// Targa files store pixles in BGR format.  This is no good due to the fact that
	// OpenGL stores them in RGB, so flip them around!
	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)	{
		temp=texture.imageData[i];
		texture.imageData[i] = texture.imageData[i + 2];
		texture.imageData[i + 2] = temp;
	}

	fclose (file);
	return true;
}

////////////////////////////////// LoadBMP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	A simple function (using GLaux Lib) to load a 24 bit bitmap file
///
////////////////////////////////// LoadBMP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
AUX_RGBImageRec * Texture::LoadBMP(char * Filename){
	// Checks if file exists, then loads A Bitmap Image
	// using auxDIBImageLoad
	/////////////////////////////////////////////////
	FILE *pFile=NULL;						
	
	if (!Filename)			
		return NULL;							
	
	pFile=fopen(Filename,"r");				

	if (pFile){									
		fclose(pFile);							
		return auxDIBImageLoad(Filename);		
	}

	char strMessage[50];
	sprintf(strMessage, "Unable to load texture: %s!", Filename);
	MessageBox(NULL, strMessage,"ERROR",MB_OK|MB_ICONEXCLAMATION);
	return NULL;
}


//////////////////////////// freeTexture \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Frees memory for a single texture managed by TextureManager
///
//////////////////////////// freeTexture \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void TextureManager::freeTexture (int ID) {
	int index = -1;

	TRACE("\nFreeing Texture ID %d\n", ID);

	for (int i = 0; i < singleton->numAvailable; i++) {
		if (singleton->textIDs [i] == ID) {
			singleton->textIDs [i] = -1;
			index = i;
			break;		// there should only be one instance of an ID (if any)
		}
	}

	// if one was found
	if (index != -1) {
		unsigned int GLID = (unsigned int) ID;
		glDeleteTextures (1, &GLID);
	}
}

//////////////////////////// freeAll \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Clears every texture managed by TextureManager
///
//////////////////////////// freeAll \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void TextureManager::freeAll (void) {

	TRACE("\nFreeing all Texture Memory\n");
	
	// copy the ids to an unsigned integer array
	unsigned int *UIIDs = new unsigned int [singleton->numTextures];
	int i, j;
	for (i = 0, j = 0; i < singleton->numTextures; i++) {
		if (singleton->textIDs [i] != -1) {
			UIIDs [j] = singleton->textIDs [i];
			j++;
		}
	}

	// let openGL delete the texture reference
	glDeleteTextures (singleton->numTextures, UIIDs);

	delete [] UIIDs;
	delete [] singleton->textIDs;

	// reset the TextureManager
	singleton->textIDs = new unsigned int [NUMTEXTURES];
	singleton->numAvailable = NUMTEXTURES;
	for (i = 0; i < NUMTEXTURES; i++)
		singleton->textIDs [i] = -1;
	
	singleton->numTextures = 0;
}







int Texture::LoadGLTextures(GLuint * texture, int trackIndex){							
	
	buildSkyBoxTextures(texture, trackIndex);
	buildCubeMapTextures(texture, trackIndex);

	return TRUE;
}

////////////////////////////// buildSkyBoxTextures \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	
///
////////////////////////////// buildSkyBoxTextures \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Texture::buildSkyBoxTextures(GLuint * texture, int trackIndex) {
	
	TRACE("\nCreating Sky Box Textures.\n");

	AUX_RGBImageRec *TextureImage[6];
	memset(TextureImage,0,sizeof(void *)*1);

	if (trackIndex == 0){
		TextureImage[0]=LoadBMP("../textures/skybox/track1back.bmp");
		TextureImage[1]=LoadBMP("../textures/skybox/track1front.bmp");
		TextureImage[2]=LoadBMP("../textures/skybox/track1right.bmp");
		TextureImage[3]=LoadBMP("../textures/skybox/track1left.bmp");
		TextureImage[4]=LoadBMP("../textures/skybox/track1top.bmp");
		TextureImage[5]=LoadBMP("../textures/skybox/track1bottom.bmp");
	}
	else{
		TextureImage[0]=LoadBMP("../textures/skybox/track2back.bmp");
		TextureImage[1]=LoadBMP("../textures/skybox/track2front.bmp");
		TextureImage[2]=LoadBMP("../textures/skybox/track2right.bmp");
		TextureImage[3]=LoadBMP("../textures/skybox/track2left.bmp");
		TextureImage[4]=LoadBMP("../textures/skybox/track2top.bmp");
		TextureImage[5]=LoadBMP("../textures/skybox/track2bottom.bmp");
	}
	
	glGenTextures(6, &texture[4]);	

	for (int i = 0, j = 4; i < 6; i++, j++){
		// This sets the alignment requirements for the start of each pixel row in memory.
		glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

		glBindTexture(GL_TEXTURE_2D, texture[j]);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);
		

		// 0x812F makes the textures clamp to the EXACT edges of the rectangle so no seams appear
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if (TextureImage[i]){		
			if (TextureImage[i]->data)	
				free(TextureImage[i]->data);
			free(TextureImage[i]);	
		}
	}
}

////////////////////////////// buildCubeMapTextures \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///
///	Create cubeMap textures.  Since environment maps are distorted and blended, low
/// resolution can be used without affecting visual quality.
///
////////////////////////////// buildCubeMapTextures \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
void Texture::buildCubeMapTextures(GLuint * texture, int trackIndex) {

	TRACE("\nCreating Cube Maps.\n");
	
	AUX_RGBImageRec *TextureImage[6];
	memset(TextureImage,0,sizeof(void *)*1);

	// map orientation (according to mesh orientation)
	if (trackIndex == 0){
		TextureImage[0]=LoadBMP("../textures/cubemaps/front.bmp"); // +x
		TextureImage[1]=LoadBMP("../textures/cubemaps/back.bmp");  // -x
		TextureImage[2]=LoadBMP("../textures/cubemaps/right.bmp"); // +y
		TextureImage[3]=LoadBMP("../textures/cubemaps/left.bmp");  // -y
		TextureImage[4]=LoadBMP("../textures/cubemaps/top.bmp");   // +z
		TextureImage[5]=LoadBMP("../textures/cubemaps/top.bmp");   // -z
	}
	else{
		TextureImage[0]=LoadBMP("../textures/cubemaps/front.bmp"); // +x
		TextureImage[1]=LoadBMP("../textures/cubemaps/back.bmp");  // -x
		TextureImage[2]=LoadBMP("../textures/cubemaps/right.bmp"); // +y
		TextureImage[3]=LoadBMP("../textures/cubemaps/left.bmp");  // -y
		TextureImage[4]=LoadBMP("../textures/cubemaps/top.bmp");   // +z
		TextureImage[5]=LoadBMP("../textures/cubemaps/top.bmp");   // -z
	}
	
	for (int i=0,j=20; i<6; i++,j++) {
	
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i, 
						0, GL_RGB,
						TextureImage[i]->sizeX,
						TextureImage[i]->sizeY,
						0, GL_RGB,
						GL_UNSIGNED_BYTE,
						TextureImage[i]->data);

		// point filtering for speed
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_MAG_FILTER,GL_NEAREST); 
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	}

	// textures have been uploaded to video memory
	// free system memory
	for (int i = 0; i < 6; i++){
		if (TextureImage[i]){		
			if (TextureImage[i]->data){	
					free(TextureImage[i]->data);
				free(TextureImage[i]);	
			}
		}
	}
}