
#include <FFGL.h>
#include <FFGLLib.h>
#include "math.h"
#include "FFGLHeat.h"
#include "glu.h"
#include "glext.h"
#define PI 3.14159265359
  
#define FFPARAM_SINXSPEED1     (0)
#define FFPARAM_SINXSPEED2     (1)
#define FFPARAM_SINXVARIANCE1  (2) 
#define FFPARAM_SINXVARIANCE2  (3) 
#define FFPARAM_SINYSPEED1	  (4)
#define FFPARAM_SINYSPEED2	  (5)
#define FFPARAM_SINYVARIANCE1  (6) 
#define FFPARAM_SINYVARIANCE2  (7) 
#define FFPARAM_AMPLITUDE1  (8) 
#define FFPARAM_AMPLITUDE2  (9) 
#define FFPARAM_AMPLITUDE3  (10) 
#define FFPARAM_AMPLITUDE4  (11) 
#define FFPARAM_NUMDOTS  (12)
#define FFPARAM_DOTSIZE  (13)
#define FFPARAM_BEATDOTSIZE  (14)
#define FFPARAM_BEATDOTRANGE  (15)
#define FFPARAM_ROUNDS  (16)
#define FFPARAM_RESYNC  (17)

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLHeat::CreateInstance,	// Create method
	"S011",								// Plugin unique ID											
	"sOm-PlaneImage",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"sOm-PlaneImage",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis aug2014 - Noise Points. " // About
); 

char *vertexShaderCode =
"uniform sampler2D inputTexture;"
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
"uniform vec4 amplitudes;" 
"float sin3Big(float p){"
"	return sin(p)+sin(p*2)+sin(p*4);"
"}"
"float cos3Big(float p){"
"	return cos(p)+cos(p*2)+cos(p*4);"
"}"
"float decay(float start,float decayRate, float t)"
"{"
"return start*pow(decayRate,t);"
"}"
"void main()"
"{" 
" vec2 texCoord=gl_Vertex.xy;"
"vec4 textcolor=texture2D(inputTexture,texCoord);"
"  gl_Position =gl_ModelViewProjectionMatrix*  "
"             vec4(((gl_Vertex.x/maxCoords.x)*2-1)*amplitudes.x,length((textcolor.xyz))*amplitudes.y"
//"									(sin(sineFactors.x +gl_Vertex.x*sineFactors.y)*amplitudes.x+cos(sineFactors.z+gl_Vertex.x *sineFactors.w)*amplitudes.y+"
//"									sin(sineFactors.x +gl_Vertex.y*sineFactors2.y)*amplitudes.z+cos(sineFactors2.z+gl_Vertex.y *sineFactors2.w)*amplitudes.w)"
",((gl_Vertex.y/maxCoords.x)*2-1)*amplitudes.z"
"									,	 gl_Vertex.w) ;"
"textcolor.w=0.1;"
"  gl_FrontColor =textcolor;"
"    gl_PointSize =dotsize* gl_Position.w*2;"
"}";
/*
sin(gl_Vertex.x*sineFactors.y+sineFactors.x+gl_Vertex.y*sineFactors2.y)*amplitudes.x+"
"sin(gl_Vertex.x*sineFactors.w+sineFactors.z+gl_Vertex.y*sineFactors2.w)*amplitudes.y+"
"					cos(gl_Vertex.y*sineFactors2.y+sineFactors2.z+gl_Vertex.x*sineFactors.y)*amplitudes.z+	cos(gl_Vertex.y*sineFactors2.w+sineFactors2.z+gl_Vertex.x*sineFactors.w)*amplitudes.z,0"
"
*/
	
char *fragmentShaderCode =
"varying vec2 varyingTextureCoordinate;"
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
"uniform vec4 amplitudes;"
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
"float hash( float n ) {"
"	return fract(sin(n)*43758.5453123);"
"}"
"float noise( in float p )"
"{"
"    float i = floor( p );"
"    float f = fract( p );"
"    return  mix( hash( i +0 ), hash( i +  1.0 )  , smoothstep(0,1,f))*2-1;"
"}"
"vec4 alphaBlend(vec4 col1,vec4 col2){"
"vec3 newcolor=col1.xyz+col2.xyz*col2.w;"
" return clamp(vec4(newcolor,col1.w+(1-col1.w)*col2.w),0,1);"
"}"
"vec4 circleBeatLine(vec2 uv, float time, float duration) {"
"vec4 resultColor = vec4(0, 0, 0,0);" 
"float numDots =  dotcount;"
"float PIFACT=(PI/2)/beatdotrange;"
"for (int i = 0; i <numDots; i++) {"
""
"vec3 currentColor = createColor( i/numDots);"
"float fact = (i/numDots)*2-1;"
"			fact=cos(clamp(fact*PIFACT,-PI/2,PI/2));"
"float result = circle(uv + vec2((noise( sineFactors.x + i * sineFactors.y)*amplitudes.x+noise( sineFactors2.x + i * sineFactors2.y)*amplitudes.y) ,"
"                                (noise( sineFactors.z + i * sineFactors.w)*amplitudes.z+noise( sineFactors2.z + i * sineFactors2.w)*amplitudes.w) *aspectRatio),"
"fact * beatdotsize +  dotsize*0.2) ;"
 
"resultColor.xyz  +=currentColor*result ;"
"resultColor.w=clamp(resultColor.w+(1-resultColor.w)*result,0,1);"
"}"
""
"return resultColor;"
""
"}"
"const vec4 grayScaleWeights = vec4(0.30, 0.59, 0.11, 0.0);"
"void main() "
"{"
" gl_FragColor=gl_Color;"
" return ;"
"   vec2 texCoord =gl_PointCoord .st;" //lookup input color
"   vec4 c = texture2D(inputTexture, texCoord);"
"	texCoord=(texCoord/maxCoords)*2-1;"
"   vec4 col_out = c;"
"	float calctime = time*(240*BPM/60);"
"   gl_FragColor.xyzw =circle(texCoord*2-1,1)*gl_Color;" 
"}";


static const unsigned char g_heatTextureData[] = {0,0,255,0,3,253,0,5,251,0,7,248,0,9,245,0,12,243,0,16,240,0,18,237,0,21,234,0,25,230,0,28,227,0,32,224,0,36,219,0,40,215,0,43,211,0,47,207,0,52,203,0,56,199,0,60,195,0,65,190,0,70,186,0,74,181,0,79,176,0,83,172,0,88,167,0,93,162,0,97,157,0,103,152,0,108,147,0,113,142,0,117,137,0,122,132,0,127,128,0,132,122,0,138,118,0,143,113,0,148,107,0,152,103,0,158,98,0,163,93,0,167,88,0,171,84,0,176,79,0,181,74,0,185,69,0,190,65,0,195,60,0,198,56,0,203,52,0,207,47,0,211,44,0,216,39,0,220,36,0,223,32,0,227,28,0,230,25,0,234,21,0,236,18,0,240,15,0,243,12,0,246,9,0,249,6,0,251,5,0,253,2,0,255,0,2,255,0,4,255,0,7,255,0,9,255,0,12,255,0,15,255,0,18,255,0,21,255,0,25,255,0,28,255,0,32,255,0,36,255,0,40,255,0,44,255,0,48,255,0,52,255,0,56,255,0,61,255,0,65,255,0,70,255,0,74,255,0,79,255,0,83,255,0,88,255,0,93,255,0,98,255,0,103,255,0,108,255,0,113,255,0,117,255,0,123,255,0,127,255,0,133,255,0,138,255,0,142,255,0,147,255,0,152,255,0,158,255,0,162,255,0,166,255,0,171,255,0,177,255,0,181,255,0,186,255,0,190,255,0,194,255,0,199,255,0,204,255,0,207,255,0,211,255,0,216,255,0,219,255,0,224,255,0,226,255,0,230,255,0,233,255,0,237,255,0,240,255,0,243,255,0,245,255,0,248,255,0,250,255,0,253,255,0,255,255,0,255,254,0,255,253,0,255,252,0,255,251,0,255,250,0,255,248,0,255,246,0,255,245,0,255,244,0,255,242,0,255,240,0,255,239,0,255,236,0,255,235,0,255,233,0,255,231,0,255,229,0,255,227,0,255,225,0,255,223,0,255,221,0,255,219,0,255,216,0,255,214,0,255,212,0,255,210,0,255,208,0,255,205,0,255,203,0,255,201,0,255,198,0,255,196,0,255,194,0,255,192,0,255,189,0,255,187,0,255,185,0,255,182,0,255,180,0,255,177,0,255,175,0,255,173,0,255,171,0,255,169,0,255,167,0,255,164,0,255,162,0,255,160,0,255,158,0,255,156,0,255,154,0,255,152,0,255,150,0,255,148,0,255,146,0,255,144,0,255,143,0,255,141,0,255,139,0,255,138,0,255,136,0,255,135,0,255,134,0,255,133,0,255,132,0,255,130,0,255,129,0,255,128,0,255,126,0,255,126,0,255,125,0,255,123,0,255,121,0,255,120,0,255,118,0,255,116,0,255,115,0,255,112,0,255,111,0,255,109,0,255,107,0,255,104,0,255,102,0,255,100,0,255,97,0,255,95,0,255,93,0,255,90,0,255,87,0,255,85,0,255,82,0,255,80,0,255,78,0,255,75,0,255,72,0,255,69,0,255,67,0,255,64,0,255,61,0,255,59,0,255,56,0,255,53,0,255,51,0,255,48,0,255,45,0,255,43,0,255,41,0,255,38,0,255,36,0,255,33,0,255,31,0,255,28,0,255,26,0,255,24,0,255,22,0,255,19,0,255,17,0,255,16,0,255,13,0,255,12,0,255,10,0,255,9,0,255,6,0,255,5,0,255,3,0,255,2,0,255,1,0};
static const int g_heatTextureWidth = 256;

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLHeat::FFGLHeat()
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
	SetParamInfo(FFPARAM_SINXSPEED1, "AngleX", FF_TYPE_STANDARD, 0.52f);
	SetParamInfo(FFPARAM_SINXSPEED2, "AngleY", FF_TYPE_STANDARD, 0.22f);
	SetParamInfo(FFPARAM_SINXVARIANCE1, "unused", FF_TYPE_STANDARD, 0.3f); 
	SetParamInfo(FFPARAM_SINYSPEED1, "unused", FF_TYPE_STANDARD, 0.31f);
	SetParamInfo(FFPARAM_SINYSPEED2, "unused", FF_TYPE_STANDARD, 0.6f);
	SetParamInfo(FFPARAM_SINYVARIANCE1, "unused", FF_TYPE_STANDARD, 0.25f); 
	SetParamInfo(FFPARAM_NUMDOTS, "DotCount(*100)", FF_TYPE_STANDARD, 0.5f);
	SetParamInfo(FFPARAM_BEATDOTRANGE, "BeatDotRange", FF_TYPE_STANDARD, 0.0f);
	SetParamInfo(FFPARAM_BEATDOTSIZE, "BeatDotSize", FF_TYPE_STANDARD, 0.0f);
	SetParamInfo(FFPARAM_DOTSIZE, "Dotsize", FF_TYPE_STANDARD, 1.0f);
	SetParamInfo(FFPARAM_AMPLITUDE1, "ScaleWidth", FF_TYPE_STANDARD, 0.05f);
	SetParamInfo(FFPARAM_AMPLITUDE2, "ScaleHeight", FF_TYPE_STANDARD, 0.025f);
	SetParamInfo(FFPARAM_AMPLITUDE3, "AmplitudeY1", FF_TYPE_STANDARD, 0.050f);
	SetParamInfo(FFPARAM_AMPLITUDE4, "AmplitudeY2", FF_TYPE_STANDARD, 0.025f);
	SetParamInfo(FFPARAM_SINXVARIANCE2, "XVariance2", FF_TYPE_STANDARD, 0.4f);
	SetParamInfo(FFPARAM_SINYVARIANCE2, "YVariance2", FF_TYPE_STANDARD, 0.34f);
	SetParamInfo(FFPARAM_ROUNDS, "Rounds(*10)", FF_TYPE_STANDARD, 0.5f);
	SetParamInfo(FFPARAM_RESYNC, "Reset", FF_TYPE_EVENT, 0.0f);
	//SetParamInfo(FFPARAM_BPM, "BPM(*240)", FF_TYPE_STANDARD, 0.5f);  

	timeCounter=0;
	sinCounter1=0;
	sinCounter2=0;
	sinCounter3=0;
	sinCounter4=0;
	
	m_amplitudeX1=1;
	m_amplitudeX2=0.5;
	m_amplitudeY1=1;
	m_amplitudeY2=0.5;
	// vec4(.11, 1.31, 0.17, 1.27)
	m_sinXSpeed=.11f;
	m_sinXSpeed2=1.21f;
	m_sinXVariance=1.31f;
	m_sinXVariance2=2.31f;
	m_sinYSpeed=0.17;
	m_sinYSpeed2=0.27;
	m_sinYVariance=1.27;
	m_sinYVariance2=1.27;
	m_dotcount=0.25;
	m_dotsize=0.5;
	m_rounds=0.5;
	lastrounds=0.5;
	
	m_beatdotrange=0.25;
	m_beatdotsize=0.1;
	GetSystemTime(&m_SystemTimeLastFrame); 
	// Initialise System Time Based Time counting	 
    GetSystemTime(&m_SystemTimeStart);

	m_Time=0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLHeat::InitGL(const FFGLViewportStruct *vp)
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
  m_sineFactorsLocation= m_shader.FindUniform("sineFactors");
  m_sineFactors2Location= m_shader.FindUniform("sineFactors2");
  m_dotcountLocation= m_shader.FindUniform("dotcount");
  m_dotsizeLocation= m_shader.FindUniform("dotsize");
  m_beatdotsizeLocation= m_shader.FindUniform("beatdotsize");
  m_beatdotrangeLocation= m_shader.FindUniform("beatdotrange");
  m_amplitudesLocation= m_shader.FindUniform("amplitudes");

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
    
  createDisplayList();

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

void FFGLHeat::createDisplayList(){
	// create one display list
	m_displayList = glGenLists(1);

	// fill displaylist
	glNewList(m_displayList, GL_COMPILE);
 glBegin(GL_POINTS);

float width=800;
float height=800;

  for(int i=0;i<width;i++){
  for(int j=0;j<height;j++){
	//  m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);
	  glVertex2f(((float)j/height) ,((float)i/width));
  }


  }
  glEnd();
	
glEndList();

}
DWORD FFGLHeat::DeInitGL()
{
  m_shader.FreeGLResources();

  if (m_heatTextureId)
  {
    glDeleteTextures(1, &m_heatTextureId);
    m_heatTextureId = 0;
  } 

  glDeleteLists(m_displayList,1);

  return FF_SUCCESS;
}

DWORD  FFGLHeat::SetTime(double time) {
	m_Time=time;
	return FF_SUCCESS; 
}


double systemTimeToDouble(SYSTEMTIME *time){
	return  ((double)time->wMinute*60000+ (double)time->wSecond*1000+(double)time->wMilliseconds)/(double)1000.0f;
}

double FFGLHeat::GetTimeInSecondsSinceStart(){
	 // Update Time Based on Systemtime and initial stored system time...
   SYSTEMTIME st;    
   GetSystemTime(&st); 

   double delta=systemTimeToDouble(&st)-systemTimeToDouble(&m_SystemTimeLastFrame);

   // increase time counter;
   timeCounter+=delta;

   // increase sin values
   sinCounter1+=delta*(m_sinXSpeed*4-2);
   sinCounter2+=delta*(m_sinXSpeed2*4-2);
   
   sinCounter3+=delta*(m_sinYSpeed*4-2);
   sinCounter4+=delta*(m_sinYSpeed2*4-2);
   
   m_SystemTimeLastFrame=st;

//   return systemTimeToDouble(&st);
   return timeCounter;

}
DWORD FFGLHeat::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<1) return FF_FAIL;
      glMatrixMode( GL_PROJECTION );
    
  if (pGL->inputTextures[0]==NULL) return FF_FAIL;
  if(lastrounds!=m_rounds){
  //rebuild display list
	  glDeleteLists(m_displayList,1);
	  createDisplayList();
  }

  // Set up camera 

  //activate our shader
  m_shader.BindShader();
  glEnable(GL_POINT_SPRITE);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
  glEnable(GL_BLEND);
//glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
//  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  //bind the heat texture to texture unit 0
  glBindTexture(GL_TEXTURE_1D, m_heatTextureId);

	FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

	
	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

  //assign the maxCoords value in the shader
  //(glUniform2f assigns to a vec2)
  m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
 
  int dotcount=m_dotcount*100;
  m_extensions.glUniform4fARB(m_sineFactorsLocation, sinCounter1,(m_sinXVariance*100),sinCounter2,((m_sinYVariance*100) ));
  m_extensions.glUniform4fARB(m_sineFactors2Location, sinCounter3,(m_sinXVariance2*100),sinCounter4,((m_sinYVariance2*100) ));
  m_extensions.glUniform4fARB(m_amplitudesLocation, m_amplitudeX1*4.0-2.0,m_amplitudeX2*4.0-2.0,m_amplitudeY1*4-2,m_amplitudeY2*4-2);
 

  //assign the heatAmount
 // m_extensions.glUniform1fARB(m_heatAmountLocation, 2.0*m_Heat);
  //assign the time
  m_extensions.glUniform1fARB(m_timeLocation,(float)GetTimeInSecondsSinceStart());
  //assign the BPM  
  m_extensions.glUniform1fARB(m_dotsizeLocation,(float)(m_dotsize*2));
  m_extensions.glUniform1fARB(m_beatdotrangeLocation,(float)(m_beatdotrange));
  m_extensions.glUniform1fARB(m_beatdotsizeLocation,(float)(m_beatdotsize));
  m_extensions.glUniform1fARB(m_dotcountLocation,(float)(dotcount));
  //assign the BPM 
  m_extensions.glUniform1fARB(m_aspectLocation,((float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width));

 glClearColor(0,0,0,0);

  //activate texture unit 1 and bind the input texture
  m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);
    

  
  glMatrixMode(GL_PROJECTION);
  
   glLoadIdentity();
  gluPerspective(65,(float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width, 0.011,100);
  gluLookAt(0,2,.1,0,0,0,0,1,0);
 // glRotatef(45,1,0,0);
   glMatrixMode(GL_MODELVIEW);
   
   glLoadIdentity();
 glRotatef(m_sinXSpeed*360,0,1,0);
 glRotatef(m_sinXSpeed2*360,1,0,0);
  //draw the quad that will be painted by the shader/textures
  //note that we are sending texture coordinates to texture unit 1..
  //the vertex shader and fragment shader refer to this when querying for
  //texture coordinates of the inputTexture
  glPointSize(m_dotsize*10);
  glCallList(m_displayList);
/*  glBegin(GL_POINTS);

	
  for(int i=0;i<100000;i++){
	  m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);
	  glVertex2f(((float)i/100.0)*PI,-1);
  }

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
	*/

  //unbind the input texture
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //switch to texture unit 0 and unbind the 1D heat texture
  m_extensions.glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, 0);
  
  //unbind the shader
  m_shader.UnbindShader();
  
  return FF_SUCCESS;
}

DWORD FFGLHeat::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {
		 
 
	case FFPARAM_SINXSPEED1:
		*((float *)(unsigned)&dwRet) = m_sinXSpeed;
		return dwRet; 
	case FFPARAM_SINYSPEED1:
		*((float *)(unsigned)&dwRet) = m_sinYSpeed;
		return dwRet; 
	case FFPARAM_SINXSPEED2:
		*((float *)(unsigned)&dwRet) = m_sinXSpeed;
		return dwRet; 
	case FFPARAM_SINYSPEED2:
		*((float *)(unsigned)&dwRet) = m_sinYSpeed;
		return dwRet; 
	case FFPARAM_SINXVARIANCE1:
		*((float *)(unsigned)&dwRet) = m_sinXVariance;
		return dwRet; 
	case FFPARAM_SINYVARIANCE1:
		*((float *)(unsigned)&dwRet) = m_sinYVariance;
		return dwRet; 
	case FFPARAM_NUMDOTS:
		*((float *)(unsigned)&dwRet) = m_dotcount;
		return dwRet; 
	case FFPARAM_BEATDOTRANGE:
		*((float *)(unsigned)&dwRet) = m_beatdotrange;
		return dwRet; 
	case FFPARAM_BEATDOTSIZE:
		*((float *)(unsigned)&dwRet) = m_beatdotsize;
		return dwRet; 
	default:
		return FF_FAIL;
	}
}

DWORD FFGLHeat::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {
		
		switch (pParam->ParameterNumber) {
  
		case FFPARAM_SINXSPEED1:
			m_sinXSpeed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SINYSPEED1:
			m_sinYSpeed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SINXSPEED2:
			m_sinXSpeed2= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SINYSPEED2:
			m_sinYSpeed2= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;
		case FFPARAM_SINXVARIANCE1:
			m_sinXVariance= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_SINYVARIANCE1:
			m_sinYVariance= *((float *)(unsigned)&(pParam->NewParameterValue));
	 
		case FFPARAM_NUMDOTS:
			m_dotcount= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_DOTSIZE:
			m_dotsize= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_BEATDOTRANGE:
			m_beatdotrange= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_BEATDOTSIZE:
			m_beatdotsize= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_AMPLITUDE1:
				m_amplitudeX1= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_AMPLITUDE2:
				m_amplitudeX2= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_AMPLITUDE3:
				m_amplitudeY1= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_AMPLITUDE4:
				m_amplitudeY2= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_SINYVARIANCE2:
				m_sinYVariance2= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_SINXVARIANCE2:
				m_sinXVariance2= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
			case FFPARAM_ROUNDS:
				m_rounds= *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
	case FFPARAM_RESYNC:
		float temp;
		
		temp=*((float *)(unsigned)&(pParam->NewParameterValue));
		if(temp==1.0f){
			timeCounter=0;
			sinCounter1=0;
			sinCounter2=0;
			sinCounter3=0;
			sinCounter4=0;
		}
			break;

		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
