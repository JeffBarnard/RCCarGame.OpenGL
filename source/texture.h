//////////////////////////////////////////////////////////////////////////////
////
////  Texture Management				
////
////  Jeff Barnard  2004
////
////  Thanks to Christopher.Smith@Trinity.edu (www.cs.trinity.edu/~csmith8)
////  For his excellent Article on texture management in OpenGL
////																			
//////////////////////////////////////////////////////////////////////////////

#ifndef _TEXTURE_H
#define _TEXTURE_H

#define GL_CLAMP_TO_EDGE  0x812F  // check on nVidia cards

#include <gl\glaux.h>	
#include <stdio.h>	
#include <math.h>
#include <windows.h>	
#include <mmsystem.h>
#include "debug.h"

#define NUMTEXTURES 40
#define TEXTURE_STEP 10

enum TextureQuality{
	NICEST,     // bilinear/trilinear filtering, very nice
	NICE,		// in between :P
	POOR		// poor filtering, very fast though
};

namespace Texture {

	struct TGATexture{
		GLubyte	*imageData;	
		GLuint	bpp;
		GLuint	width;
		GLuint	height;
		GLuint	texID;
	};

	enum TextureMip{
		NOMIPMAPS,  // minimal memory, aliasing at distance and slower 
		MIPMAPS     // lots of memory, little aliasing and better performance
	};

	enum TextureRepeat{
		CLAMP,     // stretch and clamp completely to edge
		REPEAT     // repeat texture throughout UV coords
	};

	///////////////////////////////////////////////////////
	/// Texture Manager Class (Singleton Object)
	///////////////////////////////////////////////////////
	class TextureManager {

	private:
		static TextureManager * singleton;  // static singleton pointer

		int numTextures;		// number of currently allocated textures
		int numAvailable;		// number of textures available in total
		unsigned int *textIDs;  // array of texture ID's

		TextureManager(void);
		static void initialize(void);
		unsigned int getNewTextureID(int possibleTextureID);

		///////////////////////////////////////////////////////
		/// File Type loading functions (TGA, BMP)
		///////////////////////////////////////////////////////
		bool loadTGA(TGATexture &texture, const char *filename);

	public:

		//  IMPORTANT: call ALL functions through this, or the class wont work at all.
		static TextureManager & getSingleton(void);
		 
		///  loadTexture Parameter Reference:
		///	 - textureID should be left at -1 to let openGL decide the texID
		///  - mip is mipmapping, 1 is true, 0 is false
		///  - quality is one of NICEST, NICE, or POOR
		///  - repeat, 1 for texture repeating, 0 for clamping
		unsigned int loadTexture (const char *filename, int textureID = -1, int mip = 1, int quality = NICEST, int repeat = 1);

		void freeTexture(int ID);
		void freeAll(void);
		static void destroy(void);
	};

	AUX_RGBImageRec * LoadBMP(char * Filename);	

	int LoadGLTextures(GLuint * texture, int trackIndex);	
	void buildSkyBoxTextures(GLuint * texture, int trackIndex);
	void buildCubeMapTextures(GLuint * texture, int trackIndex);
}

#endif // _TEXTURE_H