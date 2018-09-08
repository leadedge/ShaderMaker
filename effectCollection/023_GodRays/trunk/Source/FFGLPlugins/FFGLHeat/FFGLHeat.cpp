
#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"

#include "Headers.h"
#include "glu.h"
#include "glext.h"

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
	001,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"sOm-GodRays",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - GodRays. \n " // About 
	);

char *vertexShaderCode =STRINGIFY(

	void main()
	{
	  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	  gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;
	  gl_FrontColor = gl_Color;
	}
	);





char *fragmentShaderCodeSimple =
	
	"\t\n"
"float PI=3.14159265359;\n"
"\tuniform float aspectRatio;\n"
"\tuniform sampler2D iChannel0; \n"
"\tuniform sampler2D blurredInput; \n"
"\tuniform vec2 iResolution;\n"
"\tuniform vec2 maxCoords;\n"
"\tuniform float bias; \n"
"\n"
"\t  \n"
"\tuniform vec2 lightpos; \n"
"uniform float useAlpha; \n"
" uniform vec3 Color;\n"
" uniform float radius;\n"
"\t\n"
"float light(vec2 uv){\n"
"\n"
"\tfloat val=length( uv-lightpos);\n"
" if(val-radius >0.0)return 0.0;\n"
"return 1.0-val/radius;\n"
"}\n"
"\n"
"\n"
"\tvec4 downScale(vec2 uv) \n"
"\t{\n"
"\t\tvec4 result; \n"
"\t\tvec4 col = texture2D(iChannel0, uv );\n"
"\t\tif(useAlpha<0.5){\n"
"\t\t\tfloat luminance= 0.2126*col.x + 0.7152*col.y + 0.0722*col.z;\n"
"\t\t\tif(luminance<bias){\n"
"\t\t\t\tresult=vec4(light(uv));\n"
"\t\t\t}else{\n"
"\t\t\t\tresult=vec4(0,0,0,0);    \n"
"\t\t\t}\n"
"\t\t}else{\n"
"\t\t   result=vec4((1.0-col.w)*light(uv));     \n"
"\t\t}\n"
"\n"
"\t\treturn result;\n"
"\t} \n"
"\n"
"\n"
"\t\n"
"uniform  float exposure;\n"
"uniform float decay2;\n"
"uniform float density;\n"
"uniform float weight;\n"
"const int NUM_SAMPLES = 150 ;\n"
"vec4 godRay(vec2 uv){\n"
"\t\tvec2 deltaTextCoord = vec2( uv - lightpos.xy );\n"
"\t    \tvec2 textCoo =uv;\n"
"\t    \tdeltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;\n"
"\t    \tfloat illuminationDecay = 1.0;\n"
"\t\t\tvec4 result=vec4(0);\n"
"\t    \tfor(int i=0; i < NUM_SAMPLES ; i++)\n"
"\t        {\n"
"\t                 textCoo -= deltaTextCoord;\n"
"\t                 vec4 samplex =downScale(textCoo);\n"
"\t\t\t\t\t  \n"
"\t              //    vec4 samplex =texture2D(iChannel0,textCoo)*(.01);\n"
"\t                 //samplex *= illuminationDecay * (1.0/float(NUM_SAMPLES));\n"
"\t                 samplex *= illuminationDecay * weight;\n"
"\t                 result += samplex; \n"
"\t                 illuminationDecay *= decay2;\n"
"\t         }\n"
"\t    \tresult= result*exposure;\n"
"\t\t\t\n"
"\t\t\tresult.w=length(result.xyz);\n"
"\n"
"\t\t\treturn result;\n"
"\n"
"}\n"
"\n"
"\n"
"\n"
"\tvoid main() \n"
"\t{\n"
"\tvec2 uv = gl_TexCoord[1].st;\n"
"\tvec2 pixel=1.0/iResolution;\n"
"\tvec4 col = texture2D(iChannel0, uv );\n"
"\n"
"\t//vec4 colblurry = texture2D(blurredInput, uv ); \n"
"\t vec4 colblurry=downScale( gl_TexCoord[1].st);\n"
"\t vec4 godray=godRay( gl_TexCoord[1].st);\n"
"\t   gl_FragColor =col+godray;\n"
" \n"
"\t}"
	
	;

 
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
	SetParamInfo(FFPARAM_WEIGHT, "Weight", FF_TYPE_STANDARD, .15f);   
	SetParamInfo(FFPARAM_DECAY, "Decay", FF_TYPE_STANDARD, 0.95f);   
	SetParamInfo(FFPARAM_DENSITY, "Density", FF_TYPE_STANDARD, 0.85f);   
	SetParamInfo(FFPARAM_EXPOSURE, "Exposure", FF_TYPE_STANDARD, 1.0f);   
	SetParamInfo(FFPARAM_LIGHT_RADIUS, "Light Radius", FF_TYPE_STANDARD, .25f);   

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
	shaderDownscale.createShaders(&m_extensions);
	
	shaderBlur.createShaders(&m_extensions);

	m_shaderSimple.SetExtensions(&m_extensions);

	m_shaderSimple.Compile(vertexShaderCode,fragmentShaderCodeSimple);

	m_inputTextureLocationSimple = m_shaderSimple.FindUniform("iChannel0"); 
	m_blurTextureLocationSimple = m_shaderSimple.FindUniform("blurredInput"); 
	m_maxCoordsLocation= m_shaderSimple.FindUniform("maxCoords"); 
	m_radiusLocation= m_shaderSimple.FindUniform("radius"); 
	
	m_decay2Location =m_shaderSimple. FindUniform("decay2"); 
	m_densityLocation =m_shaderSimple. FindUniform("density"); 
	m_weightLocation = m_shaderSimple.FindUniform("weight"); 
	m_exposureLocation = m_shaderSimple.FindUniform("exposure"); 
	
	m_lightPosLocation = m_shaderSimple.FindUniform("lightpos"); 

	m__useAlphaLocation = m_shaderSimple.FindUniform("useAlpha"); 

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






	//create/upload the heat texture on texture unit 0
	//(which should already be the active unit) 

	// glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	 
	// Frame buffer to store fire/feedback
	//// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer. //

	m_extensions.glGenFramebuffersEXT(1, &m_FramebufferId);
	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER, m_FramebufferId);
	glGenTextures(1, &renderedTexture1);
	 
	glGenTextures(1, &renderedTextureLoRes);

	return FF_SUCCESS;
}

DWORD FFGLSineWave2D::DeInitGL()
{
	shaderDownscale.FreeGLResources();
	m_shaderSimple.FreeGLResources();
	shaderBlur.FreeGLResources();

	if (renderedTextureLoRes)
	{
		glDeleteTextures(1, &renderedTextureLoRes);
		renderedTextureLoRes = 0;
	} 
	if (renderedTexture1)
	{
		glDeleteTextures(1, &renderedTexture1);
		renderedTexture1 = 0;
	}  
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
	if((newWidth!=lastWidth)||(newHeight!=lastHeight))
	{

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, renderedTexture1);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, newWidth, newHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		 		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, renderedTextureLoRes);

		// Give an empty image to OpenGL ( the last "0" )
		glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, newWidth, newHeight, 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

		lastWidth=newWidth;
		lastHeight=newHeight;
	}
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
	maxCoords.s=1;
	maxCoords.t=1;
	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);

	/*
	// Create The Filtered Input 
	shaderDownscale.BindShader();
	shaderDownscale.setLuminanceBias(s_brightness);
	shaderDownscale.setLightPos(lightpos.x,lightpos.y); 
	shaderDownscale.setLightRadius(s_lightradius);
	shaderDownscale.setUseAlpha(useAlpha);
	// 
	m_extensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture.Handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);   

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1,1,-1,1);
	glViewport(0,0,pGL->inputTextures[0]->Width/2,pGL->inputTextures[0]->Height/2);

	// Assign Downscaled Filtered Buffer
	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FramebufferId) ;
	m_extensions.glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderedTextureLoRes, 0);


	//draw the quad that will be painted by the shader/textures
	//note that we are sending texture coordinates to texture unit 1..
	//the vertex shader and fragment shader refer to this when querying for
	//texture coordinates of the inputTexture 
	glClearColor(0,0,0,0);
	glClear(GL_DEPTH|GL_COLOR);

	//renderQuad(1,1);
	/////////////////////////////////////////////////////////////////////
	// End Of Bias Filtering
	/////////////////////////////////////////////////////////////////////
	shaderBlur.BindShader(); 
	// 
	shaderBlur.setLightPos(lightpos.x,lightpos.y); 
	shaderBlur.setWeight(s_weight); 
	shaderBlur.setDensity(s_density); 
	shaderBlur.setDecay(s_decay); 
	shaderBlur.setExposure(s_exposure); 

	m_extensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,renderedTextureLoRes); 
	shaderBlur.setResolution(pGL->inputTextures[0]->Width/4,pGL->inputTextures[0]->Height/4);
  
		// Assign HorizBlur   Buffer

		m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FramebufferId) ;
		m_extensions.glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, renderedTexture1, 0);
		*/

	//	renderQuad(1.0,1.0);
  
	/////////////////////////////////////////////////////////////////////
	// Now Apply The Blur Step
	/////////////////////////////////////////////////////////////////////





	/////////////////////////////////////////////////////////////////////
	// Blurring End
	/////////////////////////////////////////////////////////////////////


	// FINAL OUTPUT

	// Re-Render	
	//  m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pGL->HostFBO) ;
//	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,  pGL->HostFBO) ;
//	shaderDownscale.UnbindShader();
	m_shaderSimple.BindShader();

	glViewport(0,0,pGL->inputTextures[0]->HardwareWidth,pGL->inputTextures[0]->HardwareHeight);
	glDisable(GL_BLEND); 
	m_extensions.glUniform1iARB(m_inputTextureLocationSimple, 0);   
	m_extensions.glUniform1fARB(m_biasLocation, s_brightness);   
	m_extensions.glUniform1fARB(m_radiusLocation, s_lightradius);   
	m_extensions.glUniform2fARB(m_lightPosLocation,lightpos.x,lightpos.y);


	m_extensions.glUniform1fARB(m_weightLocation,s_weight);
	m_extensions.glUniform1fARB(m_densityLocation,s_density);
	m_extensions.glUniform1fARB(m_decay2Location,s_decay);
	m_extensions.glUniform1fARB(m_exposureLocation,s_exposure);
	m_extensions.glUniform1fARB(m__useAlphaLocation,useAlpha);



	
	m_extensions.glUniform2fARB(m_maxCoordsLocation,maxCoords.s,maxCoords.t);   


	m_extensions.glActiveTexture(GL_TEXTURE0); 
glBindTexture(GL_TEXTURE_2D, Texture.Handle);
	//glBindTexture(GL_TEXTURE_2D, renderedTextureLoRes); 

	// Feed Original Texture 
	
	renderQuad(maxCoords.s,maxCoords.t);


	//unbind the input texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//switch to texture unit 0 and unbind the 1D heat texture
	m_extensions.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, 0);
	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pGL->HostFBO) ;
	//unbind the shader

	
	shaderDownscale.UnbindShader();
	shaderBlur.UnbindShader();
	m_shaderSimple.UnbindShader();

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
