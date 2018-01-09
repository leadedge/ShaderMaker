#include <FFGL.h>
#include <FFGLLib.h>
#include "FFGLMirror.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLMirror::CreateInstance,	// Create method
	"GLMR",								// Plugin unique ID											
	"FFGLMirror",			// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"Sample FFGL Mirror plugin",	// Plugin description
	"by Trey Harrison - www.harrisondigitalmedia.com" // About
);


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLMirror::FFGLMirror()
: CFreeFrameGLPlugin()
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);

	// No Parameters
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLMirror::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<1)
    return FF_FAIL;

  if (pGL->inputTextures[0]==NULL)
    return FF_FAIL;
  
  FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

  //bind the texture handle to its target
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);

  //enable texturemapping
  glEnable(GL_TEXTURE_2D);

  //get the max s,t that correspond to the 
  //width,height of the used portion of the allocated texture space
  FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

  //modulate texture colors with white (just show
  //the texture colors as they are)
  glColor4f(1.f, 1.f, 1.f, 1.f);
  //(default texturemapping behavior of OpenGL is to
  //multiply texture colors by the current gl color)
  
  //first, the left side of the mirror
  glBegin(GL_QUADS);

  //lower left
  glTexCoord2d(0,0);
  glVertex2f(-1,-1);

  //upper left
  glTexCoord2d(0, maxCoords.t);
  glVertex2f(-1,1);

  //upper right
  glTexCoord2d(maxCoords.s*0.5, maxCoords.t);
  glVertex2f(0,1);

  //lower right
  glTexCoord2d(maxCoords.s*0.5, 0.0);
  glVertex2f(0,-1);
  glEnd();

  //now, the right side of the mirror
  glBegin(GL_QUADS);

  //lower left
  glTexCoord2d(maxCoords.s*0.5, 0.0);
  glVertex2f(0,-1);

  //upper left
  glTexCoord2d(maxCoords.s*0.5, maxCoords.t);
  glVertex2f(0,1);

  //upper right
  glTexCoord2d(0.0, maxCoords.t);
  glVertex2f(1,1);

  //lower right
  glTexCoord2d(0.0, 0.0);
  glVertex2f(1,-1);
  glEnd();

  //unbind the texture
  glBindTexture(GL_TEXTURE_2D, 0);

  //disable texturemapping
  glDisable(GL_TEXTURE_2D);

  return FF_SUCCESS;
}
