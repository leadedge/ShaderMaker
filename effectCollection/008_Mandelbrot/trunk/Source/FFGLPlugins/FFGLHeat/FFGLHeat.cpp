#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"
#define PI 3.14159265359
  

#define FFPARAM_MODSTEP   (0) 
#define FFPARAM_ANGLE   (1) 
#define FFPARAM_SCALE   (2) 
#define FFPARAM_CENTERX   (3) 
#define FFPARAM_CENTERY   (4) 

#define FFPARAM_SEED1_SPEED		(5) 
#define FFPARAM_SEED1_RADIUS    (6) 
#define FFPARAM_SEED1_POSX		(7) 
#define FFPARAM_SEED1_POSY		(8)
#define FFPARAM_SEED1_PAR1		(9) 
#define FFPARAM_SEED1_PAR2		(10)

#define FFPARAM_SEED2_SPEED     (11) 
#define FFPARAM_SEED2_RADIUS    (12) 
#define FFPARAM_SEED2_POSX		(13) 
#define FFPARAM_SEED2_POSY		(14) 

#define FFPARAM_SEED3_SPEED     (15) 
#define FFPARAM_SEED3_RADIUS    (16) 
#define FFPARAM_SEED3_POSX		(17) 
#define FFPARAM_SEED3_POSY		(18) 

#define FFPARAM_SCALE_HUE		(19) 
#define FFPARAM_SCALE_SATURATION (20) 
#define FFPARAM_SCALE_VALUE		(21) 

#define FFPARAM_IMAGE_BLEND		(22) 


#define FFPARAM_RESYNC			(23) 


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
//   .... <- max length
	"SFI1",								// Plugin unique ID											
//   ................ <- max length
	"sOm-HybridImage",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"sOm-HybridImage",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - ImageWarp Fractal. " // About
);

char *vertexShaderCode =
"void main()"
"{"
"  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"  gl_TexCoord[1] = gl_MultiTexCoord1;"
"  gl_FrontColor = gl_Color;"
"}";


char *fragmentShaderCode =
"float PI=3.14159265359;"
"uniform float aspectRatio;"
"uniform sampler2D inputTexture;"
"uniform sampler1D heatTexture;"
"uniform vec2 maxCoords;"
"uniform vec4 sineFactors;"
"uniform vec4 sineFactors2;"
"uniform float heatAmount;"
"uniform float time;" 
"uniform vec3 angles;"
"uniform vec4 seed1;"
"uniform vec4 seed2;"
"uniform vec4 seed3;"
"uniform vec4 location;"
"uniform vec3 hsvScale;"
"uniform int  modStep;"
"uniform float imageBlend;"
"vec3 hsv2rgb(vec3 c)"
"{"
"    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);"
"    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);"
"    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);"
"}"
"vec3 createColor(float time){\n"
"	vec3 currentColor=hsv2rgb(vec3(sin(time),1,1));"
"	return currentColor;"
"}"
""
"float sin3Big(float p){"
"	return sin(p)+sin(p*2)+sin(p*4);"
"}"
"float cos3Big(float p){"
"	return cos(p)+cos(p*2)+cos(p*4);"
"}"
"float circle(vec2 center, float radius) {"
"float l = length(center);"
"return 1-clamp((l/radius) ,0,1);" 
""
"}"
"vec4 alphaBlend(vec4 col1,vec4 col2){"
"vec3 newcolor=col1.xyz+col2.xyz*col2.w;"
" return clamp(vec4(newcolor,col1.w+(1-col1.w)*col2.w),0,1);"
"}"
"vec2 kali(vec2 z,vec2 c,float Scaling,float MinRadius) {"
"	float m =dot(z,z);"
"	if (m<MinRadius) {"
"		z = abs(z)/(MinRadius*MinRadius);"
"	}else {"
"		z = abs(z)/m*Scaling;"
"	}"
"	return z+c;"
"}"
"vec2 mandelbrot(vec2 z, vec2 c,float par1,float par2){"
"   return vec2((z.x * z.x - z.y * z.y),(z.y * z.x *2))+c ;"
"}"
"float iterate(vec2 uv,vec4 inputcolor){"
"	  vec2 z, c;"
"c=uv;"
"	    int i;"
"	    z = c;"
" float maxIter=50;" 
"float mean=0.0;"
"	    for(i=0; i<maxIter; i++) {"  
" int modValue=i%modStep;"
"	        if(modValue<(modStep/3.0)){"
"	        	z=kali(z,seed1.xy,seed1.z,seed1.w);"
"	        }else  if(modValue<(modStep/3.0)*2){"
"	        	z=kali(z,seed2.xy,seed1.z,seed1.w);"
"	        }else{"
"	        	z=kali(z,seed3.xy,seed1.z,seed1.w);"
"	        }"
"z+=inputcolor.xy*imageBlend;"
" mean+=length(z);"
"	        if(dot(z,z) > 16.0){"
"	break;"
"}"
"	    }"
"	mean/=float(maxIter);"
"		float ci =  1.0 - log(.5*log(mean ));"
"return ci;"
"	    return length(z) ;"
"}"
"mat2 rot2d(float a) {"
"	return mat2(cos(a),sin(a),-sin(a),cos(a));"
"}"
"const vec4 grayScaleWeights = vec4(0.30, 0.59, 0.11, 0.0);"
"void main() "
"{"
"   vec2 texCoord = gl_TexCoord[1].st;" //lookup input color
"   vec4 c = texture2D(inputTexture, texCoord);"
"	texCoord=(texCoord/maxCoords)*2-1;"
"   texCoord.y*=aspectRatio;" 
"   texCoord*=location.z;"
"   texCoord+=location.xy;"
"   texCoord=texCoord*rot2d(location.w);"
" float itercount=iterate(texCoord*2,c);"
"   vec4 col_out=vec4(hsv2rgb(vec3(itercount*hsvScale.x,(itercount*hsvScale.y),(itercount*hsvScale.z))),1) ;"    
"  col_out.w=1;"
"   gl_FragColor = mix(col_out,c,imageBlend);"
"}";


static const unsigned char g_heatTextureData[] = {0,0,255,0,3,253,0,5,251,0,7,248,0,9,245,0,12,243,0,16,240,0,18,237,0,21,234,0,25,230,0,28,227,0,32,224,0,36,219,0,40,215,0,43,211,0,47,207,0,52,203,0,56,199,0,60,195,0,65,190,0,70,186,0,74,181,0,79,176,0,83,172,0,88,167,0,93,162,0,97,157,0,103,152,0,108,147,0,113,142,0,117,137,0,122,132,0,127,128,0,132,122,0,138,118,0,143,113,0,148,107,0,152,103,0,158,98,0,163,93,0,167,88,0,171,84,0,176,79,0,181,74,0,185,69,0,190,65,0,195,60,0,198,56,0,203,52,0,207,47,0,211,44,0,216,39,0,220,36,0,223,32,0,227,28,0,230,25,0,234,21,0,236,18,0,240,15,0,243,12,0,246,9,0,249,6,0,251,5,0,253,2,0,255,0,2,255,0,4,255,0,7,255,0,9,255,0,12,255,0,15,255,0,18,255,0,21,255,0,25,255,0,28,255,0,32,255,0,36,255,0,40,255,0,44,255,0,48,255,0,52,255,0,56,255,0,61,255,0,65,255,0,70,255,0,74,255,0,79,255,0,83,255,0,88,255,0,93,255,0,98,255,0,103,255,0,108,255,0,113,255,0,117,255,0,123,255,0,127,255,0,133,255,0,138,255,0,142,255,0,147,255,0,152,255,0,158,255,0,162,255,0,166,255,0,171,255,0,177,255,0,181,255,0,186,255,0,190,255,0,194,255,0,199,255,0,204,255,0,207,255,0,211,255,0,216,255,0,219,255,0,224,255,0,226,255,0,230,255,0,233,255,0,237,255,0,240,255,0,243,255,0,245,255,0,248,255,0,250,255,0,253,255,0,255,255,0,255,254,0,255,253,0,255,252,0,255,251,0,255,250,0,255,248,0,255,246,0,255,245,0,255,244,0,255,242,0,255,240,0,255,239,0,255,236,0,255,235,0,255,233,0,255,231,0,255,229,0,255,227,0,255,225,0,255,223,0,255,221,0,255,219,0,255,216,0,255,214,0,255,212,0,255,210,0,255,208,0,255,205,0,255,203,0,255,201,0,255,198,0,255,196,0,255,194,0,255,192,0,255,189,0,255,187,0,255,185,0,255,182,0,255,180,0,255,177,0,255,175,0,255,173,0,255,171,0,255,169,0,255,167,0,255,164,0,255,162,0,255,160,0,255,158,0,255,156,0,255,154,0,255,152,0,255,150,0,255,148,0,255,146,0,255,144,0,255,143,0,255,141,0,255,139,0,255,138,0,255,136,0,255,135,0,255,134,0,255,133,0,255,132,0,255,130,0,255,129,0,255,128,0,255,126,0,255,126,0,255,125,0,255,123,0,255,121,0,255,120,0,255,118,0,255,116,0,255,115,0,255,112,0,255,111,0,255,109,0,255,107,0,255,104,0,255,102,0,255,100,0,255,97,0,255,95,0,255,93,0,255,90,0,255,87,0,255,85,0,255,82,0,255,80,0,255,78,0,255,75,0,255,72,0,255,69,0,255,67,0,255,64,0,255,61,0,255,59,0,255,56,0,255,53,0,255,51,0,255,48,0,255,45,0,255,43,0,255,41,0,255,38,0,255,36,0,255,33,0,255,31,0,255,28,0,255,26,0,255,24,0,255,22,0,255,19,0,255,17,0,255,16,0,255,13,0,255,12,0,255,10,0,255,9,0,255,6,0,255,5,0,255,3,0,255,2,0,255,1,0};
static const int g_heatTextureWidth = 256;

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLSineWave2D::FFGLSineWave2D()
:CFreeFrameGLPlugin(),
 m_initResources(1),
 m_heatTextureId(0),
 m_inputTextureLocation(-1),
 m_heatTextureLocation(-1),
 m_maxCoordsLocation(-1)
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);
	SetTimeSupported(true);
	// Parameters
	// Param swizzle, somehow BPM is not accepted ... right now .. ;) 
	SetParamInfo(FFPARAM_IMAGE_BLEND, "ImageBlend", FF_TYPE_STANDARD, 0.5f);  
	SetParamInfo(FFPARAM_MODSTEP, "HybridStep(*25)", FF_TYPE_STANDARD, 0.03f);  
	SetParamInfo(FFPARAM_ANGLE, "Angle", FF_TYPE_STANDARD, 0.0f);  
	SetParamInfo(FFPARAM_SCALE, "Scale", FF_TYPE_STANDARD, 1.0f);  
	SetParamInfo(FFPARAM_CENTERX, "CenterX", FF_TYPE_STANDARD, 0.50f);  
	SetParamInfo(FFPARAM_CENTERY, "CenterY", FF_TYPE_STANDARD, 0.50f);  

	SetParamInfo(FFPARAM_SEED1_POSX, "Seed1X", FF_TYPE_STANDARD, 0.55f); 
	SetParamInfo(FFPARAM_SEED1_POSY, "Seed1Y", FF_TYPE_STANDARD,0.55f); 
	SetParamInfo(FFPARAM_SEED1_RADIUS, "Seed1Radius", FF_TYPE_STANDARD, 0.1f); 
	SetParamInfo(FFPARAM_SEED1_SPEED, "Seed1Speed", FF_TYPE_STANDARD,0.25f); 

	SetParamInfo(FFPARAM_SEED1_PAR1, "Seed1Param1", FF_TYPE_STANDARD, 0.1f); 
	SetParamInfo(FFPARAM_SEED1_PAR2, "Seed1Param2", FF_TYPE_STANDARD,0.5f); 

	
	SetParamInfo(FFPARAM_SEED2_POSX, "Seed2X", FF_TYPE_STANDARD, 0.75f); 
	SetParamInfo(FFPARAM_SEED2_POSY, "Seed2Y", FF_TYPE_STANDARD,0.65f); 
	SetParamInfo(FFPARAM_SEED2_RADIUS, "Seed2Radius", FF_TYPE_STANDARD, 0.2f); 
	SetParamInfo(FFPARAM_SEED2_SPEED, "Seed2Speed", FF_TYPE_STANDARD,0.25f);  
	 
	SetParamInfo(FFPARAM_SEED3_POSX, "Seed3X", FF_TYPE_STANDARD, 0.25f); 
	SetParamInfo(FFPARAM_SEED3_POSY, "Seed3Y", FF_TYPE_STANDARD,0.15f); 
	SetParamInfo(FFPARAM_SEED3_RADIUS, "Seed3Radius", FF_TYPE_STANDARD, 0.25f); 
	SetParamInfo(FFPARAM_SEED3_SPEED, "Seed3Speed", FF_TYPE_STANDARD,0.75f);  

	
	SetParamInfo(FFPARAM_SCALE_HUE, "ScaleHue", FF_TYPE_STANDARD,1.0f);  
	SetParamInfo(FFPARAM_SCALE_SATURATION, "ScaleSaturation", FF_TYPE_STANDARD,0.5f);  
	SetParamInfo(FFPARAM_SCALE_VALUE, "ScaleValue", FF_TYPE_STANDARD,0.5f);  
	 

	SetParamInfo(FFPARAM_RESYNC, "Reset", FF_TYPE_EVENT, 0.0f); 
	//SetParamInfo(FFPARAM_BPM, "BPM(*240)", FF_TYPE_STANDARD, 0.5f);  
	seedAnim1.radius=0.5;
	seedAnim1.speed=1;
	seedAnim1.center.x=0;
	seedAnim1.center.y=0;
	seedAnim1.count=0;
	seedAnim1.par1=0;
	seedAnim1.par2=0;
	m_modStep=1;
	imageblendValue=1;
	scaleHue=5;
scaleSaturation=5;
scaleValue=5;

	seedAnim2.radius=0.5;
	seedAnim2.speed=1;
	seedAnim2.center.x=0;
	seedAnim2.center.y=0;
	seedAnim2.count=0;
	seedAnim2.par1=0;
	seedAnim2.par2=0;

	seedAnim3.radius=.5;
	seedAnim3.speed=1;
	seedAnim3.center.x=0.4;
	seedAnim3.center.y=0;
	seedAnim3.count=0;
	seedAnim3.par1=0;
	seedAnim3.par2=0;

	fractalAngle=0.25;
	fractalScale=1;
	fractalCenter.x=0.5;
	fractalCenter.y=0.5;
	 

	timeCounter=0;  
	GetSystemTime(&m_SystemTimeLastFrame); 
	// Initialise System Time Based Time counting	 
    GetSystemTime(&m_SystemTimeStart);

	m_Time=0;
}

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
  m_shader.SetExtensions(&m_extensions);
  m_shader.Compile(vertexShaderCode,fragmentShaderCode);
 
  //activate our shader
  m_shader.BindShader();
    
  //to assign values to parameters in the shader, we have to lookup
  //the "location" of each value.. then call one of the glUniform* methods
  //to assign a value
  m_inputTextureLocation = m_shader.FindUniform("inputTexture");
  m_heatTextureLocation = m_shader.FindUniform("heatTexture");
  m_maxCoordsLocation = m_shader.FindUniform("maxCoords"); 
  m_timeLocation = m_shader.FindUniform("time"); 
  m_aspectLocation= m_shader.FindUniform("aspectRatio");
 
  m_seed1Location=m_shader.FindUniform("seed1");
  m_seed2Location=m_shader.FindUniform("seed2");
  m_seed3Location=m_shader.FindUniform("seed3");
  m_locationLocation=m_shader.FindUniform("location");
  m_HSVScaleLocation=m_shader.FindUniform("hsvScale");
  m_modstepLocation=m_shader.FindUniform("modStep");
  m_imageblendLocation=m_shader.FindUniform("imageBlend");
  //the 0 means that the 'heatTexture' in
  //the shader will use the texture bound to GL texture unit 0
  m_extensions.glUniform1iARB(m_heatTextureLocation, 0);
    
  //the 1 means that the 'inputTexture' in
  //the shader will use the texture bound to GL texture unit 1
  m_extensions.glUniform1iARB(m_inputTextureLocation, 1);
    
  m_shader.UnbindShader();

  //create/upload the heat texture on texture unit 0
  //(which should already be the active unit)
  glGenTextures( 1, &m_heatTextureId );
  glBindTexture(GL_TEXTURE_1D, m_heatTextureId);
    
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
  glTexImage1D(
    GL_TEXTURE_1D,
    0, //0 means the base mipmap level
    3, //# of color components in the texture
    g_heatTextureWidth, //width
    0, //no border,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    g_heatTextureData);  

  return FF_SUCCESS;
}

DWORD FFGLSineWave2D::DeInitGL()
{
  m_shader.FreeGLResources();

  if (m_heatTextureId)
  {
    glDeleteTextures(1, &m_heatTextureId);
    m_heatTextureId = 0;
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

double FFGLSineWave2D::GetTimeInSecondsSinceStart(){
	 // Update Time Based on Systemtime and initial stored system time...
   SYSTEMTIME st;    
   GetSystemTime(&st); 

   double delta=systemTimeToDouble(&st)-systemTimeToDouble(&m_SystemTimeLastFrame);

   // increase time counter;
   timeCounter+=delta;

   // increase sin values 
    
   seedAnim1.count+=delta*((seedAnim1.speed*2.0f-1.0f)*PI);
   seedAnim2.count+=delta*((seedAnim2.speed*2.0f-1.0f)*PI);
   seedAnim3.count+=delta*((seedAnim3.speed*2.0f-1.0f)*PI);
   
   m_SystemTimeLastFrame=st;

//   return systemTimeToDouble(&st);
   return timeCounter;

}

DWORD FFGLSineWave2D::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<1) return FF_FAIL;

  if (pGL->inputTextures[0]==NULL) return FF_FAIL;
  
  //activate our shader
  m_shader.BindShader();
    
  //bind the heat texture to texture unit 0
  glBindTexture(GL_TEXTURE_1D, m_heatTextureId);

	FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

  //assign the maxCoords value in the shader
  //(glUniform2f assigns to a vec2)
  m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
   
  
   // Update timers
  GetTimeInSecondsSinceStart();

  // Calculate Seeds based on sine 
  float tempseed1real=(seedAnim1.center.x*2-1)+ sin(seedAnim1.count)*seedAnim1.radius;  
  float tempseed1imag=(seedAnim1.center.y*2-1)+ cos(seedAnim1.count)*seedAnim1.radius;
  
  m_extensions.glUniform4fARB(m_seed1Location,tempseed1real,tempseed1imag,seedAnim1.par1*4-2,seedAnim1.par2);
  
  float tempseed2real=(seedAnim2.center.x*2-1)+ sin(seedAnim2.count)*seedAnim2.radius;  
  float tempseed2imag=(seedAnim2.center.y*2-1)+ cos(seedAnim2.count)*seedAnim2.radius;
  m_extensions.glUniform4fARB(m_seed2Location,tempseed2real,tempseed2imag,seedAnim2.par1*4-2,seedAnim2.par2);

  float tempseed3real=(seedAnim3.center.x*2-1)+ sin(seedAnim3.count)*seedAnim3.radius;  
  float tempseed3imag=(seedAnim3.center.y*2-1)+ cos(seedAnim3.count)*seedAnim3.radius;
  m_extensions.glUniform4fARB(m_seed3Location,tempseed3real,tempseed3imag,seedAnim3.par1*4-2,seedAnim3.par2);

  //assign the heatAmount
 // m_extensions.glUniform1fARB(m_heatAmountLocation, 2.0*m_Heat);
  //assign the time
 // m_extensions.glUniform1fARB(m_timeLocation,(float)GetTimeInSecondsSinceStart()); 
  m_extensions.glUniform1fARB(m_aspectLocation,((float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width));
  m_extensions.glUniform4fARB(m_locationLocation,fractalCenter.x*2-1,fractalCenter.y*2-1,fractalScale,fractalAngle*PI*2);
  m_extensions.glUniform3fARB(m_HSVScaleLocation,scaleHue,scaleSaturation,scaleValue);
  m_extensions.glUniform1iARB(m_modstepLocation,(int)(m_modStep*10));
  m_extensions.glUniform1fARB(m_imageblendLocation,imageblendValue);


  glClearColor(0,0,0,0);

  //activate texture unit 1 and bind the input texture
  m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);
    
  //draw the quad that will be painted by the shader/textures
  //note that we are sending texture coordinates to texture unit 1..
  //the vertex shader and fragment shader refer to this when querying for
  //texture coordinates of the inputTexture
	glBegin(GL_QUADS);

	//lower left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);
	glVertex2f(-1,-1);

	//upper left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0, maxCoords.t);
	glVertex2f(-1,1);

	//upper right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, maxCoords.t);
	glVertex2f(1,1);

	//lower right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, 0);
	glVertex2f(1,-1);
	glEnd();

  //unbind the input texture
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //switch to texture unit 0 and unbind the 1D heat texture
  m_extensions.glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, 0);
  
  //unbind the shader
  m_shader.UnbindShader();
  
  return FF_SUCCESS;
}

DWORD FFGLSineWave2D::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {
		 
 
	case FFPARAM_SEED1_POSX:
		*((float *)(unsigned)&dwRet) = seedAnim1.center.x;
		return dwRet;  
	case FFPARAM_SEED1_POSY:
		*((float *)(unsigned)&dwRet) = seedAnim1.center.y;
		return dwRet;  
	default:
		return FF_FAIL;
	}
}

DWORD FFGLSineWave2D::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {
		
		switch (pParam->ParameterNumber) {
  
		case FFPARAM_SEED1_POSX:
			seedAnim1.center.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED1_POSY:
			seedAnim1.center.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED1_RADIUS:
			seedAnim1.radius= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED1_SPEED:
			seedAnim1.speed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	  	case FFPARAM_SEED2_POSX:
			seedAnim2.center.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED2_POSY:
			seedAnim2.center.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED2_RADIUS:
			seedAnim2.radius= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED2_SPEED:
			seedAnim2.speed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	   
		 	case FFPARAM_SEED3_POSX:
			seedAnim3.center.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	case FFPARAM_SEED3_POSY:
			seedAnim3.center.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED3_RADIUS:
			seedAnim3.radius= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SEED3_SPEED:
			seedAnim3.speed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
	   

		case FFPARAM_ANGLE:	
			fractalAngle= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SCALE:	
			fractalScale= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_CENTERX:	
			fractalCenter.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_CENTERY:	
			fractalCenter.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_SEED1_PAR1:	
			seedAnim1.par1= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_SEED1_PAR2:	
			seedAnim1.par2= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
	   
			
		case FFPARAM_MODSTEP:	
			m_modStep= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
			
		case FFPARAM_SCALE_HUE:	
			scaleHue= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
			
		case FFPARAM_SCALE_SATURATION:	
			scaleSaturation= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
			
		case FFPARAM_SCALE_VALUE:	
			scaleValue= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 

		case FFPARAM_IMAGE_BLEND:	
			imageblendValue= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 

	case FFPARAM_RESYNC:
		float temp;
		
		temp=*((float *)(unsigned)&(pParam->NewParameterValue));
		if(temp==1.0f){
			timeCounter=0; 
		}
			break;

		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
