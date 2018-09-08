#include <FFGL.h>
#include <FFGLLib.h>

#include "FFGLTile.h"

#define FFPARAM_TileX (0)
#define FFPARAM_TileY (1)

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo (
	FFGLTile::CreateInstance,	// Create method
	"GLTL",								// Plugin unique ID
	"FFGLTile",					// Plugin name
	1,									// API major version number
	000,								  // API minor version number
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"Sample FFGL Tile plugin",	// Plugin description
	"by Edwin de Koning & Trey Harrison - www.resolume.com, www.harrisondigitalmedia.com" // About
);

char *vertexShaderCode =
"void main()"
"{"
"	 gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"  gl_TexCoord[0] = gl_MultiTexCoord0;"
"  gl_FrontColor = gl_Color;"
"}";


char *fragmentShaderCode =
"uniform sampler2D inputTexture;"
"uniform vec2 maxCoords;"
"uniform vec2 tileAmount;"
"void main(void)"
"{"
"	 vec2 texCoord = mod(tileAmount*gl_TexCoord[0].xy,maxCoords);"
"  gl_FragColor = texture2D(inputTexture, texCoord);"
"}";


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLTile::FFGLTile()
:CFreeFrameGLPlugin(),
 m_initResources(1),
 m_inputTextureLocation(-1),
 m_maxCoordsLocation(-1),
 m_tileAmountLocation(-1)
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);

	// Parameters
	SetParamInfo(FFPARAM_TileX, "TileX", FF_TYPE_STANDARD, 0.5f);
	m_TileX = 0.5f;

	SetParamInfo(FFPARAM_TileY, "TileY", FF_TYPE_STANDARD, 0.5f);
	m_TileY = 0.5f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLTile::InitGL(const FFGLViewportStruct *vp)
{
  m_extensions.Initialize();

  if (m_extensions.ARB_shader_objects==0)
    return FF_FAIL;

  m_shader.SetExtensions(&m_extensions);
  m_shader.Compile(vertexShaderCode,fragmentShaderCode);

  //activate our shader
  m_shader.BindShader();

  //to assign values to parameters in the shader, we have to lookup
  //the "location" of each value.. then call one of the glUniform* methods
  //to assign a value
  m_inputTextureLocation = m_shader.FindUniform("inputTexture");
  m_maxCoordsLocation = m_shader.FindUniform("maxCoords");
  m_tileAmountLocation = m_shader.FindUniform("tileAmount");

  //the 0 means that the 'inputTexture' in
  //the shader will use the texture bound to GL texture unit 0
  m_extensions.glUniform1iARB(m_inputTextureLocation, 0);

  m_shader.UnbindShader();

  return FF_SUCCESS;
}

DWORD FFGLTile::DeInitGL()
{
  m_shader.FreeGLResources();
  return FF_SUCCESS;
}

DWORD FFGLTile::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<1)
    return FF_FAIL;

  if (pGL->inputTextures[0]==NULL)
    return FF_FAIL;

  //activate our shader
  m_shader.BindShader();

  FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

  //activate rendering with the input texture
  //note that when using shaders, no glEnable(Texture.Target) is required
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);

  //get the max s,t that correspond to the
  //width,height of the used portion of the allocated texture space
  FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

  //assign the maxCoords value in the shader
  //(glUniform2f assigns to a vec2)
  m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);

  //assign the tileAmount
  m_extensions.glUniform2fARB(m_tileAmountLocation, 20.0*m_TileX, 20.0*m_TileY);
  
  //draw the quad that will be painted by the shader/texture
  glBegin(GL_QUADS);

  //lower left
  glTexCoord2f(0,0);
  glVertex2f(-1,-1);

  //upper left
  glTexCoord2f(0, maxCoords.t);
  glVertex2f(-1,1);

  //upper right
  glTexCoord2f(maxCoords.s, maxCoords.t);
  glVertex2f(1,1);

  //lower right
  glTexCoord2f(maxCoords.s, 0);
  glVertex2f(1,-1);
  glEnd();

  //unbind the shader and texture
  glBindTexture(GL_TEXTURE_2D, 0);
  m_shader.UnbindShader();
  
  return FF_SUCCESS;
}

DWORD FFGLTile::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {

	case FFPARAM_TileX:
    *((float *)(unsigned)&dwRet) = m_TileX;
		return dwRet;
	case FFPARAM_TileY:
    *((float *)(unsigned)&dwRet) = m_TileY;
		return dwRet;

	default:
		return FF_FAIL;
	}
}

DWORD FFGLTile::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {
		
		switch (pParam->ParameterNumber) {

		case FFPARAM_TileX:
			m_TileX = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_TileY:
			m_TileY = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
