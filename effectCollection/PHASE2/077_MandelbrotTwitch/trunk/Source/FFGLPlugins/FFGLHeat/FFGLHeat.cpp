#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"
#include <iostream>
#define PI 3.14159265359
   
#define FFPARAM_ANGLE   (3) 
#define FFPARAM_SCALE   (4) 
#define FFPARAM_CENTERX   (5) 
#define FFPARAM_CENTERY   (6) 

#define FFPARAM_SEED1_SPEED		(7) 
#define FFPARAM_SEED1_RADIUS    (8) 
#define FFPARAM_SEED1_POSX		(9) 
#define FFPARAM_SEED1_POSY		(10)
#define FFPARAM_SEED1_PAR1		(812) 
#define FFPARAM_SEED1_PAR2		(9324)

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
#define FFPARAM_SHIFT_VALUE		(22) 


#define FFPARAM_SCALE_HUE_INNER		(23) 
#define FFPARAM_SCALE_SATURATION_INNER (24) 
#define FFPARAM_SCALE_VALUE_INNER		(25)  
#define FFPARAM_SHIFT_VALUE_INNER		(26) 

#define FFPARAM_PARAM1_X     (27) 
#define FFPARAM_PARAM1_Y     (28) 
#define FFPARAM_PARAM1_Z     (29) 
#define FFPARAM_PARAM1_W     (2) 
#define FFPARAM_PARAM_JULIA     (0) 
#define FFPARAM_PARAM_POWER     (1) 

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
//   .... <- max length
	"S077",								// Plugin unique ID											
//   ................ <- max length
	"sOm-MandelTwitch",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	002,									// Plugin minor version number
	FF_EFFECT,						// Plugin type
	"sOm-Mandelbrot Twitch",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - Twitching. " // About
);

#define STRINGIFY(X) #X

char *vertexShaderCode =STRINGIFY(
void main()
{
  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
  gl_TexCoord[1] = gl_MultiTexCoord1;
  gl_FrontColor = gl_Color;
}
)
;


char *fragmentShaderCode =
"float PI=3.14159265359;\n"
"uniform float aspectRatio;\n"
"uniform sampler2D inputTexture;\n"
"uniform vec2 maxCoords;\n"
"uniform vec4 sineFactors;\n"
"uniform vec4 sineFactors2;\n"
"uniform float time; \n"
"uniform vec3 angles;\n"
"uniform vec4 seed1;\n"
"uniform vec4 seed2;\n"
"uniform vec4 seed3;\n"
"uniform vec4 location;\n"
"uniform vec4 hsvScale;\n"
"uniform vec4 hsvScaleInner;\n"
"uniform vec4 twitchAndIteration;\n"
"uniform float power;\n"
"uniform float julia;\n"
"uniform int  modStep;\n"
"uniform float imageBlend;\n"
"\n"
"float maxIter =floor(twitchAndIteration.w*128.0);\n"
"vec3 hsv2rgb(vec3 c) \n"
"{ \n"
"    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0); \n"
"    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www); \n"
"    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y); \n"
"} \n"
"vec3 createColor(float time){\n \n"
"\tvec3 currentColor=hsv2rgb(vec3(sin(time),1.0,1.0)); \n"
"\treturn currentColor; \n"
"} \n"
"\n"
"\n"
"vec2 cPower(vec2 z, float n) {\n"
"\tfloat r2 = dot(z, z);\n"
"\treturn pow(r2, n / 2.0)*vec2(cos(n*atan(z.y, z.x)), sin(n*atan(z.y, z.x)));\n"
"}\n"
"\n"
" \n"
"float circle(vec2 center, float radius) { \n"
"float l = length(center); \n"
"return 1.0-clamp((l/radius) ,0.0,1.0);  \n"
" \n"
"} \n"
"vec2 kali(vec2 z,vec2 c,float Scaling,float MinRadius) { \n"
"\tfloat m =dot(z,z); \n"
"\tif (m<MinRadius) { \n"
"\t\tz = abs(z)/(MinRadius*MinRadius); \n"
"\t}else { \n"
"\t\tz = abs(z)/m*Scaling; \n"
"\t} \n"
"\treturn z+c; \n"
"} \n"
"vec2 mandelbrot(vec2 z, vec2 c){ \n"
"   return cPower((z),power)+c ; \n"
"} \n"
"vec2 iterate(vec2 uv){ \n"
"\tvec2 z;\n"
"\tvec2 c;\n"
"\t    int i; \n"
"\t    z = vec2(0.0,0.0);   \n"
"\n"
"\t\tif (julia < 0.5) {\n"
"\t\t\tz = 0.0;\n"
"\n"
"\t\t\tc = uv;\n"
"\t\t}\n"
"\t\telse {\n"
"\t\t\tz = uv;\n"
"\n"
"\t\t\tc = 0.0;\n"
"\t\t}\n"
"\n"
"\t\t\n"
"\t\tfor(i=0; i<int(maxIter); i++) {    \n"
"\n"
" z = mandelbrot(z, c);\n"
" if(     (i>=floor(twitchAndIteration.x*maxIter))   &&(i<floor(twitchAndIteration.y*maxIter))   )\n"
" {\n"
"\t // twitch 1\n"
"//\t z = mandelbrot(z, seed1.xy, seed1.z, seed1.w);\n"
"\t z += seed1.xy;\n"
"\n"
"\t      //  \tz= kali(z,seed1.xy,seed1.z,seed1.w);\n"
"} else if(     (i>=floor(twitchAndIteration.y*maxIter))   &&(i<floor(twitchAndIteration.z*maxIter))   )\n"
"{"
"\t//\t\t\t \tz= kali(z,seed2.xy,seed1.z,seed1.w);\n"
"\t\t\t\tz+= seed2.xy;\n"
"\t }else {\n"
"\t\t\t\n"
"//\t        \tz= kali(z,seed3.xy,seed1.z,seed1.w);\n"
"\t\t\t\tz += seed3.xy;\n"
"\n"
"\t        } \n"
"  \n"
"\t        if(dot(z,z) > 1024.0){ \n"
"\tbreak; \n"
"} \n"
"\t    }  \n"
"\n"
"\t// mu = N + 1 - log (log  |Z(N)|) / log 2\n"
"\t\tfloat ci =  float(i)+1.0 - log(log(length(z) ))/log(power); \n"
"\t\tci *= 1.0 / 8.0;\n"
"return vec2(ci,i); \n"
"\t//    return length(z) ; \n"
"} \n"
"mat2 rot2d(float a) { \n"
"\treturn mat2(cos(a),sin(a),-sin(a),cos(a)); \n"
"} \n"
"void main()  \n"
"{ \n"
"   vec2 texCoord = gl_TexCoord[1].st;  //lookup input color\n"
"\ttexCoord=(texCoord/maxCoords)*2.0-1.0; \n"
"   texCoord.y*=aspectRatio;\n"
"   texCoord = texCoord*rot2d(location.w);\n"
"   texCoord*=location.z; \n"
"   texCoord+=location.xy; \n"
" vec2 result=iterate(texCoord*2.0); \n"
" vec4 col_out;\n"
" if (result.y < maxIter)\n"
" {\n"
"\t col_out = vec4(hsv2rgb(vec3(result.x*hsvScale.x + hsvScale.w, result.x*hsvScale.y, (result.x*hsvScale.z) )), 1);\n"
" }\n"
" else {\n"
"\t col_out = vec4(hsv2rgb(vec3(result.x*hsvScaleInner.x + hsvScaleInner.w, (result.x*hsvScaleInner.y), (result.x*hsvScaleInner.z) )), 1);\n"
" }\n"
"  col_out.w=1.0; \n"
"   gl_FragColor =col_out; \n"
"} \n"
""
;

 
////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLSineWave2D::FFGLSineWave2D()
	:CFreeFrameGLPlugin(),
	m_initResources(1),
	m_inputTextureLocation(-1),
	m_maxCoordsLocation(-1),

	bInitialized(false)
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);
	SetTimeSupported(true);
	// Parameters
	// Param swizzle, somehow BPM is not accepted ... right now .. ;)   
	SetParamInfo(FFPARAM_ANGLE, "Angle", FF_TYPE_STANDARD, 0.0f);  
	SetParamInfo(FFPARAM_SCALE, "Scale", FF_TYPE_STANDARD, 1.0f);  
	SetParamInfo(FFPARAM_CENTERX, "CenterX", FF_TYPE_STANDARD, 0.50f);  
	SetParamInfo(FFPARAM_CENTERY, "CenterY", FF_TYPE_STANDARD, 0.50f);  

	SetParamInfo(FFPARAM_SEED1_POSX, "Seed1 X", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SEED1_POSY, "Seed1 Y", FF_TYPE_STANDARD,0.5f); 
	SetParamInfo(FFPARAM_SEED1_RADIUS, "Seed1 Radius", FF_TYPE_STANDARD, 0.0f); 
	SetParamInfo(FFPARAM_SEED1_SPEED, "Seed1 Speed", FF_TYPE_STANDARD,0.5f); 

//	SetParamInfo(FFPARAM_SEED1_PAR1, "Seed1Param1", FF_TYPE_STANDARD, 0.0f); 
//	SetParamInfo(FFPARAM_SEED1_PAR2, "Seed1Param2", FF_TYPE_STANDARD,0.0f); 

	
	SetParamInfo(FFPARAM_SEED2_POSX, "Seed2 X", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SEED2_POSY, "Seed2 Y", FF_TYPE_STANDARD,0.5f); 
	SetParamInfo(FFPARAM_SEED2_RADIUS, "Seed2 Radius", FF_TYPE_STANDARD, 0.f); 
	SetParamInfo(FFPARAM_SEED2_SPEED, "Seed2 Speed", FF_TYPE_STANDARD,0.5f);  
	 
	SetParamInfo(FFPARAM_SEED3_POSX, "Seed3 X", FF_TYPE_STANDARD, 0.5f); 
	SetParamInfo(FFPARAM_SEED3_POSY, "Seed3 Y", FF_TYPE_STANDARD,0.5f); 
	SetParamInfo(FFPARAM_SEED3_RADIUS, "Seed3 Radius", FF_TYPE_STANDARD, 0.0f); 
	SetParamInfo(FFPARAM_SEED3_SPEED, "Seed3 Speed", FF_TYPE_STANDARD,0.5f);  


	SetParamInfo(FFPARAM_SCALE_HUE, "ScaleHue Outer", FF_TYPE_STANDARD, 1.0f);
	SetParamInfo(FFPARAM_SCALE_SATURATION, "ScaleSat Outer", FF_TYPE_STANDARD, 1.0f);
	SetParamInfo(FFPARAM_SCALE_VALUE, "ScaleVal Outer", FF_TYPE_STANDARD, 1.0f);

	SetParamInfo(FFPARAM_SHIFT_VALUE, "ShiftVal Outer", FF_TYPE_STANDARD, 0.0f);

	SetParamInfo(FFPARAM_SCALE_HUE_INNER, "ScaleHue Inner", FF_TYPE_STANDARD, 1.0f);
	SetParamInfo(FFPARAM_SCALE_SATURATION_INNER, "ScaleSat Inner", FF_TYPE_STANDARD, .5f);
	SetParamInfo(FFPARAM_SCALE_VALUE_INNER, "ScaleVal Inner", FF_TYPE_STANDARD, .5f);
	SetParamInfo(FFPARAM_SHIFT_VALUE_INNER, "ShiftVal Inner", FF_TYPE_STANDARD, 0.f);

	SetParamInfo(FFPARAM_PARAM1_X, "Twitch 1 Iter", FF_TYPE_STANDARD, 0.0f);
	SetParamInfo(FFPARAM_PARAM1_Y, "Twitch 2 Iter", FF_TYPE_STANDARD, 0.5f);
	SetParamInfo(FFPARAM_PARAM1_Z, "Twitch 3 Iter", FF_TYPE_STANDARD, 0.75f);
	SetParamInfo(FFPARAM_PARAM1_W, "MaxIter", FF_TYPE_STANDARD, 0.5f);
	SetParamInfo(FFPARAM_PARAM_POWER, "Power", FF_TYPE_STANDARD, 0.2f);
	SetParamInfo(FFPARAM_PARAM_JULIA, "Julia", FF_TYPE_BOOLEAN, 0.0f); 
	 
	//SetParamInfo(FFPARAM_BPM, "BPM(*240)", FF_TYPE_STANDARD, 0.5f);  
	seedAnim1.radius=0.5;
	seedAnim1.speed=1;
	seedAnim1.center.x=0;
	seedAnim1.center.y=0;
	seedAnim1.count=0;
	seedAnim1.par1=0;
	seedAnim1.par2=0; 

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
  m_maxCoordsLocation = m_shader.FindUniform("maxCoords"); 
  m_timeLocation = m_shader.FindUniform("time"); 
  m_aspectLocation= m_shader.FindUniform("aspectRatio");
 
  m_seed1Location=m_shader.FindUniform("seed1");
  m_seed2Location=m_shader.FindUniform("seed2");
  m_seed3Location=m_shader.FindUniform("seed3");
  m_locationLocation=m_shader.FindUniform("location");
  m_HSVScaleLocation = m_shader.FindUniform("hsvScale");
  m_HSVScaleLocation_Inner = m_shader.FindUniform("hsvScaleInner");
  m_modstepLocation=m_shader.FindUniform("modStep");
  m_imageblendLocation = m_shader.FindUniform("imageBlend");
  m_twitchAndIterLocation = m_shader.FindUniform("twitchAndIteration");
  m_juliaLocation= m_shader.FindUniform("julia");
  m_powerLocation = m_shader.FindUniform("power");
  //the 0 means that the 'heatTexture' in
  //the shader will use the texture bound to GL texture unit 0 
    
  //the 1 means that the 'inputTexture' in
  //the shader will use the texture bound to GL texture unit 1
//  m_extensions.glUniform1iARB(m_inputTextureLocation, 1);
    
  m_shader.UnbindShader();
   

  bInitialized = true;
  return FF_SUCCESS;
}

DWORD FFGLSineWave2D::DeInitGL()
{


	bInitialized = false;
//	MessageBox(NULL, "De Init GL ", NULL, MB_OK);
	if (bInitialized) {
	m_shader.UnbindShader();
}

  m_shader.FreeGLResources();
   

 // MessageBox(NULL, "De Init GL FINISHED", NULL, MB_OK); 
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

float sin3Big(float p) {
	return sin(p) + sin(p *.5) + sin(p * .25);
}
float cos3Big(float p) {
	return cos(p) + cos(p * .5) + cos(p * .25);
}
DWORD FFGLSineWave2D::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	if (bInitialized) {
	if (pGL->numInputTextures < 1) return FF_FAIL;

		if (pGL->inputTextures[0] == NULL) return FF_FAIL;
		//activate our shader
		m_shader.BindShader();

		//bind the heat texture to texture unit 0 

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
		float tempseed1real = (seedAnim1.center.x * 2 - 1) + sin3Big(seedAnim1.count*1.013)*seedAnim1.radius;
		float tempseed1imag = (seedAnim1.center.y * 2 - 1) + cos3Big(seedAnim1.count*1.019)*seedAnim1.radius;

		m_extensions.glUniform4fARB(m_seed1Location, tempseed1real, tempseed1imag, seedAnim1.par1 * 4 - 2, seedAnim1.par2);

		float tempseed2real = (seedAnim2.center.x * 2 - 1) + sin3Big(seedAnim2.count*1.013)*seedAnim2.radius;
		float tempseed2imag = (seedAnim2.center.y * 2 - 1) + cos3Big(seedAnim2.count*1.019)*seedAnim2.radius;
		m_extensions.glUniform4fARB(m_seed2Location, tempseed2real, tempseed2imag, seedAnim2.par1 * 4 - 2, seedAnim2.par2);

		float tempseed3real = (seedAnim3.center.x * 2 - 1) + sin3Big(seedAnim3.count*1.003)*seedAnim3.radius;
		float tempseed3imag = (seedAnim3.center.y * 2 - 1) + cos3Big(seedAnim3.count*1.019)*seedAnim3.radius;
		m_extensions.glUniform4fARB(m_seed3Location, tempseed3real, tempseed3imag, seedAnim3.par1 * 4 - 2, seedAnim3.par2);

		//assign the heatAmount
	   // m_extensions.glUniform1fARB(m_heatAmountLocation, 2.0*m_Heat);
		//assign the time
	   // m_extensions.glUniform1fARB(m_timeLocation,(float)GetTimeInSecondsSinceStart()); 
		m_extensions.glUniform1fARB(m_aspectLocation, ((float)pGL->inputTextures[0]->Height / (float)pGL->inputTextures[0]->Width));
		m_extensions.glUniform4fARB(m_locationLocation, fractalCenter.x * 2 - 1, fractalCenter.y * 2 - 1, fractalScale, fractalAngle*PI * 2);
		m_extensions.glUniform4fARB(m_HSVScaleLocation, colorHSV1.x, colorHSV1.y, colorHSV1.z, colorHSV1.w);
		m_extensions.glUniform4fARB(m_HSVScaleLocation_Inner, colorHSV2.x, colorHSV2.y, colorHSV2.z, colorHSV2.w);
		m_extensions.glUniform4fARB(m_twitchAndIterLocation, twitchAndIteration.x, twitchAndIteration.y, twitchAndIteration.z, twitchAndIteration.w);
		m_extensions.glUniform1fARB(m_powerLocation, power*10.0f);
		m_extensions.glUniform1fARB(m_juliaLocation, julia);



		glClearColor(0, 0, 0, 0);

		//activate texture unit 1 and bind the input texture
		m_extensions.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture.Handle);

		//draw the quad that will be painted by the shader/textures
		//note that we are sending texture coordinates to texture unit 1..
		//the vertex shader and fragment shader refer to this when querying for
		//texture coordinates of the inputTexture
		glBegin(GL_QUADS);

		//lower left
		m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0, 0);
		glVertex2f(-1, -1);

		//upper left
		m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0, maxCoords.t);
		glVertex2f(-1, 1);

		//upper right
		m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, maxCoords.t);
		glVertex2f(1, 1);

		//lower right
		m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, 0);
		glVertex2f(1, -1);
		glEnd();

		//unbind the input texture
		glBindTexture(GL_TEXTURE_2D, 0);

		//switch to texture unit 0 and unbind the 1D heat texture 

		//unbind the shader
		m_shader.UnbindShader();
	} 
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
		 
	case FFPARAM_SEED1_RADIUS:

		*((float *)(unsigned)&dwRet) = seedAnim1.radius;
		return dwRet;

		break;
	case FFPARAM_SEED1_SPEED:
		*((float *)(unsigned)&dwRet) =		seedAnim1.speed ;
		return dwRet;

		break;
	case FFPARAM_SEED2_POSX:
		*((float *)(unsigned)&dwRet) = 		seedAnim2.center.x ;
		return dwRet;

		break;
	case FFPARAM_SEED2_POSY:
		*((float *)(unsigned)&dwRet) = 		seedAnim2.center.y ;
		return dwRet;

		break;
	case FFPARAM_SEED2_RADIUS:
		*((float *)(unsigned)&dwRet) =		seedAnim2.radius ;
		return dwRet;

		break;
	case FFPARAM_SEED2_SPEED:
		*((float *)(unsigned)&dwRet) =		seedAnim2.speed ;
		return dwRet;

		break;

	case FFPARAM_SEED3_POSX:
		*((float *)(unsigned)&dwRet) = 		seedAnim3.center.x ;
		return dwRet;

		break;
	case FFPARAM_SEED3_POSY:
		*((float *)(unsigned)&dwRet) =		seedAnim3.center.y ;
		return dwRet;

		break;
	case FFPARAM_SEED3_RADIUS:
		*((float *)(unsigned)&dwRet) =		seedAnim3.radius ;
		return dwRet;

		break;
	case FFPARAM_SEED3_SPEED:
		*((float *)(unsigned)&dwRet) =		seedAnim3.speed ;
		return dwRet;

		break;


	case FFPARAM_ANGLE:
		*((float *)(unsigned)&dwRet) = 		fractalAngle ;
		return dwRet;

		break;
	case FFPARAM_SCALE:
		*((float *)(unsigned)&dwRet) = 		fractalScale ;
		return dwRet;

		break;
	case FFPARAM_CENTERX:
		*((float *)(unsigned)&dwRet) = 		fractalCenter.x ;
		return dwRet;

		break;
	case FFPARAM_CENTERY:
		*((float *)(unsigned)&dwRet) = 		fractalCenter.y ;
		return dwRet;

		break;
	case FFPARAM_SEED1_PAR1:
		*((float *)(unsigned)&dwRet) = 		seedAnim1.par1 ;
		return dwRet;

		break;
	case FFPARAM_SEED1_PAR2:
		*((float *)(unsigned)&dwRet) =		seedAnim1.par2 ;
		return dwRet;

		break;


	case FFPARAM_SCALE_HUE:
		*((float *)(unsigned)&dwRet) = 		colorHSV1.x ;
		return dwRet;

		break;

	case FFPARAM_SCALE_SATURATION:
		*((float *)(unsigned)&dwRet) = 		colorHSV1.y ;
		return dwRet;

		break;

	case FFPARAM_SCALE_VALUE:
		*((float *)(unsigned)&dwRet) = 		colorHSV1.z ;
		return dwRet;

		break;

	case FFPARAM_SHIFT_VALUE:
		*((float *)(unsigned)&dwRet) = 		colorHSV1.w ;
		return dwRet;

		break;

	case FFPARAM_SCALE_HUE_INNER:
		*((float *)(unsigned)&dwRet) =		colorHSV2.x ;
		return dwRet;

		break;

	case FFPARAM_SCALE_SATURATION_INNER:
		*((float *)(unsigned)&dwRet) = 		colorHSV2.y ;
		return dwRet;

		break;

	case FFPARAM_SCALE_VALUE_INNER:
		*((float *)(unsigned)&dwRet) = 		colorHSV2.z ;
		return dwRet;

		break;

	case FFPARAM_SHIFT_VALUE_INNER:
		*((float *)(unsigned)&dwRet) = 		colorHSV2.w ;
		return dwRet;

		break;
	case FFPARAM_PARAM1_X:
		*((float *)(unsigned)&dwRet) = 		twitchAndIteration.x ;
		return dwRet;

		break;
	case FFPARAM_PARAM1_Y:
		*((float *)(unsigned)&dwRet) = 		twitchAndIteration.y ;
		return dwRet;

		break;
	case FFPARAM_PARAM1_Z:
		*((float *)(unsigned)&dwRet) = 		twitchAndIteration.z ;
		return dwRet;

		break;
	case FFPARAM_PARAM1_W:
		*((float *)(unsigned)&dwRet) = 		twitchAndIteration.w ;
		return dwRet;

		break;
	case FFPARAM_PARAM_JULIA:
		*((float *)(unsigned)&dwRet) = julia;
		return dwRet;

		break;
	case FFPARAM_PARAM_POWER:
		*((float *)(unsigned)&dwRet) = 		power ;
		return dwRet;

		break;




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


		case FFPARAM_SCALE_HUE:
			colorHSV1.x = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		case FFPARAM_SCALE_SATURATION:
			colorHSV1.y = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		case FFPARAM_SCALE_VALUE:
			colorHSV1.z = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		case FFPARAM_SHIFT_VALUE:
			colorHSV1.w = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		case FFPARAM_SCALE_HUE_INNER:
			colorHSV2.x = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		case FFPARAM_SCALE_SATURATION_INNER:
			colorHSV2.y = *((float *)(unsigned)&(pParam->NewParameterValue));
//			char numstr[201]; // enough to hold all numbers up to 64-bits
	//		sprintf(numstr, "sat value inner is %d %f", pParam->ParameterNumber,colorHSV2.y);
	//		MessageBox(NULL, (LPCTSTR)&numstr, NULL, MB_OK);

			break;

		case FFPARAM_SCALE_VALUE_INNER:
			colorHSV2.z = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;

		case FFPARAM_SHIFT_VALUE_INNER:
			colorHSV2.w = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_PARAM1_X:
			twitchAndIteration.x = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_PARAM1_Y:
			twitchAndIteration.y = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_PARAM1_Z:
			twitchAndIteration.z = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_PARAM1_W:
			twitchAndIteration.w = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_PARAM_JULIA:
			julia = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		case FFPARAM_PARAM_POWER:
			power = *((float *)(unsigned)&(pParam->NewParameterValue));
			break;
		 

		default:
			std::cout <<"Error" << pParam;
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
