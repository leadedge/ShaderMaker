#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"
#define PI 3.14159265359
  

#define FFPARAM_CIRCLE1WIDTH   (0) 
#define FFPARAM_CIRCLE2WIDTH   (1) 
#define FFPARAM_CIRCLE3WIDTH   (2) 
#define FFPARAM_CIRCLE4WIDTH   (3)  
#define FFPARAM_SPEED   (4)  

#define FFPARAM_CENTERX   (5)  
#define FFPARAM_CENTERY  (6)  

#define FFPARAM_SCALE   (7)  
#define FFPARAM_FUZZYNESS   (8)  
#define FFPARAM_EDGINESS   (9)  


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
//   .... <- max length
	"S014",								// Plugin unique ID											
//   ................ <- max length
	"sOm-Circles",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_SOURCE,						// Plugin type
	"sOm-Concentric Circles",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - Bokeh. " // About
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
"uniform sampler2D iChannel0;"
"uniform sampler1D heatTexture;"
"uniform vec2 iResolution;"
"uniform vec2 maxCoords;" 
"uniform float iGlobalTime;" 
"uniform float bokehAmount;"
"uniform vec4 circleWidths;" 
"uniform vec3 hsvScale;" //! slider[(0,0,0), (1,1,1), (1,1,1)]
"uniform vec2 center;" //! slider[(-2,-2), (0,0), (2,2)]
"uniform float scale;" //! slider[0, 1, 10]
"uniform float fuzzyness;" //! slider[0, .1, 1]
"uniform float edge;"
"const int Iterations=24;"
"const float Scale=1.27;"
"const vec3 Julia=vec3(-1.2,-1.95,-.6);" 
"uniform vec4 RotAxisAndAngle;"
"uniform float Speed;"
"const float Amplitude=0.45;"
"const float detail=.0125;"
"const vec3 lightdir=-vec3(-0.5,1.,-0.5);"
""
"mat2 rot;"
""
"float de(vec3 p); "
""
"vec3 normal(vec3 p) {"
"vec3 e = vec3(0.0,detail,0.0);"
""
"return normalize(vec3("
"de(p+e.yxx)-de(p-e.yxx),"
"de(p+e.xyx)-de(p-e.xyx),"
"de(p+e.xxy)-de(p-e.xxy)"
")"
");"
"}"
""
"float softshadow( in vec3 ro, in vec3 rd, float mint, float k )"
"{"
"    float res = 1.0;"
"    float t = mint;"
"    for( int i=0; i<48; i++ )"
"    {"
"        float h = de(ro + rd*t);"
"h = max( h, 0.0 );"
"        res = min( res, k*h/t );"
"        t += clamp( h, 0.01, 0.5 );"
"    }"
"    return clamp(res,0.0,1.0);"
"}"
""
"float light(in vec3 p, in vec3 dir) {"
"vec3 ldir=normalize(lightdir);"
"vec3 n=normal(p);"
"float sh=softshadow(p,-ldir,1.,20.);"
"float diff=max(0.,dot(ldir,-n));"
"vec3 r = reflect(ldir,n);"
"float spec=max(0.,dot(dir,-r));"
"return diff*sh+pow(spec,30.)*.5*sh+.15*max(0.,dot(normalize(dir),-n));"
"}"
""
"float raymarch(in vec3 from, in vec3 dir)"
"{"
"float st,d=1.0,col,totdist=st=0.;"
"vec3 p;"
"for (int i=0; i<70; i++) {"
"if (d>detail && totdist<50.)"
"{"
"p=from+totdist*dir;"
"d=de(p);"
"totdist+=d;"
"}"
"}"
"float backg=0;"
"if (d<detail) {"
"col=0.5+light(p-detail*dir, dir); "
"} else { "
"col=backg;"
"}"
"col = mix(col, backg, 1.0-exp(-.000025*pow(totdist,3.5)));"
"return col;"
"}"
""
""
"mat3  rotationMatrix3(vec3 v, float angle)"
"{"
"float c = cos(radians(angle));"
"float s = sin(radians(angle));"
""
"return mat3(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y,"
"(1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x,"
"(1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z"
");"
"}"
""
""
"vec3 hsv2rgb(vec3 c)"
"{"
"    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);"
"    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);"
"    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);"
"}"
"float de(vec3 p) {"
"p=p.zxy;"
"float a=1.5+sin(iGlobalTime*.5)*.5;"
"p.xy=p.xy*mat2(cos(a),sin(a),-sin(a),cos(a));"
"p.x*=.75;"
"float time=Speed;"
"vec3 ani;"
"ani=vec3(sin(1.),sin(time*.133),cos(time*.2))*Amplitude;"
"p+=sin(p*3.+time*6.)*.04;"
"mat3 rot = rotationMatrix3(normalize(RotAxisAndAngle.yzw), RotAxisAndAngle.x);"
"vec3 pp=p;"
"float l;"
"for (int i=0; i<Iterations; i++) {"
"p.xy=abs(p.xy);"
"p=p*Scale+Julia;"
"p*=rot;"
"l=length(p);"
"}"
"return l*pow(Scale, -float(Iterations))*.9;"
"}"
""
""
"float circleRing(vec2 center, float radius, float width) {"
"	float result = 1;"
"	float l = length(center);"
""
"	l -= radius;"
"	if (l < -width)"
"		result = 0;"
"	if (l > width)"
"		result = 0;"
""
"	return result;"
"}"

"float lengthWrap(vec2 p){"
"p=abs(p);"
"	float n=edge;"
"	return pow(pow(p.x,n)+pow(p.y,n),(1.0/n));"
""
"}"
"void main(void)"
"{"
"float t=iGlobalTime*.3;"
"vec2 uv = gl_FragCoord.xy / iResolution.xy*2.-1.;"
"uv*=scale;"
"uv+=center*scale;"
"	float modulusRange=mod(Speed+lengthWrap(uv),1);"
"	float modulusRange2=mod(modulusRange,0.25);"
"	int circleIndex=int(modulusRange*4);"
"	float val=(abs(modulusRange2-0.125)*8) ;"
"	float insideCircle=clamp(sign(val-circleWidths[circleIndex]),0,1);"
"	float insideFuzz=clamp(sign(val-(1-fuzzyness)*circleWidths[circleIndex]),0,1);"
"	float remeain=val-(1-fuzzyness)*circleWidths[circleIndex];"
"	float fuzzval=mix(1,0,remeain/(circleWidths[circleIndex]*fuzzyness));"
"	val=mix(mix(1,fuzzval,insideFuzz),0,insideCircle);	"
"	gl_FragColor = vec4(val*hsv2rgb(vec3(circleIndex/4.0,1,1)*hsvScale), 1);"
""
"}"
""
"";

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
	SetParamInfo(FFPARAM_CIRCLE1WIDTH, "Circle1", FF_TYPE_STANDARD, 0.1f);   
	SetParamInfo(FFPARAM_CIRCLE2WIDTH, "Circle2", FF_TYPE_STANDARD, 0.1f);   
	SetParamInfo(FFPARAM_CIRCLE3WIDTH, "Circle3", FF_TYPE_STANDARD, 0.1f);   
	SetParamInfo(FFPARAM_CIRCLE4WIDTH, "Circle4", FF_TYPE_STANDARD, 0.1f);   
	SetParamInfo(FFPARAM_SPEED, "Speed", FF_TYPE_STANDARD, 0.5f);   

	SetParamInfo(FFPARAM_CENTERX, "CenterX", FF_TYPE_STANDARD, 0.5f);   
	SetParamInfo(FFPARAM_CENTERY, "CenterY", FF_TYPE_STANDARD, 0.5f);   
	
	SetParamInfo(FFPARAM_EDGINESS, "Edginess", FF_TYPE_STANDARD, 0.2f);   
	SetParamInfo(FFPARAM_SCALE, "Scale", FF_TYPE_STANDARD,0.1f);   
	SetParamInfo(FFPARAM_FUZZYNESS, "Fuzzyness", FF_TYPE_STANDARD, 0.5f);   

	
	
	anim1.speedx=1;
	anim1.countx=0;
	   circle1Width=0.1;
circle2Width=0.1;
circle3Width=0.1;
circle4Width=0.1;
 
	hsvScale.x=1; 
	hsvScale.y=1; 
	hsvScale.z=1; 


	m_fuzzyness=0.5;
	m_scale=1;
	m_edginess=20;
	center.x=.25;
center.y=0.5;


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
  m_inputTextureLocation = m_shader.FindUniform("iChannel0");
  m_heatTextureLocation = m_shader.FindUniform("heatTexture");
  m_maxCoordsLocation = m_shader.FindUniform("maxCoords"); 
  m_timeLocation = m_shader.FindUniform("iGlobalTime"); 
  m_aspectLocation= m_shader.FindUniform("aspectRatio");
 
  m_bokehAmountLocation=m_shader.FindUniform("bokehAmount"); 
  m_bokehRadiusLocation=m_shader.FindUniform("bokehRadius"); 
   m_resolutionLocation=m_shader.FindUniform("iResolution");
  //the 0 means that the 'heatTexture' in
  //the shader will use the texture bound to GL texture unit 0
  m_extensions.glUniform1iARB(m_heatTextureLocation, 0);
  m_speedLocation=m_shader.FindUniform("Speed");
  m_circleWidthsLocation=m_shader.FindUniform("circleWidths");
  m_hsvScaleLocation=m_shader.FindUniform("hsvScale");



	 m_scaleLocation=m_shader.FindUniform("scale");

	 m_fuzzynessLocation=m_shader.FindUniform("fuzzyness");
	 
	 m_edginessLocation=m_shader.FindUniform("edge");
	 m_centerLocation=m_shader.FindUniform("center");
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
    
   anim1.countx+=delta*((anim1.speedx*2.0f-1.0f)); 
   
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
  m_extensions.glUniform1fARB(m_aspectLocation,((float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width));
  m_extensions.glUniform2fARB(m_resolutionLocation,pGL->inputTextures[0]->Width,pGL->inputTextures[0]->Height );
  m_extensions.glUniform1fARB(m_speedLocation,anim1.countx);
  m_extensions.glUniform1fARB(m_edginessLocation,m_edginess*10);
  m_extensions.glUniform1fARB(m_scaleLocation,m_scale*10);
  m_extensions.glUniform1fARB(m_fuzzynessLocation,m_fuzzyness);

  m_extensions.glUniform2fARB(m_centerLocation,center.x*4.0-2.0,center.y*4.0-2.0);

  m_extensions.glUniform4fARB(m_circleWidthsLocation,circle1Width,circle2Width,circle3Width,circle4Width);
  m_extensions.glUniform3fARB(m_hsvScaleLocation,hsvScale.x,hsvScale.y,hsvScale.z);


  glClearColor(0,0,0,0);

  //activate texture unit 1 and bind the input texture
  m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
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
   
		case FFPARAM_CIRCLE1WIDTH:	
			circle1Width= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_CIRCLE2WIDTH:	
			circle2Width= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_CIRCLE3WIDTH:	
			circle3Width= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_CIRCLE4WIDTH:	
			circle4Width= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_SPEED:	
			anim1.speedx= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_CENTERX:	
			center.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CENTERY:	
			center.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_EDGINESS:	
			m_edginess= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_FUZZYNESS:	
			m_fuzzyness= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_SCALE:	
			m_scale= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
