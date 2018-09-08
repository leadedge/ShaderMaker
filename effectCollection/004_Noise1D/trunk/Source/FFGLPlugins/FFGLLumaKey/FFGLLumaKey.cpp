#include <FFGL.h>
#include <FFGLLib.h>
#include "FFGLLumaKey.h"

#define FFPARAM_Luma (0)

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo (
	FFGLLumaKey::CreateInstance,	// Create method
	"GLLK",								// Plugin unique ID
	"FFGLLumaKey",					// Plugin name
	1,									// API major version number
	000,								  // API minor version number
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"Sample FFGL LumaKey plugin",	// Plugin description
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
"uniform sampler2D textureDest;"
"uniform sampler2D textureSrc;"
"uniform float luma;"
"const vec4 grayScaleWeights = vec4(0.30, 0.59, 0.11, 0.0);"
"void main( void )"
"{"
"    vec4 colorDest = texture2D(textureDest,gl_TexCoord[0].st);"
"    vec4 colorSrc = texture2D(textureSrc,gl_TexCoord[0].st);"
"    vec4 scaledColor = colorSrc * grayScaleWeights;"
"    float luminance = scaledColor.r + scaledColor.g + scaledColor.b;"
"    if (luminance  >= (1.0-luma)) {"
"	 gl_FragColor  =  colorSrc;"
"    } else {"
"	 gl_FragColor  =  colorDest;"
"    }"
"}";


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLLumaKey::FFGLLumaKey()
:CFreeFrameGLPlugin(),
 m_initResources(1),
 m_inputTextureLocation1(-1),
 m_inputTextureLocation2(-1),
 m_maxCoordsLocation(-1),
 m_LumaLocation(-1)
{
	// Input properties
	SetMinInputs(2);
	SetMaxInputs(2);

	// Parameters
	SetParamInfo(FFPARAM_Luma, "Luma", FF_TYPE_STANDARD, 0.5f);
	m_Luma = 0.5f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////
DWORD FFGLLumaKey::InitGL(const FFGLViewportStruct *vp)
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
  m_inputTextureLocation1 = m_shader.FindUniform("textureDest");
  m_inputTextureLocation2 = m_shader.FindUniform("textureSrc");
  m_LumaLocation = m_shader.FindUniform("luma");

  //the 0 means that the 'inputTexture' in
  //the shader will use the texture bound to GL texture unit 0
  m_extensions.glUniform1iARB(m_inputTextureLocation1, 0);
  m_extensions.glUniform1iARB(m_inputTextureLocation2, 1);

  m_shader.UnbindShader();
  return FF_SUCCESS;
}

DWORD FFGLLumaKey::DeInitGL()
{
  m_shader.FreeGLResources();
  return FF_SUCCESS;
}

DWORD FFGLLumaKey::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<2)
    return FF_FAIL;

  if (pGL->inputTextures[0]==NULL)
    return FF_FAIL;

  if (pGL->inputTextures[1]==NULL)
    return FF_FAIL;

  //activate our shader
  m_shader.BindShader();

  FFGLTextureStruct &TextureDest = *(pGL->inputTextures[0]);
  FFGLTextureStruct &TextureSrc  = *(pGL->inputTextures[1]);

  //activate rendering with the input texture
  //note that when using shaders, no glEnable(Texture.Target) is required
  m_extensions.glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, TextureDest.Handle);

  m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, TextureSrc.Handle);

  //get the max s,t that correspond to the
  //width,height of the used portion of the allocated texture space
  FFGLTexCoords maxCoords = GetMaxGLTexCoords(TextureDest);


  //assign the LumaKeyAmount
  m_extensions.glUniform1fARB(m_LumaLocation, m_Luma);

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

  m_shader.UnbindShader();

  //GL_TEXTURE1 is still active
  //m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  m_extensions.glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  return FF_SUCCESS;
}

DWORD FFGLLumaKey::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {

	case FFPARAM_Luma:
    *((float *)(unsigned)&dwRet) = m_Luma;
		return dwRet;

	default:
		return FF_FAIL;
	}
}

DWORD FFGLLumaKey::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {

		switch (pParam->ParameterNumber) {

		case FFPARAM_Luma:
			m_Luma = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;

	}

	return FF_FAIL;
}
