#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"
#define PI 3.14159265359
  

#define FFPARAM_HUE_SCALE   (0) 
#define FFPARAM_SAT_SCALE   (1) 
#define FFPARAM_VALUE_SCALE   (2) 
#define FFPARAM_ANGLE   (3) 
#define FFPARAM_SCALE   (4) 
#define FFPARAM_SPEEDZ     (5) 
#define FFPARAM_SPEEDX     (6) 
#define FFPARAM_PERSISTENCE     (7) 
#define FFPARAM_RESYNC     (8) 


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
//   .... <- max length
	"S009",								// Plugin unique ID											
//   ................ <- max length
	"sOm-Noise",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_SOURCE,						// Plugin type
	"sOm-Noise",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - Mandelbrot. " // About
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
"uniform float BPM;"
"uniform float dotcount;"
"uniform float dotsize;"
"uniform float beatdotsize;"
"uniform float beatdotrange;"
"uniform vec3 sineWave1;"
"uniform vec3 sineWave2;"
"uniform  vec3 sineWave3;"
"uniform vec3 sineWave1Y;"
"uniform vec3 sineWave2Y;"
"uniform  vec3 sineWave3Y;"
"uniform vec3 angles;"
"uniform vec4 seed1;"
"uniform vec4 seed2;"
"uniform vec4 seed3;"
"uniform vec4 location;"
"uniform vec4 noiseparams;"
"uniform vec3 hsvScale;"
"uniform float persistence;"
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
"int LFSR_Rand_Gen(in int n)"
"{"
"  n = (n << 13) ^ n;"
"  return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;"
"}"
"float LFSR_Rand_Gen_f( in int n )"
"{"
"  return float(LFSR_Rand_Gen(n));"
"}"
"float noise3f(in vec3 p)"
"{"
"  ivec3 ip = ivec3(floor(p));"
"  vec3 u = fract(p);"
"  u = u*u*(3.0-2.0*u);"
""
"  int n = ip.x + ip.y*57 + ip.z*113;"
"  float res = mix(mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*0)),"
"                          LFSR_Rand_Gen_f(n+(1+57*0+113*0)),u.x),"
"                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*0)),"
"                          LFSR_Rand_Gen_f(n+(1+57*1+113*0)),u.x),u.y),"
"                 mix(mix(LFSR_Rand_Gen_f(n+(0+57*0+113*1)),"
"                          LFSR_Rand_Gen_f(n+(1+57*0+113*1)),u.x),"
"                      mix(LFSR_Rand_Gen_f(n+(0+57*1+113*1)),"
"                          LFSR_Rand_Gen_f(n+(1+57*1+113*1)),u.x),u.y),u.z);"
"  return 1.0 - res*(1.0/1073741824.0);"
"}"
"float noise3x(in vec2 p){"
"	return noise3f(vec3(p,seed1.y));"
"}"
"vec2 mandelbrot(vec2 z, vec2 c,float par1,float par2){"
"   return vec2((z.x * z.x - z.y * z.y),(z.y * z.x *2))+c ;"
"}"
"float iterate(vec2 uv){"
"	  vec2 z, c;"
"c=uv;"
"	    int i;"
"	    z = c;"
" float maxIter=125;"
"	    for(i=0; i<maxIter; i++) {" 
"	        if(i%3<1){"
"	        	z=kali(z,seed1.xy,seed1.z,seed1.w);"
"	        }else  if(i%3<2){"
"	        	z=mandelbrot(z,seed2.xy,seed2.z,seed2.w);"
"	        }else{"
"	        	z=kali(z,seed3.xy,seed3.z,seed3.w);"
"	        }"
"	        if(dot(z,z) > 16.0) return i/maxIter;"
"	    }"
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
"   texCoord=texCoord*rot2d(seed1.w);"
"   texCoord*=seed1.z;"
" texCoord.x+=seed1.x;"
" vec4 val;"
"int octavecount=8;"
"   vec4 col_out=iterate(texCoord*2) ;"    
"	for(int i =1;i<octavecount;i++){"
""
"		val.x +=noise3x(texCoord*pow(2,i))*pow(persistence,i) ;"
"	}"
"	val.x=val.x*0.5+0.5;"
"  col_out=vec4(hsv2rgb(vec3(val.x*hsvScale.x,1.0-val.x*hsvScale.y,1.0-val.x*hsvScale.z)),1);"
"   gl_FragColor = col_out;"
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
	SetParamInfo(FFPARAM_HUE_SCALE, "HueScale", FF_TYPE_STANDARD, 1.0f);  
	SetParamInfo(FFPARAM_SAT_SCALE, "SatScale", FF_TYPE_STANDARD, 0.0f);  
	SetParamInfo(FFPARAM_VALUE_SCALE, "ValueScale", FF_TYPE_STANDARD, 0.0f);  
	SetParamInfo(FFPARAM_ANGLE, "Angle", FF_TYPE_STANDARD, 0.0f);  
	SetParamInfo(FFPARAM_SCALE, "Scale", FF_TYPE_STANDARD, 1.0f);  
	SetParamInfo(FFPARAM_SPEEDX, "SpeedX", FF_TYPE_STANDARD, 0.50f);  
	SetParamInfo(FFPARAM_SPEEDZ, "SpeedZ", FF_TYPE_STANDARD, 0.50f);  

	SetParamInfo(FFPARAM_PERSISTENCE, "Persistence", FF_TYPE_STANDARD, 1.00f);  

	 
	 

	SetParamInfo(FFPARAM_RESYNC, "Reset", FF_TYPE_EVENT, 0.0f); 
	//SetParamInfo(FFPARAM_BPM, "BPM(*240)", FF_TYPE_STANDARD, 0.5f);  
	seedAnim1.angle=0;
	seedAnim1.countx=0;
	seedAnim1.countz=0;
	seedAnim1.persistence=1;
	seedAnim1.scale=1;
	seedAnim1.speedx=1;
seedAnim1.speedz=1;

m_hueScale=1;
m_satScale=0;
m_valueScale=0;


	   
	 

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
  m_locationLocation=m_shader.FindUniform("location");
  m_persistenceLocation=m_shader.FindUniform("persistence");
  m_hueScaleLocation=m_shader.FindUniform("hsvScale");
  
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
    
   seedAnim1.countx+=delta*((seedAnim1.speedx*4.0f-2.0f)); 
   seedAnim1.countz+=delta*((seedAnim1.speedz*4.0f-2.0f)); 
   
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
  
  
  
   
   
   
  //assign the heatAmount
 // m_extensions.glUniform1fARB(m_heatAmountLocation, 2.0*m_Heat);
  //assign the time
  m_extensions.glUniform1fARB(m_timeLocation,(float)GetTimeInSecondsSinceStart()); 
  m_extensions.glUniform1fARB(m_persistenceLocation,(float)seedAnim1.persistence); 
  m_extensions.glUniform1fARB(m_aspectLocation,((float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width));
  m_extensions.glUniform4fARB(m_locationLocation,seedAnim1.countx,seedAnim1.countz,seedAnim1.scale*10,seedAnim1.angle*PI*2);
  m_extensions.glUniform4fARB(m_seed1Location,seedAnim1.countx,seedAnim1.countz,seedAnim1.scale*10,seedAnim1.angle*PI*2);
  m_extensions.glUniform3fARB(m_hueScaleLocation,m_hueScale,m_satScale,m_valueScale);


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
		 
  
	default:
		return FF_FAIL;
	}
}

DWORD FFGLSineWave2D::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {
		
		switch (pParam->ParameterNumber) {
   
		case FFPARAM_ANGLE:	
			seedAnim1.angle= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_SCALE:	
			seedAnim1.scale= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_SPEEDX:	
			seedAnim1.speedx= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_SPEEDZ:	
			seedAnim1.speedz= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_PERSISTENCE:	
			seedAnim1.persistence= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_HUE_SCALE:	
			m_hueScale= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_SAT_SCALE:	
			m_satScale= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break; 
		case FFPARAM_VALUE_SCALE:	
			m_valueScale= *((float *)(unsigned)&(pParam->NewParameterValue)); 
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
