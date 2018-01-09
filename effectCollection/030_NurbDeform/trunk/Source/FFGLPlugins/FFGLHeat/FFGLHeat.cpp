
#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"

#include "glu.h"
#include "glext.h"


#include "NurbPlane.h"

#define PI 3.14159265359

  
#define FFPARAM_BRIGHTNESS   (0)  
#define FFPARAM_LIGHTPOS_X   (1) 
#define FFPARAM_LIGHTPOS_Y   (2) 
#define FFPARAM_DECAY   (3) 
#define FFPARAM_WEIGHT   (4) 
#define FFPARAM_EXPOSURE   (5) 
#define FFPARAM_DENSITY   (6) 

#define FFPARAM_LIGHT_RADIUS   (7) 

#define FFPARAM_USE_ALPHA   (8) 


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
	//   .... <- max length
	"S023",								// Plugin unique ID											
	//   ................ <- max length
	"sOm-GodRays",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	003,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"sOm-GodRays",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - GodRays. " // About
	);

char *vertexShaderCode =
	"void main()"
	"{"
	"  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
	"  gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;"
	"  gl_FrontColor = gl_Color;"
	"}";





char *fragmentShaderCodeSimple =
	"float PI=3.14159265359;"
	"uniform float aspectRatio;"
	"uniform sampler2D iChannel0;" 
	"uniform sampler2D blurredInput;" 
	"uniform vec2 iResolution;"
	"uniform vec2 maxCoords;" 
	"uniform float bias;" 
	"void main() "
	"{"
	"vec2 uv = gl_TexCoord[1].st;" 
	"vec4 col = texture2D(iChannel0, uv );" 
	 
	"   gl_FragColor =col;"  	 
	 
	"}";

 
////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLSineWave2D::FFGLSineWave2D()
	:CFreeFrameGLPlugin(),
	m_initResources(1),
	m_maxCoordsLocation(-1)
{
	// Input properties
	SetMinInputs(1); 
	SetMaxInputs(1);
	SetTimeSupported(false);
	// Parameters
	// Param swizzle, somehow BPM is not accepted ... right now .. ;)   
	SetParamInfo(FFPARAM_BRIGHTNESS, "Bias", FF_TYPE_STANDARD, 0.5f);   
	SetParamInfo(FFPARAM_LIGHTPOS_X, "Light Pos X", FF_TYPE_STANDARD, 0.5f);   
	SetParamInfo(FFPARAM_LIGHTPOS_Y, "Light Pos Y", FF_TYPE_STANDARD, 0.5f);   
	SetParamInfo(FFPARAM_WEIGHT, "Weight", FF_TYPE_STANDARD, 1.0f);   
	SetParamInfo(FFPARAM_DECAY, "Decay", FF_TYPE_STANDARD, 0.9f);   
	SetParamInfo(FFPARAM_DENSITY, "Density", FF_TYPE_STANDARD, 0.7f);   
	SetParamInfo(FFPARAM_EXPOSURE, "Exposure", FF_TYPE_STANDARD, 0.6f);   
	SetParamInfo(FFPARAM_LIGHT_RADIUS, "Light Radius", FF_TYPE_STANDARD, 0.25f);   

	SetParamInfo(FFPARAM_USE_ALPHA, "UseAlpha", FF_TYPE_BOOLEAN, 0.0f);   

	s_brightness=1.0;
	m_bluramount=1.0;
	s_brightnessadd=0.6;
	s_power=0.5;
	s_poweradd=0.5;
	s_color_red=0.33;
	s_color_green=0.33;
	s_color_blue=0.33;
	m_Amount=0.5;
	m_Radius=0.5;

	lastWidth=0;
	lastHeight=0;

	frameindexforswapping=0;

	timeCounter=0;  
	GetSystemTime(&m_SystemTimeLastFrame); 
	// Initialise System Time Based Time counting	 
	GetSystemTime(&m_SystemTimeStart);

	m_Time=0;
}
void FFGLSineWave2D::initBlurShader(){


}
void FFGLSineWave2D::initDownScaleBiasShader()
{
 

	m_shaderSimple.SetExtensions(&m_extensions);

	m_shaderSimple.Compile(vertexShaderCode,fragmentShaderCodeSimple);

	m_inputTextureLocationSimple = m_shaderSimple.FindUniform("iChannel0"); 
	m_blurTextureLocationSimple = m_shaderSimple.FindUniform("blurredInput"); 
	m_maxCoordsLocation= m_shaderSimple.FindUniform("maxCoords"); 

	m_biasLocation=m_shaderSimple.FindUniform("bias");
	m_shaderSimple.BindShader();
	//the 1 means that the 'inputTexture' in
	//the shader will use the texture bound to GL texture unit 1
	m_extensions.glUniform1iARB(m_inputTextureLocationSimple,0);
	m_extensions.glUniform1iARB(m_blurTextureLocationSimple,1);
}
void FFGLSineWave2D::initFinalShader()
{}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLSineWave2D::InitGL(const FFGLViewportStruct *vp)
{
	//initialize gl extensions and
	//make sure required features are supported
	m_extensions.Initialize();
	if (m_extensions.multitexture==0 || m_extensions.ARB_shader_objects==0)
		return FF_FAIL; 


	//initialize gl shader


	initDownScaleBiasShader();

	// Blur SHader
	initBlurShader();
	// Blur Shader End


	// Nurb test
initBezier();
	  mybezier.dlBPatch = genBezier(mybezier, 7,m_extensions);          // Generate The Patch ( NEW )




	//create/upload the heat texture on texture unit 0
	//(which should already be the active unit) 

	// glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	 
	// Frame buffer to store fire/feedback
	//// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer. //

	 

	return FF_SUCCESS;
}

DWORD FFGLSineWave2D::DeInitGL()
{ 
	m_shaderSimple.FreeGLResources(); 

	 
	return FF_SUCCESS;
}

DWORD  FFGLSineWave2D::SetTime(double time) {
	m_Time=time;
	return FF_SUCCESS; 
}


double FFGLSineWave2D::systemTimeToDouble(SYSTEMTIME *time){
	return  ((double)time->wMinute*60000+ (double)time->wSecond*1000+(double)time->wMilliseconds)/(double)1000.0f;
}
void FFGLSineWave2D::updateRenderTargets(int newWidth,int newHeight)
{
 
}


double FFGLSineWave2D::GetTimeInSecondsSinceStart(){
	// Update Time Based on Systemtime and initial stored system time...
	SYSTEMTIME st;    
	GetSystemTime(&st); 

	double delta=systemTimeToDouble(&st)-systemTimeToDouble(&m_SystemTimeLastFrame);

	// increase time counter;
	timeCounter+=delta;
	 
	m_SystemTimeLastFrame=st;

	//   return systemTimeToDouble(&st);
	return timeCounter;

}


void  FFGLSineWave2D::renderDownScaleInput(){


}


DWORD FFGLSineWave2D::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	if (pGL->numInputTextures<1) return FF_FAIL;

	if (pGL->inputTextures[0]==NULL) return FF_FAIL;
	updateRenderTargets(pGL->inputTextures[0]->Width/2,pGL->inputTextures[0]->Height/2);
	
	renderDownScaleInput();

	//activate our shader
	// m_shader.BindShader();

	//the 1 means that the 'inputTexture' in
	//the shader will use the texture bound to GL texture unit 1
	//  m_extensions.glUniform1iARB(m_inputTextureLocation, 1);
	//the 0 means that the 'heatTexture' in
	//the shader will use the texture bound to GL texture unit 0
	// m_extensions.glUniform1iARB(m_lastframeTextureLocation, 0);

	FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);
//	maxCoords.s=1;
	//maxCoords.t=1;
	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);

	// Create The Filtered Input  
	// 
	m_extensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture.Handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);   

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1,1,-1,1);
	glViewport(0,0,pGL->inputTextures[0]->Width,pGL->inputTextures[0]->Height);

	 
	//draw the quad that will be painted by the shader/textures
	//note that we are sending texture coordinates to texture unit 1..
	//the vertex shader and fragment shader refer to this when querying for
	//texture coordinates of the inputTexture 
	 
	/////////////////////////////////////////////////////////////////////
	// End Of Bias Filtering
	/////////////////////////////////////////////////////////////////////
	 
 	/////////////////////////////////////////////////////////////////////
	// Now Apply The Blur Step
	/////////////////////////////////////////////////////////////////////





	/////////////////////////////////////////////////////////////////////
	// Blurring End
	/////////////////////////////////////////////////////////////////////

 m_shaderSimple.BindShader();
  
	//glDisable(GL_BLEND); 
//	m_extensions.glUniform1iARB(m_inputTextureLocationSimple, 0);   
	//m_extensions.glUniform1fARB(m_biasLocation, s_brightness);   
	
	//m_extensions.glUniform2fARB(m_maxCoordsLocation,maxCoords.s,maxCoords.t);   

	
	m_extensions.glActiveTexture(GL_TEXTURE0); 
glBindTexture(GL_TEXTURE_2D, Texture.Handle);
	m_extensions.glActiveTexture(GL_TEXTURE1); 
glBindTexture(GL_TEXTURE_2D, Texture.Handle);
	//glBindTexture(GL_TEXTURE_2D, renderedTextureLoRes);
 

	
	// Feed Original Texture 
	
	renderQuad(maxCoords.s,maxCoords.t);
	 

//	DrawGLScene();
	   glCallList(mybezier.dlBPatch);                  // Call The Bezier's Display List
DrawGLScene();

	m_shaderSimple.UnbindShader();
	//unbind the input texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//switch to texture unit 0 and unbind the 1D heat texture
	m_extensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, 0);
	//m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pGL->HostFBO) ;
	//unbind the shader



	frameindexforswapping++;
	return FF_SUCCESS;
}

void FFGLSineWave2D::renderQuad(float texMaxX,float texMaxY)
{

	glBegin(GL_QUADS);

	//lower left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);
	glVertex2f(-1,-1);

	//upper left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,texMaxY);
	glVertex2f(-1,1);

	//upper right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, texMaxX,texMaxY);
	glVertex2f(1,1);

	//lower right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1,texMaxX, 0);
	glVertex2f(1,-1);

	glEnd();

}
DWORD FFGLSineWave2D::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {

		 
		 
	default:
		return FF_FAIL;
	}
}

DWORD FFGLSineWave2D::SetParameter(const SetParameterStruct* pParam)
{
	float temp=0.0;
	if (pParam != NULL) {

		switch (pParam->ParameterNumber) {

		case FFPARAM_LIGHTPOS_X:	
			lightpos.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		 
		case FFPARAM_LIGHTPOS_Y:	
			lightpos.y=*((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		 
			
		case FFPARAM_BRIGHTNESS:	
			s_brightness= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		 
		 
		 
			
		case FFPARAM_DECAY:	
			s_decay= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_DENSITY:	
			s_density= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_WEIGHT:	
			s_weight				= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_EXPOSURE:	
			s_exposure= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		 
		case FFPARAM_LIGHT_RADIUS:	
			s_lightradius= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		 
		 
		case FFPARAM_USE_ALPHA:	
			  temp= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			useAlpha=temp!=0.0;
			break; 
		 
		 
		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;

	}

	return FF_FAIL;
}
