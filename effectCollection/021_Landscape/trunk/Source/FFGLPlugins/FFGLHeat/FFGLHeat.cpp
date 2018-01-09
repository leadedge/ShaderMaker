#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"
#define PI 3.14159265359
  

#define FFPARAM_SEA_HEIGHT   (0) 
#define FFPARAM_SEA_CHOPPY  (1) 
#define FFPARAM_SEA_FREQUENCY   (2)  
#define FFPARAM_SPEED   (3)   
#define FFPARAM_CAMERA_ANGLE_X   (4)   
#define FFPARAM_CAMERA_ANGLE_Y   (5)   
#define FFPARAM_CAMERA_ANGLE_Z   (6)   
#define FFPARAM_SKY_INTENSITY   (7)   


#define FFPARAM_WATER_BASE_RED   (8)   
#define FFPARAM_WATER_BASE_GREEN  (9)   
#define FFPARAM_WATER_BASE_BLUE   (10)   


#define FFPARAM_WATER_RED   (11)   
#define FFPARAM_WATER_GREEN   (12)   
#define FFPARAM_WATER_BLUE   (13)   


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
//   .... <- max length
	"S020",								// Plugin unique ID											
//   ................ <- max length
	"sOm-SeaScape",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_SOURCE,						// Plugin type
	"sOm-SeaScape",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - SeaScape. " // About
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
""
"uniform vec2      iResolution;           "
"uniform float     iGlobalTime;           " 
""
"const int NUM_STEPS = 40;"
"const float EPSILON= 1e-3;" 
"float EPSILON_NRM= 0.1 / iResolution.x;"
""
"const int ITER_GEOMETRY = 2;"
"const int ITER_FRAGMENT = 4;"
" uniform float SEA_HEIGHT;"
" uniform float SEA_CHOPPY ;"
"uniform vec3 cameraRotation;"
"uniform float seaTime;" 
"uniform float SEA_FREQ ;"
"uniform vec3 SEA_BASE ;"
"uniform vec3 SEA_WATER_COLOR ;"
"uniform float SKY_INTENSITY ;"
""
"mat4 fromEuler(vec3 ang) {"
"vec2 a1 = vec2(sin(ang.x),cos(ang.x));"
"    vec2 a2 = vec2(sin(ang.y),cos(ang.y));"
"    vec2 a3 = vec2(sin(ang.z),cos(ang.z));"
"    mat4 m;"
"    m[0] = vec4(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x,0.0);"
"m[1] = vec4(-a2.y*a1.x,a1.y*a2.y,a2.x,0.0);"
"m[2] = vec4(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y,0.0);"
"m[3] = vec4(0.0,0.0,0.0,1.0);"
"return m;"
"}"
"vec3 rotate(vec3 v, mat4 m) {"
"    return vec3(dot(v,m[0].xyz),dot(v,m[1].xyz),dot(v,m[2].xyz));"
"}"
"float hash( vec2 p ) {"
"float h = dot(p,vec2(127.1,311.7));"
"    return fract(sin(h)*43758.5453123);"
"}"
"float noise( in vec2 p ) {"
"    vec2 i = floor( p );"
"    vec2 f = fract( p );"
"vec2 u = f*f*(3.0-2.0*f);"
"    return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ), "
"                     hash( i + vec2(1.0,0.0) ), u.x),"
"                mix( hash( i + vec2(0.0,1.0) ), "
"                     hash( i + vec2(1.0,1.0) ), u.x), u.y);"
"}"
""
""
"float diffuse(vec3 n,vec3 l,float p) { return pow(dot(n,l) * 0.4 + 0.6,p); }"
"float specular(vec3 n,vec3 l,vec3 e,float s) {    "
"    float nrm = (s + 8.0) / (3.1415 * 8.0);"
"    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;"
"}"
"vec3 sky_color(vec3 e) {"
"    e.y = max(e.y,0.0);"
"    vec3 ret;"
"    ret.x = pow(1.0-e.y,2.0);"
"    ret.y = 1.0-e.y;"
"    ret.z = 0.6+(1.0-e.y)*0.4;"
"    return ret * SKY_INTENSITY;"
"}"
""
"float sea_octave(vec2 uv, float choppy) {"
"    uv += noise(uv);        "
"    vec2 wv = 1.0-abs(sin(uv));"
"    vec2 swv = abs(cos(uv));    "
"    wv = mix(wv,swv,wv);"
"    return pow(1.0-pow(wv.x * wv.y,0.6),choppy);"
"}"
""
"float map(vec3 p) {"
"    float time =seaTime;"
"    float freq = SEA_FREQ;"
"    float amp = SEA_HEIGHT;"
"    float choppy = SEA_CHOPPY;"
"    vec2 uv = p.xz; uv.x *= 0.75;"
"    mat2 m = mat2(1.6,1.2,-1.2,1.6);"
"    "
"    float d, h = 0.0;    "
"    for(int i = 0; i < ITER_GEOMETRY; i++) {        "
"    d = sea_octave((uv+time)*freq,choppy);"
"    d += sea_octave((uv-time)*freq,choppy);"
"        h += d * amp;        "
"    uv *= m; freq *= 1.9; amp *= 0.22;"
"        choppy = mix(choppy,1.0,0.2);"
"    }"
"    return p.y - h;"
"}"
"float map_detailed(vec3 p) {"
"    float time = seaTime;"
"    float freq = SEA_FREQ;"
"    float amp = SEA_HEIGHT;"
"    float choppy = SEA_CHOPPY;"
"    vec2 uv = p.xz; uv.x *= 0.75;"
"    mat2 m = mat2(1.6,1.2,-1.2,1.6);"
"    "
"    float d, h = 0.0;    "
"    for(int i = 0; i < ITER_FRAGMENT; i++) {        "
"    d = sea_octave((uv+time)*freq,choppy);"
"    d += sea_octave((uv-time)*freq,choppy);"
"        h += d * amp;        "
"    uv *= m; freq *= 1.9; amp *= 0.22;"
"        choppy = mix(choppy,1.0,0.2);"
"    }"
"    return p.y - h;"
"}"
""
"vec3 sea_color(in vec3 p, in vec3 n, in vec3 eye, in vec3 dist) {  "
"    float fresnel_o = 1.0 - max(dot(n,-eye),0.0);"
"    float fresnel = pow(fresnel_o,4.0) * 0.65;"
"        "
"    vec3 refl = sky_color(reflect(eye,n));"
"    "
"    vec3 ret = SEA_BASE;    "
"    ret = mix(ret,refl,fresnel);"
"    "
"    float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);"
"    ret += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;"
"    "
"    return ret;"
"}"
""
"vec3 getNormal(vec3 p, float eps) {"
"    vec3 n;"
"    n.x = map_detailed(vec3(p.x+eps,p.y,p.z));"
"    n.y = map_detailed(vec3(p.x,p.y+eps,p.z));"
"    n.z = map_detailed(vec3(p.x,p.y,p.z+eps));"
"    return normalize(n-map_detailed(p));"
"}"
"float hftracing(vec3 ori, vec3 dir, out vec3 p) {"
"    float t = 0.0;"
"    float d = 0.0;"
"    for(int i = 0; i < NUM_STEPS; i++) {"
"        p = ori + dir * t;"
"        d = map(p);"
"        if(d <= 0.0) break;"
"        t += d * 0.9;"
"    } "
"    return d;"
"}"
""
"void main(void) {"
"vec2 uv = gl_FragCoord.xy / iResolution.xy;"
"    uv = uv * 2.0 - 1.0;"
"    uv.x *= iResolution.x / iResolution.y;    "
"    float time = iGlobalTime * 0.3;"
"        " 
"mat4 rot = fromEuler(cameraRotation);"
"    "
"    vec3 ori = vec3(0.0,3.5,0);"
"    vec3 dir = normalize(vec3(uv.xy,-2.0));"
"    dir.z += length(uv) * 0.15;"
"    dir = rotate(normalize(dir),rot);"
"    "
"    vec3 p;"
"    float dens = hftracing(ori,dir,p);"
"    vec3 dist = p - ori;"
"    vec3 n = getNormal(p, dot(dist,dist)*EPSILON_NRM);"
"             "
"    vec3 color = sea_color(p,n,dir,dist);"
"    vec3 light = normalize(vec3(0.0,1.0,0.8));  "
"    color += vec3(diffuse(n,light,80.0) * SEA_WATER_COLOR) * 0.12; "
"    color += vec3(specular(n,light,dir,60.0));  "
"    "
"    color = mix(sky_color(dir),color,pow(smoothstep(0.0,-0.05,dir.y),0.3)); "
"    color = pow(color,vec3(0.75));"
"gl_FragColor = vec4(color,1.0);"
"}"
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
	SetParamInfo(FFPARAM_SEA_HEIGHT, "Sea Height", FF_TYPE_STANDARD, 0.5f);   
	SetParamInfo(FFPARAM_SEA_CHOPPY, "Sea Choppy", FF_TYPE_STANDARD, 0.5f);   
	SetParamInfo(FFPARAM_SEA_FREQUENCY, "Sea Frequency", FF_TYPE_STANDARD, 0.75f);    
	SetParamInfo(FFPARAM_SPEED, "Speed", FF_TYPE_STANDARD, 0.75f);   
	 
	SetParamInfo(FFPARAM_CAMERA_ANGLE_X, "CameraAngleX", FF_TYPE_STANDARD, 0.0f);   
	SetParamInfo(FFPARAM_CAMERA_ANGLE_Y, "CameraAngleY", FF_TYPE_STANDARD, 0.0f);   
	SetParamInfo(FFPARAM_CAMERA_ANGLE_Z, "CameraAngleZ", FF_TYPE_STANDARD, 0.0f);   
	SetParamInfo(FFPARAM_SKY_INTENSITY, "Sky Intensity", FF_TYPE_STANDARD, 1.0f);   
	
	SetParamInfo(FFPARAM_WATER_BASE_RED, "Water Base Red", FF_TYPE_STANDARD, 0.1f);   
	SetParamInfo(FFPARAM_WATER_BASE_GREEN, "Water Base Green", FF_TYPE_STANDARD, 0.19f);   
	SetParamInfo(FFPARAM_WATER_BASE_BLUE, "Water Base Blue", FF_TYPE_STANDARD, 0.22f);   
	 
	SetParamInfo(FFPARAM_WATER_RED, "Water Red", FF_TYPE_STANDARD, 0.8f);   
	SetParamInfo(FFPARAM_WATER_GREEN, "Water Green", FF_TYPE_STANDARD, 0.9f);   
	SetParamInfo(FFPARAM_WATER_BLUE, "Water Blue", FF_TYPE_STANDARD, .6f);   
	 

	s_skyIntensity=1;
	s_seaChoppy=0.5;
s_seaHeight=0.5;
s_seaFrequency=0.5;
cameraRotation.x=0;
cameraRotation.y=0;
cameraRotation.z=0;

	seaAnim.speedx=1;
	seaAnim.countx=0; 
	hsvScale.x=1; 
	hsvScale.y=1; 
	hsvScale.z=1; 

	 
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

  
  m_seaHeightLocation=m_shader.FindUniform("SEA_HEIGHT");
  
  m_seaChoppyLocation=m_shader.FindUniform("SEA_CHOPPY");

  
  m_waterBaseLocation=m_shader.FindUniform("SEA_BASE");
  m_waterColorLocation=m_shader.FindUniform("SEA_WATER_COLOR");
  
  m_seaFrequencyLocation=m_shader.FindUniform("SEA_FREQ");
  m_skyIntensityLocation=m_shader.FindUniform("SKY_INTENSITY");

  m_seaTimeLocation=m_shader.FindUniform("seaTime");
  m_cameraRotationLocation=m_shader.FindUniform("cameraRotation");


    
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
   
   seaAnim.countx+=delta*((seaAnim.speedx*12.0f-6.0f)); 
   
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
  m_extensions.glUniform1fARB(m_seaChoppyLocation,s_seaChoppy);
  m_extensions.glUniform1fARB(m_seaFrequencyLocation,s_seaFrequency);
  m_extensions.glUniform1fARB(m_seaHeightLocation,s_seaHeight);
  m_extensions.glUniform1fARB(m_skyIntensityLocation,s_skyIntensity);

  m_extensions.glUniform2fARB(m_centerLocation,center.x*4.0-2.0,center.y*4.0-2.0); 
  m_extensions.glUniform3fARB(m_hsvScaleLocation,hsvScale.x,hsvScale.y,hsvScale.z);
  m_extensions.glUniform3fARB(m_cameraRotationLocation,cameraRotation.x*PI*2,cameraRotation.y*PI*2,cameraRotation.z*PI*2);
  m_extensions.glUniform3fARB(m_waterBaseLocation,color1.x,color1.y,color1.z);
  m_extensions.glUniform3fARB(m_waterColorLocation,color2.x,color2.y,color2.z);
  m_extensions.glUniform1fARB(m_seaTimeLocation,seaAnim.countx);


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
   
		case FFPARAM_SEA_CHOPPY:	
			s_seaChoppy= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_SEA_FREQUENCY:	
			s_seaFrequency= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_SEA_HEIGHT:	
			s_seaHeight= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;   
		case FFPARAM_SPEED:	
			seaAnim.speedx= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_CAMERA_ANGLE_X:	
			cameraRotation.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CAMERA_ANGLE_Y:	
			cameraRotation.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CAMERA_ANGLE_Z:	
			cameraRotation.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_SKY_INTENSITY:	
			s_skyIntensity= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_WATER_BASE_RED:	
			color1.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_WATER_BASE_GREEN:	
			color1.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_WATER_BASE_BLUE:	
			color1.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
	 
		case FFPARAM_WATER_RED:	
			color2.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_WATER_GREEN:	
			color2.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
		case FFPARAM_WATER_BLUE:	
			color2.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	 
	 
	 
	 
	 
		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
