//
//		ShaderMaker.cpp
//
//		A source code template that can be used to compile FreeframeGL
//		plugins from shader source copied from "GLSL Sandbox" and "ShaderToy".
//
//		------------------------------------------------------------
//		Revisions :
//		21-01-15	Version 1.000
//		26.02.15	Changes for FFGL 1.6
//					change DWORD to FFResult
//					remove "Virtual" from destructor definition
//					Changes  for port to OSX
//					Version 1.001
//		06.03.15	Provided for revised SharedToy spec with mainImage instead of main
//                  See ShaderToy example 4
//					Version 1.002
//		30.03.15	Set m_glTextureXX to zero after delete
//					Check for incoming Texture ID change
//					Version 1.003
//		14.04.15	Corrected texture change test in ProcessOpenGL
//					Recommend setting PluginInfo to FF_SOURCE for shaders that do not require a texture
//					Version 1.004
//
//		------------------------------------------------------------
//
//		Copyright (C) 2015. Lynn Jarvis, Leading Edge. Pty. Ltd.
//		Ported to OSX by Amaury Hazan (amaury@billaboop.com)
//
//		This program is free software: you can redistribute it and/or modify
//		it under the terms of the GNU Lesser General Public License as published by
//		the Free Software Foundation, either version 3 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU Lesser General Public License for more details.
//
//		You will receive a copy of the GNU Lesser General Public License along 
//		with this program.  If not, see http://www.gnu.org/licenses/.
//		--------------------------------------------------------------
//
//
#include "ShaderMaker.h"
 

#define PI 3.14159265359
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
int (*cross_secure_sprintf)(char *, size_t, const char *,...) = sprintf_s;
#else 
// posix
int (*cross_secure_sprintf)(char *, size_t, const char *, ...) = snprintf;
#endif

#define FFPARAM_SPEED       (0)
#define FFPARAM_SPEED2       (1)
#define FFPARAM_SPEED3       (2)
#define FFPARAM_SPEED4       (3)
#define FFPARAM_MOUSEX      (4)
#define FFPARAM_MOUSEY      (5)
#define FFPARAM_MOUSELEFTX  (6)
#define FFPARAM_MOUSELEFTY  (7)
#define FFPARAM_PARAM2_X       (8)
#define FFPARAM_PARAM2_Y       (9)
#define FFPARAM_PARAM2_Z       (10)
#define FFPARAM_PARAM2_W       (11)


#define FFPARAM_PARAM3_X       (12)
#define FFPARAM_PARAM3_Y       (13)
#define FFPARAM_PARAM3_Z       (14)
#define FFPARAM_PARAM3_W       (15)



#define FFPARAM_PARAM3_CAMERA_X       (16)
#define FFPARAM_PARAM3_CAMERA_Y       (17)
#define FFPARAM_PARAM3_CAMERA_Z       (18)

#define FFPARAM_PARAM3_CAMERA_TARGET_X       (19)
#define FFPARAM_PARAM3_CAMERA_TARGET_Y       (20)
#define FFPARAM_PARAM3_CAMERA_TARGET_Z       (21)

#define FFPARAM_PARAM_ANGLE_X       (22)
#define FFPARAM_PARAM_ANGLE_Y       (23)
#define FFPARAM_PARAM_ANGLE_Z       (24)









#define FFPARAM_RED         (98)
#define FFPARAM_GREEN       (99)
#define FFPARAM_BLUE        (910)
#define FFPARAM_ALPHA       (911)


#define FFPARAM_PARAM4_X       (111)
#define FFPARAM_PARAM4_Y       (1111)
#define FFPARAM_PARAM4_Z       (11111)
#define FFPARAM_PARAM4_W       (111111)


#define STRINGIFY(A) #A

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++ IMPORTANT : DEFINE YOUR PLUGIN INFORMATION HERE +++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static CFFGLPluginInfo PluginInfo ( 
	ShaderMaker::CreateInstance,		// Create method
	"S050",								// *** Plugin unique ID (4 chars) - this must be unique for each plugin
	"sOm-SeaCreature",						// *** Plugin name - make it different for each plugin 
	1,						   			// API major version number 													
	006,								// API minor version number	
	1,									// *** Plugin major version number
	005,								// *** Plugin minor version number
	FF_SOURCE,							// Plugin type can always be an effect
	// FF_SOURCE,						// or change this to FF_SOURCE for shaders that do not use a texture
	"Kali Living Sea Creature", // *** Plugin description - you can expand on this
	"spack-O-mat Shadertoy Collection 2 TriSin Kaliset Hybrid"			// *** About - use your own name and details
);

 
// Common vertex shader code as per FreeFrame examples
char *vertexShaderCode = STRINGIFY (
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;

} );


// Important notes :

// The shader code is pasted into a section of the source file below
// which uses the Stringizing operator (#) (https://msdn.microsoft.com/en-us/library/7e3a913x.aspx)
// This converts the shader code into a string which is then used by the shader compiler on load of the plugin.
// There are some limitations of using the stringizing operator in this way because it depends on the "#" symbol,
// e.g. #( .. code ), Therefore there cannot be any # characters in the code itself.
//
// For example it is common to see :
//
//		#ifdef GL_ES
//		precision mediump float;
//		#endif
//
//	The #ifdef and #endif have to be removed.
//
//		// #ifdef GL_ES
//		// precision mediump float;
//		// #endif
//
// Compile the code as-is to start with and you should get a functioning plugin.
// Several examples can be used below. Only one can be compiled at a time.
//

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++ COPY/PASTE YOUR GLSL SANDBOX OR SHADERTOY SHADER CODE HERE +++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char *fragmentShaderCode = STRINGIFY (
// ==================== PASTE WITHIN THESE LINES =======================
// http://www.fractalforums.com/movies-showcase-%28rate-my-movie%29/very-rare-deep-sea-fractal-creature/


float triSin(float x){

	return (sin(x)+sin(x*2)+sin(x*4))/3;
}
float triCos(float x){

	return (cos(x)+cos(x*2)+cos(x*4))/5;
}

 
const int Iterations=25;
float Scale=(iParam3.w+1.0);
 vec3 Julia=iParam3.xyz*15.0;
vec3 RotVector=normalize(vec3(
				sin((iMouse.x/iResolution.x)*2*PI),
				cos((iMouse.x/iResolution.x)*2*PI),
				sin((iMouse.y/iResolution.y)*2*PI)

				));
 float RotAngle=triSin(iChannelTime[1])*(iMouse.w/iResolution.y)*360.0 +((iMouse.z/iResolution.x)*360.0);
const float Speed=0.3;
const float Amplitude=0.6;
const float detail=.0005;
vec3 lightdir=vec3(0,-1,0);
vec3 RotVector2=normalize(vec3(
				sin((iParam2.x*PI)),
				cos((iParam2.x*PI)),
				sin((iParam2.y*PI))

				));
 float RotAngle2=triSin(iChannelTime[2])*(iParam2.w)*180+iParam2.z*180.0;

 mat2 rot;
mat2 rot2;

float de(vec3 p); 
vec3 color(vec3 p);

vec3 normal(vec3 p) {
	vec3 e = vec3(0.0,detail,0.0);
	
	return normalize(vec3(
			de(p+e.yxx)-de(p-e.yxx),
			de(p+e.xyx)-de(p-e.xyx),
			de(p+e.xxy)-de(p-e.xxy)
			)
		);	
}

float calcAO( const vec3 pos, const vec3 nor ) {
	float aodet=detail*3.;
	float totao = 0.0;
    float sca = 1.0;
    for( int aoi=0; aoi<5; aoi++ ) {
        float hr = aodet*float(aoi*aoi);
        vec3 aopos =  nor * hr + pos;
        float dd = de( aopos );
        totao += -(dd-hr)*sca;
        sca *= 0.7;
    }
    return clamp( 1.0 - 5.0*totao, 0., 1.0 );
}


float softshadow( in vec3 ro, in vec3 rd, float mint, float k )
{
    float res = 1.0;
    float t = mint;
    for( int i=0; i<48; i++ )
    {
        float h = de(ro + rd*t);
		h = max( h, 0.0 );
        res = min( res, k*h/t );
        t += clamp( h, 0.01, 0.5 );
    }
    return clamp(res,0.0,1.0);
}




vec3 light(in vec3 p, in vec3 dir) {
	vec3 ldir=normalize(lightdir);
	vec3 n=normal(p);
	float sh=softshadow(p,-ldir,1.,20.);
	float diff=max(0.,dot(ldir,-n));
	vec3 r = reflect(ldir,n);
	float spec=max(0.,dot(dir,-r));
	vec3 colo=color(p);
    return (diff*sh+.15*max(0.,dot(normalize(dir),-n))*calcAO(p,n))*colo+pow(spec,30.)*.5*sh;	
		}


float kaliset(vec3 p) {
    p.y-=iGlobalTime;
	p.y=abs(2.-mod(p.y,4.));
    for (int i=0;i<18;i++) p=abs(p)/dot(p,p)-.8;
    return length(p);
}



vec4 raymarch(in vec3 from, in vec3 dir, vec2 fragCoord)
{
	vec3 col=vec3(0.0);
    float st=0.0;
	float d=1.0;
		float totdist=st=0.0;
	vec3 p;
	float k;
    for (int i=0; i<256; i++) {
	if (d>detail && totdist<500.0)
	{
	//	k+=kaliset(p)*exp(-.002*totdist*totdist)*max(0.,totdist-3.)*(1.-step(0.,.2-d));
        p=from+totdist*dir;
		d=de(p);
		totdist+=d;
	}else{
	break;
	}
	}
	vec3 backg=vec3(.4,.5,.55)*(1.+fragCoord.y/iResolution.y*1.5);
backg*=0.0;
float alpha=1.0;

	if (d<detail) {
		col=light(p-detail*dir, dir); 
		alpha=1.0;
	} else { 
		col=vec3(backg);
		alpha=0.0;
	} 
    return vec4(col,alpha);
}

mat3  rotationMatrix3(vec3 v, float angle)
{
	float c = cos(radians(angle));
	float s = sin(radians(angle));
	
	return mat3(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y,
		(1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x,
		(1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z
		);
}


vec3 lookat(vec3 p0, vec3 at, vec2 uv, float fov)
{
    vec3 v = normalize(at - p0);
    float sin_el = v.y;
    float cos_el = cos(asin(sin_el));
    float sin_az = -v.x / cos_el;
    float cos_az = -v.z / cos_el;
    uv *= tan(radians(fov));
    return normalize(vec3(
         uv.x*cos_az + uv.y*sin_az*sin_el + v.x,
                       uv.y       *cos_el + v.y,
        -uv.x*sin_az + uv.y*cos_az*sin_el + v.z
    ));
}

	float time=iGlobalTime*Speed;
	vec3 ani=vec3(sin(time),sin(time),cos(time))*Amplitude;
	mat3 kaliRot1 = rotationMatrix3(normalize(RotVector), RotAngle);
	mat3 kaliRot2= rotationMatrix3(normalize(RotVector2), RotAngle2);


	mat3 setCamera( in vec3 ro, in vec3 ta, float cr )
{
	vec3 cw = normalize(ta-ro);
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
	vec3 cu = normalize( cross(cw,cp) );
	vec3 cv = normalize( cross(cu,cw) );
    return mat3( cu, cv, cw );
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{

	

	float t=0;
	vec2 uv = fragCoord.xy / iResolution.xy*2.-1.;
	uv.y*=iResolution.y/iResolution.x;
	vec3 from=vec3(0.,-.7,-20.+sin(t)*8.);

	from=iCamera*50.0;


	vec3 dir=normalize(vec3(uv,1.));
	rot=mat2(cos(-.5),sin(-.5),-sin(-.5),cos(-.5));
	dir.yz=dir.yz*rot;
	from.yz=from.yz*rot;

	vec3 camPos=vec3(
				sin(iCamera.x*PI),
				cos(iCamera.x*PI),
				sin(iCamera.y*PI)

				)*iCamera.z*10;
	 


	
	vec2 q = fragCoord.xy/iResolution.xy;
    vec2 p = -1.0+2.0*q;
	p.x *= iResolution.x/iResolution.y;
	    mat3 ca = setCamera(iCamera*50, iCameraTarget*50, 0.0 );
    
    // ray direction
	vec3 rd = ca * normalize( vec3(p.xy,2.5) );


	vec4 col=raymarch(iCamera*50,rd,fragCoord); 
	//col*=max(0.,.1-length(uv*.05))/.1;
    fragColor = col;
}


float de(vec3 p) {
	p*=mat3(iRotationMatrix);
	// cube . return length(max(abs(p)-1.0,0.0));
	p=p.zxy;
	p.z/=2;
	float a=1.5+sin(0*.3578)*.5;
	p.xy=p.xy*mat2(cos(a),sin(a),-sin(a),cos(a));
	p.x*=.75;
	p+=sin(p*3.+time*6.)*.04;
	vec3 pp=p;
	float l;
	for (int i=0; i<Iterations; i++) {
		p.xy=abs(p.xy);
		p=p*Scale+Julia;
if(i%2==0){	
	
		// iparam3.xyz= seed .w=scale
	p*=mat3(iRotationMatrix3);
	}else{
	p*=mat3(iRotationMatrix2);
	
	}
		l=length(p);
	}
	return l*pow(Scale, -float(Iterations))-.1;
}



vec3 color(vec3 p) {
	p=p.zxy;
	float a=1.5+sin(0*.3578)*.5;
	p.xy=p.xy*mat2(cos(a),sin(a),-sin(a),cos(a));
	p.x*=.75;
	float time=0*Speed;
	vec3 ani;
	ani=vec3(sin(time),sin(time),cos(time))*Amplitude;
	p+=sin(p*3.+time*6.)*.04;
	mat3 rot = rotationMatrix3(normalize(RotVector+ani), RotAngle+sin(time)*10.);
	vec3 pp=p;
	float l;
	float ot=9999.;
    for (int i=0; i<15; i++) {
		p.xy=abs(p.xy);
		p=p*Scale+Julia;
		p*=rot;
		l=length(p.x);
        ot=min(l,ot);
	}
	return mix(vec3(1.,0.2,0.1),vec3(0.6,0.5,0.6),max(0.,2.-ot)/2.);
}

);


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////
ShaderMaker::ShaderMaker():CFreeFrameGLPlugin()
{

	/*
	// Debug console window so printf works
	FILE* pCout; // should really be freed on exit 
	AllocConsole();
	freopen_s(&pCout, "CONOUT$", "w", stdout); 
	printf("Shader Maker Vers 1.004\n");
	printf("GLSL version [%s]\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	*/

	// Input properties allow for no texture or for four textures
	SetMinInputs(0);
	SetMaxInputs(2); // TODO - 4 inputs

	// Parameters
	SetParamInfo(FFPARAM_SPEED,         "Speed",         FF_TYPE_STANDARD, 0.5f); m_UserSpeed = 0.0f;
	SetParamInfo(FFPARAM_SPEED2,         "Speed 2",         FF_TYPE_STANDARD, 0.0f); m_UserSpeed2 = 0.0f;
	SetParamInfo(FFPARAM_SPEED3,         "Speed 3",         FF_TYPE_STANDARD, 0.0f); m_UserSpeed3 = 0.0f;
	SetParamInfo(FFPARAM_SPEED4,         "Speed 4",         FF_TYPE_STANDARD, 0.0f); m_UserSpeed4 = 0.0f;

	SetParamInfo(FFPARAM_MOUSEX,        "Rot1 Axis XY",       FF_TYPE_STANDARD, 0.0f); m_UserMouseX = 0.0f;
	SetParamInfo(FFPARAM_MOUSEY,        "Rot1 Axis Z",       FF_TYPE_STANDARD, 0.0f); m_UserMouseY = 0.0f;
	SetParamInfo(FFPARAM_MOUSELEFTX,    "Rot1 Offset",  FF_TYPE_STANDARD, 0.0f); m_UserMouseLeftX = 0.0f;
	SetParamInfo(FFPARAM_MOUSELEFTY,    "Rot1 Sway",  FF_TYPE_STANDARD, 0.1f); m_UserMouseLeftY = 0.5f;

	/*
	SetParamInfo(FFPARAM_RED,           "Red",           FF_TYPE_STANDARD, 0.75f); m_UserRed = 07.5f;


	SetParamInfo(FFPARAM_GREEN,         "Green",         FF_TYPE_STANDARD, 0.5f); m_UserGreen = 0.5f;
	SetParamInfo(FFPARAM_BLUE,          "Blue",          FF_TYPE_STANDARD, 0.5f); m_UserBlue = 0.5f;
	SetParamInfo(FFPARAM_ALPHA,         "Alpha",         FF_TYPE_STANDARD, 1.0f); m_UserAlpha = 1.0f;
	*/
	SetParamInfo(FFPARAM_PARAM2_X,         "Rot2 Axi2 XY",         FF_TYPE_STANDARD, 0.0f); 
	SetParamInfo(FFPARAM_PARAM2_Y,         "Rot2 Axis Z ",         FF_TYPE_STANDARD, 0.0f); 
	SetParamInfo(FFPARAM_PARAM2_Z,         "Rot2 Offset",         FF_TYPE_STANDARD, 0.0f);  
	SetParamInfo(FFPARAM_PARAM2_W,         "Rot2 Sway",         FF_TYPE_STANDARD, 0.25f); 

	SetParamInfo(FFPARAM_PARAM3_X,         "Julia 1 X",         FF_TYPE_STANDARD, 0.750f); m_UserParam3x = 0.750f;
	SetParamInfo(FFPARAM_PARAM3_Y,         "Julia 1 Y",         FF_TYPE_STANDARD, 0.750f); m_UserParam3y = 0.750f;
	SetParamInfo(FFPARAM_PARAM3_Z,         "Julia 1 Z",         FF_TYPE_STANDARD, 0.750f); m_UserParam3z = 0.750f;	
	SetParamInfo(FFPARAM_PARAM3_W,         "Scale 1",         FF_TYPE_STANDARD, 0.70f); m_UserParam3w = 0.75f;

/*	SetParamInfo(FFPARAM_PARAM4_X,         "UNUSED",         FF_TYPE_STANDARD, 0.750f); m_UserParam4x = 0.0f;
	SetParamInfo(FFPARAM_PARAM4_Y,         "UNUSED ",         FF_TYPE_STANDARD, 0.250f); m_UserParam4y = 0.0f;
	SetParamInfo(FFPARAM_PARAM4_Z,         "UNUSED",         FF_TYPE_STANDARD, 0.750f); m_UserParam4z = 0.0f;	
	SetParamInfo(FFPARAM_PARAM4_W,         "UNUSED",         FF_TYPE_STANDARD, 0.75f); m_UserParam4w = 0.75f;
	*/
	SetParamInfo(FFPARAM_PARAM3_CAMERA_X,"Camera X",         FF_TYPE_STANDARD, 0.750f); m_Camera_x = .75f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_Y,"Camera Y",         FF_TYPE_STANDARD, 0.50f); m_Camera_y= 0.5f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_Z,"Camera Z",         FF_TYPE_STANDARD, 0.40f); m_Camera_z = .45f;


	// 0.5 means the middle = because its multiplied *2 and -1 ;)
	SetParamInfo(FFPARAM_PARAM3_CAMERA_TARGET_X,"Camera Target X",         FF_TYPE_STANDARD, 0.5f); m_CameraTarget_x= 0.50f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_TARGET_Y,"Camera Target Y",         FF_TYPE_STANDARD, 0.50f); m_CameraTarget_y = 0.50f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_TARGET_Z,"Camera Target Z",         FF_TYPE_STANDARD, 0.50f); m_CameraTarget_z = 0.50f;
	 
	SetParamInfo(FFPARAM_PARAM_ANGLE_X,"Rotation X",         FF_TYPE_STANDARD, 0.f);
	SetParamInfo(FFPARAM_PARAM_ANGLE_Y,"Rotation Z",         FF_TYPE_STANDARD, 0.f);
	SetParamInfo(FFPARAM_PARAM_ANGLE_Z,"Rotation Y",         FF_TYPE_STANDARD, 0.f); 
 
	// Set defaults
	SetDefaults();

	// Flags
	bInitialized = false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////
FFResult ShaderMaker::InitGL(const FFGLViewportStruct *vp)
{
	// initialize gl extensions and make sure required features are supported
	m_extensions.Initialize();
	if (m_extensions.multitexture==0 || m_extensions.ARB_shader_objects==0)
		return FF_FAIL;

	// Set the viewport size
	// Actually it has to be checked in ProcessOpenGL because it could change
	m_vpWidth  = (float)vp->width;
	m_vpHeight = (float)vp->height;

	// Start the clock
	StartCounter();

	// Load the shader
	std::string shaderString = fragmentShaderCode;
	bInitialized = LoadShader(shaderString);

	return FF_SUCCESS;
}

ShaderMaker::~ShaderMaker()
{
	// Not using this but it is here just in case
}


FFResult ShaderMaker::DeInitGL()
{
	if(bInitialized)
		m_shader.UnbindShader();

	m_shader.FreeGLResources();

	if(m_fbo) m_extensions.glDeleteFramebuffersEXT(1, &m_fbo);
	if(m_glTexture0) glDeleteTextures(1, &m_glTexture0);
	if(m_glTexture1) glDeleteTextures(1, &m_glTexture1);
	if(m_glTexture2) glDeleteTextures(1, &m_glTexture2);
	if(m_glTexture3) glDeleteTextures(1, &m_glTexture3);
	m_glTexture0 = 0;
	m_glTexture1 = 0;
	m_glTexture2 = 0;
	m_glTexture3 = 0;
	m_fbo = 0;
	bInitialized = false;

	return FF_SUCCESS;
}



float triSin(float x){
return (sin(x)+sin(x*2)+sin(x*4))/4.0f;

}
FFResult ShaderMaker::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	FFGLTextureStruct Texture0;
	FFGLTextureStruct Texture1;
	// TODO
	// FFGLTextureStruct Texture2;
	// FFGLTextureStruct Texture3;
	FFGLTexCoords maxCoords;
	time_t datime;
	struct tm tmbuff;

	if(bInitialized) {

		// To the host this is an effect plugin, but it can be either a source or an effect
		// and will work without any input, so we still start up if even there is no input texture

		// Set the global viewport resolution from OpenGL now for certainty
		// It could be different to that receieved by InitGL
		float vpdim[4];
		glGetFloatv(GL_VIEWPORT, vpdim);
		m_vpWidth  = vpdim[2];
		m_vpHeight = vpdim[3];

		// Is there is texture needed by the shader ?
		if(m_inputTextureLocation >= 0 || m_inputTextureLocation1 >= 0) {

			// Is there a texture available ?
			if(m_inputTextureLocation >= 0 && pGL->numInputTextures > 0 && pGL->inputTextures[0] != NULL) {

				Texture0 = *(pGL->inputTextures[0]);
				maxCoords = GetMaxGLTexCoords(Texture0);

				// Delete the local texture if the incoming size is different
				if((int)m_channelResolution[0][0] != Texture0.Width 
				|| (int)m_channelResolution[0][1] != Texture0.Height) {
					if(m_glTexture0 > 0) {
						glDeleteTextures(1, &m_glTexture0);
						m_glTexture0 = 0; // This is needed or the local texture is not re-created in CreateRectangleTexture (30.03.15)
					}
				}

				// Set the resolution of the first texture size
				m_channelResolution[0][0] = (float)Texture0.Width;
				m_channelResolution[0][1] = (float)Texture0.Height;

				// For a power of two texture, the size will be different to the hardware size.
				// The shader will not compensate for this, so we have to create another texture
				// the same size as the resolution we set to the shader.  Also the shader needs
				// textures created with wrapping REPEAT rather than CLAMP to edge. So we ALWAYS
				// create such a texture and use it for every frame. The texture is re-created
				// if the texture size changes 
				CreateRectangleTexture(Texture0, maxCoords, m_glTexture0, GL_TEXTURE0, m_fbo, pGL->HostFBO);
				// Now we have a local texture of the right size and type
				// Filled with the data from the incoming Freeframe texture
			}

			// Repeat if there is a second incoming texture and the shader needs it
			if(m_inputTextureLocation1 >= 0 && pGL->numInputTextures > 1 && pGL->inputTextures[1] != NULL) {
				Texture1 = *(pGL->inputTextures[1]);
				maxCoords = GetMaxGLTexCoords(Texture1);
				if((int)m_channelResolution[1][0] != Texture1.Width 
				|| (int)m_channelResolution[1][1] != Texture1.Height) {
					if(m_glTexture1 > 0) {
						glDeleteTextures(1, &m_glTexture1);
						m_glTexture1 = 0;
					}
				}
				// Set the channel resolution of the second texture size
				m_channelResolution[1][0] = (float)Texture1.Width;
				m_channelResolution[1][1] = (float)Texture1.Height;
				CreateRectangleTexture(Texture1, maxCoords, m_glTexture1, GL_TEXTURE1, m_fbo, pGL->HostFBO);
			}

			/*
			// And for textures 3 and 4
			if(m_inputTextureLocation2 >= 0 && pGL->numInputTextures > 2 && pGL->inputTextures[2] != NULL) {
				Texture2 = *(pGL->inputTextures[2]);
				maxCoords = GetMaxGLTexCoords(Texture2);
				if((int)m_channelResolution[2][0] != Texture2.Width 
				|| (int)m_channelResolution[2][1] != Texture2.Height) {
					if(m_glTexture2 > 0) {
						glDeleteTextures(1, &m_glTexture2);
						m_glTexture2 = 0;
					}
				}				
				m_channelResolution[2][0] = (float)Texture2.Width;
				m_channelResolution[2][1] = (float)Texture2.Height;
				CreateRectangleTexture(Texture2, maxCoords, m_glTexture2, GL_TEXTURE1, m_fbo, pGL->HostFBO);
			}

			if(m_inputTextureLocation3 >= 0 && pGL->numInputTextures > 3 && pGL->inputTextures[3] != NULL) {
				Texture3 = *(pGL->inputTextures[3]);
				maxCoords = GetMaxGLTexCoords(Texture3);
				if((int)m_channelResolution[3][0] != Texture3.Width 
				|| (int)m_channelResolution[3][1] != Texture3.Height) {
					if(m_glTexture3 > 0) {
						glDeleteTextures(1, &m_glTexture3);
						m_glTexture3 = 0;
					}
				}
				m_channelResolution[3][0] = (float)Texture3.Width;
				m_channelResolution[3][1] = (float)Texture3.Height;
				CreateRectangleTexture(Texture3, maxCoords, m_glTexture3, GL_TEXTURE1, m_fbo, pGL->HostFBO);
			}
			*/

		} // endif shader uses a texture
	
		// Calculate elapsed time
		lastTime = elapsedTime;
		elapsedTime = GetCounter()/1000.0; // In seconds - higher resolution than timeGetTime()
		m_time = m_time + (float)(elapsedTime-lastTime)*m_UserSpeed*2.0f; // increment scaled by user input 0.0 - 2.0

		m_time2 = m_time2 + (float)(elapsedTime-lastTime)*m_UserSpeed2*2.0f; // increment scaled by user input 0.0 - 2.0

		m_time3 = m_time3 + (float)(elapsedTime-lastTime)*m_UserSpeed3*2.0f; // increment scaled by user input 0.0 - 2.0

		m_time4= m_time4 + (float)(elapsedTime-lastTime)*m_UserSpeed4*2.0f; // increment scaled by user input 0.0 - 2.0

		// Just pass elapsed time for individual channel times
		m_channelTime[0] = m_time;
		m_channelTime[1] = m_time2;
		m_channelTime[2] = m_time3;
		m_channelTime[3] = m_time4;

		// Calculate date vars
		time(&datime);
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
		localtime_s(&tmbuff, &datime);
#else
        localtime_r(&datime, &tmbuff);
#endif
		m_dateYear = (float)tmbuff.tm_year;
		m_dateMonth = (float)tmbuff.tm_mon+1;
		m_dateDay = (float)tmbuff.tm_mday;
		m_dateTime = (float)(tmbuff.tm_hour*3600 + tmbuff.tm_min*60 + tmbuff.tm_sec);

		// activate our shader
		m_shader.BindShader();

		//
		// Assign values and set the uniforms to the shader
		//

		//
		// Common
		//

		// First input texture
		// The shader will use the first texture bound to GL texture unit 0
		if(m_inputTextureLocation >= 0 && Texture0.Handle > 0) {
			m_extensions.glUniform1iARB(m_inputTextureLocation, 0);
		}

		// Second input texture
		// The shader will use the texture bound to GL texture unit 1
		if(m_inputTextureLocation1 >= 0 && Texture1.Handle > 0)
			m_extensions.glUniform1iARB(m_inputTextureLocation1, 1);

		/*
		// 4 channels
		if(m_inputTextureLocation2 >= 0 && Texture2.Handle > 0)
			m_extensions.glUniform1iARB(m_inputTextureLocation2, 2);

		if(m_inputTextureLocation3 >= 0 && Texture3.Handle > 0)
			m_extensions.glUniform1iARB(m_inputTextureLocation3, 3);
		*/

		// fixme: calculate matrix only when values changed :/
		float rotmat[16];

		getEulerMatrix(rotmat,rotation.x*360,rotation.y*360,rotation.z*360);
		if(m_rotationMatrixLocation>= 0) 
			m_extensions.glUniformMatrix4fvARB(m_rotationMatrixLocation,1,false, rotmat);
		 

 getRotMatrixFromSpherical(rotmat,triSin(m_time2)*iParam1.w+iParam1.z,iParam1.x,iParam1.y);
		if(m_rotationMatrixLocation2>= 0) 
			m_extensions.glUniformMatrix4fvARB(m_rotationMatrixLocation2,1,false, rotmat);




 getRotMatrixFromSpherical(rotmat,triSin(m_time3)*iParam2.w+iParam2.z,iParam2.x,iParam2.y);
		if(m_rotationMatrixLocation3>= 0) 
			m_extensions.glUniformMatrix4fvARB(m_rotationMatrixLocation3,1,false, rotmat);







		// Elapsed time
		if(m_timeLocation >= 0) 
			m_extensions.glUniform1fARB(m_timeLocation, m_time);
	
		m_extensions.glUniform4fARB(m_iParam2Location, iParam2.x*2.0-1.0,iParam2.y*2.0-1.0,iParam2.z*2.0-1.0,iParam2.w*2.0-1.0); 
		m_extensions.glUniform4fARB(m_iParam3Location, m_UserParam3x*2.0-1.0,m_UserParam3y*2.0-1.0,m_UserParam3z*2.0-1.0,m_UserParam3w*2.0-1.0); 
		m_extensions.glUniform4fARB(m_iParam4Location, m_UserParam4x*2.0-1.0,m_UserParam4y*2.0-1.0,m_UserParam4z*2.0-1.0,m_UserParam4w*2.0-1.0); 
		//
		// GLSL sandbox
		//
		// resolution (viewport size)
		if(m_screenLocation >= 0) 
			m_extensions.glUniform2fARB(m_screenLocation, m_vpWidth, m_vpHeight); 

		// mouse - Mouse position
		if(m_mouseLocation >= 0) { // Vec2 - normalized
			m_mouseX = m_UserMouseX;
			m_mouseY = m_UserMouseY;
			m_extensions.glUniform2fARB(m_mouseLocation, m_mouseX, m_mouseY); 
		}
		if(m_iCameraLocation >= 0) { // Vec3- normalized 
			m_extensions.glUniform3fARB(m_iCameraLocation, m_Camera_x*2.0f-1.0f,m_Camera_y*2.0f-1.0f,m_Camera_z*2.0f-1.0f); 
		}
		if(m_iCameraTargetLocation >= 0) { // Vec3- normalized 
			m_extensions.glUniform3fARB(m_iCameraTargetLocation, m_CameraTarget_x*2.0f-1.0f,m_CameraTarget_y*2.0f-1.0f,m_CameraTarget_z*2.0f-1.0f); 
		}
		if(m_rotationLocation >= 0) { // Vec3- normalized 
			m_extensions.glUniform3fARB(m_rotationLocation,rotation.x*2.0f-1.0f,rotation.y*2.0f-1.0f,rotation.z*2.0f-1.0f); 
		}

		// surfaceSize - Mouse left drag position - in pixel coordinates
		if(m_surfaceSizeLocation >= 0) {
			m_mouseLeftX = m_UserMouseLeftX*m_vpWidth;
			m_mouseLeftY = m_UserMouseLeftY*m_vpHeight;
			m_extensions.glUniform2fARB(m_surfaceSizeLocation, m_mouseLeftX, m_mouseLeftY);
		}

		//
		// Shadertoy

		// iMouse
		// xy contain the current pixel coords (if LMB is down);
		// zw contain the click pixel.
		// Modified here equivalent to mouse unclicked or left button dragged
		// The mouse is not being simulated, they are just inputs that can be used within the shader.
		if(m_mouseLocationVec4 >= 0) {
			// Convert from 0-1 to pixel coordinates for ShaderToy
			// Here we use the resolution rather than the screen
			m_mouseX     = m_UserMouseX*m_vpWidth;
			m_mouseY     = m_UserMouseY*m_vpHeight;
			m_mouseLeftX = m_UserMouseLeftX*m_vpWidth;
			m_mouseLeftY = m_UserMouseLeftY*m_vpHeight;
			m_extensions.glUniform4fARB(m_mouseLocationVec4, m_mouseX, m_mouseY, m_mouseLeftX, m_mouseLeftY); 
		}

		// iResolution - viewport resolution
		if(m_resolutionLocation >= 0) // Vec3
			m_extensions.glUniform3fARB(m_resolutionLocation, m_vpWidth, m_vpHeight, 1.0); 

		// Channel resolutions are linked to the actual texture resolutions - the size is set in ProcessOpenGL
		// Global resolution is the viewport
		if(m_channelresolutionLocation >= 0) {
			// uniform vec3	iChannelResolution[4]
			// 4 channels Vec3. Float array is 4 rows, 3 cols
			// TODO - 4 channels - 2 & 3 are unused so will not have a texture anyway
			m_channelResolution[2][0] = m_vpWidth;
			m_channelResolution[2][1] = m_vpHeight;
			m_channelResolution[2][2] = 1.0;
			m_channelResolution[3][0] = m_vpWidth;
			m_channelResolution[3][1] = m_vpHeight;
			m_channelResolution[3][2] = 1.0;
			m_extensions.glUniform3fvARB(m_channelresolutionLocation, 4, (GLfloat *)m_channelResolution);
		}

		// iDate - vec4
		if(m_dateLocation >= 0) 
			m_extensions.glUniform4fARB(m_dateLocation, m_dateYear, m_dateMonth, m_dateDay, m_dateTime);

		// Channel elapsed time - vec4
		if(m_channeltimeLocation >= 0)
			m_extensions.glUniform1fvARB(m_channeltimeLocation, 4, m_channelTime);

		// Extras - input colour is linked to the user controls Red, Green, Blue, Alpha
		if(m_inputColourLocation >= 0)
			m_extensions.glUniform4fARB(m_inputColourLocation, m_UserRed, m_UserGreen, m_UserBlue, m_UserAlpha);


		// Bind a texture if the shader needs one
		if(m_inputTextureLocation >= 0 && Texture0.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE0);
			// Has the local texture been created
			// TODO - it should have been always created so this logic can be changed
			if(m_glTexture0 > 0) 
				glBindTexture(GL_TEXTURE_2D, m_glTexture0);
			else 
				glBindTexture(GL_TEXTURE_2D, Texture0.Handle);
		}

		// If there is a second texture, bind it to texture unit 1
		if(m_inputTextureLocation1 >= 0 && Texture1.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE1);
			if(m_glTexture1 > 0)
				glBindTexture(GL_TEXTURE_2D, m_glTexture1);
			else
				glBindTexture(GL_TEXTURE_2D, Texture1.Handle);
		}

		/*
		// Texture units 2 and 3
		if(m_inputTextureLocation2 >= 0 && Texture2.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE2);
			if(m_glTexture2 > 0)
				glBindTexture(GL_TEXTURE_2D, m_glTexture2);
			else
				glBindTexture(GL_TEXTURE_2D, Texture2.Handle);
		}

		if(m_inputTextureLocation3 >= 0 && Texture3.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE3);
			if(m_glTexture3 > 0)
				glBindTexture(GL_TEXTURE_2D, m_glTexture3);
			else
				glBindTexture(GL_TEXTURE_2D, Texture3.Handle);
		}
		*/

		// Do the draw for the shader to work
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);	
		glVertex2f(-1.0, -1.0);
		glTexCoord2f(0.0, 1.0);	
		glVertex2f(-1.0,  1.0);
		glTexCoord2f(1.0, 1.0);	
		glVertex2f( 1.0,  1.0);
		glTexCoord2f(1.0, 0.0);	
		glVertex2f( 1.0, -1.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		/*
		// unbind input texture 3
		if(m_inputTextureLocation3 >= 0 && Texture3.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// unbind input texture 2
		if(m_inputTextureLocation2 >= 0 && Texture2.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		*/

		// unbind input texture 1
		if(m_inputTextureLocation1 >= 0 && Texture1.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// unbind input texture 0
		m_extensions.glActiveTexture(GL_TEXTURE0); // default
		if(m_inputTextureLocation >= 0 && Texture0.Handle > 0)
			glBindTexture(GL_TEXTURE_2D, 0);

		// unbind the shader
		m_shader.UnbindShader();

	} // endif bInitialized

	return FF_SUCCESS;
}

char * ShaderMaker::GetParameterDisplay(DWORD dwIndex) {

	memset(m_DisplayValue, 0, 15);
	switch (dwIndex) {
		
	case FFPARAM_PARAM3_CAMERA_X:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_Camera_x));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_Y:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_Camera_y));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_Z:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_Camera_z));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_X:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_CameraTarget_x));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Y:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_CameraTarget_y));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Z:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_CameraTarget_z));
			return m_DisplayValue;
	
		case FFPARAM_SPEED:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed*100.0));
			return m_DisplayValue;
	
		case FFPARAM_SPEED2:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed2*100.0));
			return m_DisplayValue;
	
		case FFPARAM_SPEED3:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed3*100.0));
			return m_DisplayValue;
	
		case FFPARAM_SPEED4:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed4*100.0));
			return m_DisplayValue;
	
		case FFPARAM_PARAM2_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(iParam2.x));
			return m_DisplayValue;
		case FFPARAM_PARAM2_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(iParam2.y));
			return m_DisplayValue;
		case FFPARAM_PARAM2_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(iParam2.z));
			return m_DisplayValue;
		case FFPARAM_PARAM2_W:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(iParam2.w));
			return m_DisplayValue;
			
		case FFPARAM_PARAM3_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3x));
			return m_DisplayValue;
		case FFPARAM_PARAM3_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3y));
			return m_DisplayValue;
		case FFPARAM_PARAM3_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3z));
			return m_DisplayValue;
		case FFPARAM_PARAM3_W:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3w));
			return m_DisplayValue;
			
		case FFPARAM_PARAM4_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4x));
			return m_DisplayValue;
		case FFPARAM_PARAM4_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4y));
			return m_DisplayValue;
		case FFPARAM_PARAM4_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4z));
			return m_DisplayValue;
		case FFPARAM_PARAM4_W:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4w));
			return m_DisplayValue;

		case FFPARAM_MOUSEX:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseX*m_vpWidth));
			return m_DisplayValue;

		case FFPARAM_MOUSEY:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseY*m_vpHeight));
			return m_DisplayValue;

		case FFPARAM_MOUSELEFTX:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseLeftX*m_vpWidth));
			return m_DisplayValue;

		case FFPARAM_MOUSELEFTY:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseLeftY*m_vpHeight));
			return m_DisplayValue;

		case FFPARAM_RED:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserRed*256.0));
			return m_DisplayValue;

		case FFPARAM_GREEN:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserGreen*256.0));
			return m_DisplayValue;

		case FFPARAM_BLUE:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserBlue*256.0));
			return m_DisplayValue;
			
		case FFPARAM_ALPHA:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserAlpha*256.0));
			return m_DisplayValue;
			
		case FFPARAM_PARAM_ANGLE_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(rotation.x));
			return m_DisplayValue;

		case FFPARAM_PARAM_ANGLE_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(rotation.y));
			return m_DisplayValue;

		case FFPARAM_PARAM_ANGLE_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(rotation.z));
			return m_DisplayValue;

		default:
			return m_DisplayValue;
	}
	return NULL;
}

FFResult ShaderMaker::GetInputStatus(DWORD dwIndex)
{
	DWORD dwRet = FF_INPUT_NOTINUSE;

	switch (dwIndex) {

		case 0 :
			if(m_inputTextureLocation >= 0)
				dwRet = FF_INPUT_INUSE;
			break;

		case 1 :
			if(m_inputTextureLocation1 >= 0)
				dwRet = FF_INPUT_INUSE;
			break;

		/* TODO - 4 channels
		case 2 :
			if(m_inputTextureLocation2 >= 0)
				dwRet = FF_INPUT_INUSE;
			break;

		case 3 :
			if(m_inputTextureLocation3 >= 0)
				dwRet = FF_INPUT_INUSE;
			break;
		*/


		default :
			break;

	}

	return dwRet;

}

float ShaderMaker::GetFloatParameter(unsigned int index)
{
	switch (index) {
		
	case FFPARAM_PARAM3_CAMERA_X:
		return  m_Camera_x;
		
	case FFPARAM_PARAM3_CAMERA_Y:
		return  m_Camera_y;
		
	case FFPARAM_PARAM3_CAMERA_Z:
		return  m_Camera_z;
			
	case FFPARAM_PARAM3_CAMERA_TARGET_X:
		return  m_CameraTarget_x;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Y:
		return  m_CameraTarget_y;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Z:
		return  m_CameraTarget_z;
	
		case FFPARAM_SPEED:
			return  m_UserSpeed;
	
		case FFPARAM_SPEED2:
			return  m_UserSpeed2;
	
		case FFPARAM_SPEED3:
			return  m_UserSpeed3;
	
		case FFPARAM_SPEED4:
			return  m_UserSpeed4;
	
		case FFPARAM_MOUSEX:
			return  m_UserMouseX;

		case FFPARAM_MOUSEY:
			return  m_UserMouseY;

		case FFPARAM_MOUSELEFTX:
			return m_UserMouseLeftX;

		case FFPARAM_MOUSELEFTY:
			return m_UserMouseLeftY;

		case FFPARAM_RED:
			return m_UserRed;

		case FFPARAM_GREEN:
			return m_UserGreen;

		case FFPARAM_BLUE:
			return m_UserBlue;

		case FFPARAM_ALPHA:
			return m_UserAlpha;

		case FFPARAM_PARAM2_X:
			return iParam2.x;
case FFPARAM_PARAM2_Y:
	return iParam2.y;
case FFPARAM_PARAM2_Z:
	return iParam2.z;
case FFPARAM_PARAM2_W:
	return iParam2.w;


		case FFPARAM_PARAM3_X:
			return m_UserParam3x;
case FFPARAM_PARAM3_Y:
			return m_UserParam3y;
case FFPARAM_PARAM3_Z:
			return m_UserParam3z;
case FFPARAM_PARAM3_W:
			return m_UserParam3w;


		case FFPARAM_PARAM4_X:
			return m_UserParam4x;
case FFPARAM_PARAM4_Y:
			return m_UserParam4y;
case FFPARAM_PARAM4_Z:
			return m_UserParam4z;
case FFPARAM_PARAM4_W:
			return m_UserParam4w;


			
case FFPARAM_PARAM_ANGLE_X:
	return rotation.x;
case FFPARAM_PARAM_ANGLE_Z:
	return rotation.z;
case FFPARAM_PARAM_ANGLE_Y:
	return rotation.y;


		default:
			return FF_FAIL;
	}
}

FFResult ShaderMaker::SetFloatParameter(unsigned int index, float value)
{
		switch (index) {
			
		case FFPARAM_PARAM3_CAMERA_X:
			m_Camera_x = value;
				break;

		case FFPARAM_PARAM3_CAMERA_Y:
			m_Camera_y = value;
				break;

		case FFPARAM_PARAM3_CAMERA_Z:
			m_Camera_z = value;
				break;

			
		case FFPARAM_PARAM3_CAMERA_TARGET_X:
			m_CameraTarget_x = value;
				break;

		case FFPARAM_PARAM3_CAMERA_TARGET_Y:
			m_CameraTarget_y = value;
				break;

		case FFPARAM_PARAM3_CAMERA_TARGET_Z:
			m_CameraTarget_z = value;
				break;

			
			case FFPARAM_SPEED:
				m_UserSpeed = value;
				break;
				
			case FFPARAM_SPEED2:
				m_UserSpeed2 = value;
				break;

			case FFPARAM_SPEED3:
				m_UserSpeed3 = value;
				break;

			case FFPARAM_SPEED4:
				m_UserSpeed4 = value;
				break;
			
			case FFPARAM_PARAM2_X:
				iParam2.x = value;
				break;
			case FFPARAM_PARAM2_Y:
				iParam2.y= value;
				break;
			case FFPARAM_PARAM2_Z:
				iParam2.z= value;
				break;
			case FFPARAM_PARAM2_W:
				iParam2.w= value;
				break;

			
			case FFPARAM_PARAM3_X:
				m_UserParam3x = value;
				break;
			case FFPARAM_PARAM3_Y:
				m_UserParam3y = value;
				break;
			case FFPARAM_PARAM3_Z:
				m_UserParam3z = value;
				break;
			case FFPARAM_PARAM3_W:
				m_UserParam3w = value;
				break;

			
			case FFPARAM_PARAM4_X:
				m_UserParam4x = value;
				break;
			case FFPARAM_PARAM4_Y:
				m_UserParam4y = value;
				break;
			case FFPARAM_PARAM4_Z:
				m_UserParam4z = value;
				break;
			case FFPARAM_PARAM4_W:
				m_UserParam4w = value;
				break;


			case FFPARAM_MOUSEX:
				iParam1.x=value;
				m_UserMouseX = value;
				break;

			case FFPARAM_MOUSEY:
				iParam1.y=value;
				m_UserMouseY = value;
				break;

			case FFPARAM_MOUSELEFTX:
				iParam1.z=value;
				m_UserMouseLeftX = value;
				break;

			case FFPARAM_MOUSELEFTY:
				iParam1.w=value;
				m_UserMouseLeftY = value;
				break;

			case FFPARAM_RED:
				m_UserRed = value;
				break;

			case FFPARAM_GREEN:
				m_UserGreen = value;
				break;

			case FFPARAM_BLUE:
				m_UserBlue = value;
				break;
				
			case FFPARAM_ALPHA:
				m_UserAlpha = value;
				break;

			case FFPARAM_PARAM_ANGLE_X:
				rotation.x= value;
				break;
			case FFPARAM_PARAM_ANGLE_Y:
				rotation.y= value;
				break;
			case FFPARAM_PARAM_ANGLE_Z:
				rotation.z= value;
				break;

			default:
				return FF_FAIL;
		}
		return FF_SUCCESS;
}

void ShaderMaker::SetDefaults() {

    elapsedTime            = 0.0;
    lastTime               = 0.0;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	PCFreq                 = 0.0;
	CounterStart           = 0;
#else
    start = std::chrono::steady_clock::now();
#endif

	m_mouseX               = 0.5;
	m_mouseY               = 0.5;
	m_mouseLeftX           = 0.5;
	m_mouseLeftY           = 0.5;

	m_UserMouseX           = 0.5;
	m_UserMouseY           = 0.5;
	m_UserMouseLeftX       = 0.5;
	m_UserMouseLeftY       = 0.5;
	
	m_time                 = 0.0;
	m_time2                 = 0.0;
	m_time3                 = 0.0;
	m_time4                 = 0.0;
	m_dateYear             = 0.0;
	m_dateMonth            = 0.0;
	m_dateDay              = 0.0;
	m_dateTime             = 0.0;

	m_channelTime[0]       = 0.0;
	m_channelTime[1]       = 0.0;
	m_channelTime[2]       = 0.0;
	m_channelTime[3]       = 0.0;

	// ShaderToy -  Vec3 - 4 channels 
	m_channelResolution[0][0] = 0.0; // 0 is width
	m_channelResolution[0][1] = 0.0; // 1 is height
	m_channelResolution[0][2] = 1.0; // 2 is depth

	m_channelResolution[1][0] = 0.0;
	m_channelResolution[1][1] = 0.0;
	m_channelResolution[1][2] = 1.0;

	m_channelResolution[2][0] = 0.0;
	m_channelResolution[2][1] = 0.0;
	m_channelResolution[2][2] = 1.0;

	m_channelResolution[3][0] = 0.0;
	m_channelResolution[3][1] = 0.0;
	m_channelResolution[4][2] = 1.0;
	
	m_UserSpeed               = 0.5;
	m_UserSpeed2               = 0.5;
	m_UserSpeed3               = 0.5;
	m_UserSpeed4               = 0.5;
	m_UserMouseX              = 0.5;
	m_UserMouseY              = 0.5;
	m_UserMouseLeftX          = 0.5;
	m_UserMouseLeftY          = 0.5;

	// OpenGL
	m_glTexture0              = 0;
	m_glTexture1              = 0;
	m_glTexture2              = 0;
	m_glTexture3              = 0;
	m_fbo                     = 0;

}

bool ShaderMaker::LoadShader(std::string shaderString) {
		
		std::string stoyUniforms;

		//
		// Extra uniforms specific to ShaderMaker for buth GLSL Sandbox and ShaderToy
		// For GLSL Sandbox, the extra "inputColour" uniform has to be typed into the shader
		//		uniform vec4 inputColour
		static char *extraUniforms = { "uniform vec4 inputColour;\n" };
		
		// Is it a GLSL Sandbox file?
		// look for "uniform float time;". If it does not exist it is a ShaderToy file
		// This is an exact string, so the shader has to have it.
		if(strstr(shaderString.c_str(), "uniform float time;") == 0) {
			//
			// ShaderToy file
			//
			// Shadertoy does not include uniform variables in the source file so add them here
			//
			// uniform vec3			iResolution;			// the rendering resolution (in pixels)
			// uniform float		iGlobalTime;			// current time (in seconds)
			// uniform vec4		 	iMouse;					// xy contain the current pixel coords (if LMB is down). zw contain the click pixel.
			// uniform vec4			iDate;					// (year, month, day, time in seconds)
			// uniform float		iChannelTime[4];		// channel playback time (in seconds)
			// uniform vec3			iChannelResolution[4];	// channel resolution (in pixels)
			// uniform sampler2D	iChannel0;				// sampler for input texture 0.
			// uniform sampler2D	iChannel1;				// sampler for input texture 1.
			// uniform sampler2D	iChannel2;				// sampler for input texture 2.
			// uniform sampler2D	iChannel3;				// sampler for input texture 3.
			static char *uniforms = { "uniform vec3 iResolution;\n"
									  "uniform float iGlobalTime;\n"
									  "uniform vec4 iMouse;\n"
									  "uniform vec4 iDate;\n"
									  "uniform float iChannelTime[4];\n"
									  "uniform vec3 iChannelResolution[4];\n"
									  "uniform sampler2D iChannel0;\n"
									  "uniform sampler2D iChannel1;\n"
									  "uniform sampler2D iChannel2;\n"
									  "uniform sampler2D iChannel3;\n"
									  // ck hackss
									  "#define PI 3.14159265359 \n"
									  "uniform vec4 iParam2;\n"
									  "uniform vec4 iParam3;\n"
									  "uniform vec4 iParam4;\n"
									  "uniform vec3 iCamera;\n"
									  "uniform vec3 iCameraTarget;\n"
			  						  "uniform vec3 iRotation;\n"
			  						  "uniform mat4 iRotationMatrix;\n"
			  						  "uniform mat4 iRotationMatrix2;\n"
			  						  "uniform mat4 iRotationMatrix3;\n"
			
			};
			
			stoyUniforms = uniforms;
			stoyUniforms += extraUniforms;
			stoyUniforms += shaderString; // add the rest of the shared content

			// It might be a revised ShaderToy file with "mainImage" instead of "main"
			if(strstr(shaderString.c_str(), "void mainImage") != 0) {
				//
				// If it is a revised spec ShaderToy file, add a fix at the end for GLSL compatibility
				//
				// Credit Eric Newman 
				// http://magicmusicvisuals.com/forums/viewtopic.php?f=2&t=196
				//
				static char *stoyMainFunction = { "void main(void) {\n"
												  "    mainImage(gl_FragColor, gl_FragCoord.xy);\n"
												  "}\n" };
				stoyUniforms += stoyMainFunction;
			}

			shaderString = stoyUniforms; // the final string
		}
	
		// initialize gl shader
		m_shader.SetExtensions(&m_extensions);
		if (!m_shader.Compile(vertexShaderCode, shaderString.c_str())) {
			// SelectSpoutPanel("Shader compile error");
		 
			
			return false;
		}
		else {
			// activate our shader
			bool success = false;
			if (m_shader.IsReady()) {
				if (m_shader.BindShader())
					success = true;
			}

			if (!success) {
				// SelectSpoutPanel("Shader bind error");
				return false;
			}
			else {
				// Set uniform locations to -1 so that they are only used if necessary
				m_timeLocation				 = -1;
				m_channeltimeLocation		 = -1;
				m_mouseLocation				 = -1;
				m_mouseLocationVec4			 = -1;
				m_dateLocation				 = -1;
				m_resolutionLocation		 = -1;
				m_channelresolutionLocation  = -1;
				m_inputTextureLocation		 = -1;
				m_inputTextureLocation1		 = -1;
				m_inputTextureLocation2		 = -1;
				m_inputTextureLocation3		 = -1;
				m_screenLocation			 = -1;
				m_surfaceSizeLocation		 = -1;
				// m_surfacePositionLocation	= -1; // TODO
				// m_vertexPositionLocation    = -1; // TODO

				// Extras
				// Input colour is linked to the user controls Red, Green, Blue, Alpha
				m_inputColourLocation        = -1;


				// lookup the "location" of each uniform

				//
				// GLSL Sandbox
				//
				// Normalized mouse position. Components of this vector are always between 0.0 and 1.0.
				//	uniform vec2 mouse;
				// Screen (Viewport) resolution.
				//	uniform vec2 resolution;
				// Used for mouse left drag currently
				//	uniform vec2 surfaceSize;
				//  TODO uniform vec2 surfacePosition;

				// Input textures do not appear to be in the GLSL Sandbox spec
				// but are allowed for here

				// From source of index.html on GitHub
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("texture");

				// Preferred names tex0 and tex1 which are commonly used
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("tex0");
				
				if(m_inputTextureLocation1 < 0)
					m_inputTextureLocation1 = m_shader.FindUniform("tex1");
				
				if(m_iCameraLocation< 0)
					m_iCameraLocation					= m_shader.FindUniform("iCamera");
				if(m_rotationLocation< 0)
					m_rotationLocation					= m_shader.FindUniform("iRotation");
				
				if(m_rotationMatrixLocation< 0)
					m_rotationMatrixLocation= m_shader.FindUniform("iRotationMatrix");
				if(m_rotationMatrixLocation2< 0)
					m_rotationMatrixLocation2= m_shader.FindUniform("iRotationMatrix2");
				if(m_rotationMatrixLocation3< 0)
					m_rotationMatrixLocation3= m_shader.FindUniform("iRotationMatrix3");

				if(m_iCameraTargetLocation< 0)
					m_iCameraTargetLocation					= m_shader.FindUniform("iCameraTarget");

				// TODO tex2 and tex3 ?

				// Backbuffer is not supported and is mapped to Texture unit 0
				// From source of index.html on GitHub
				// https://github.com/mrdoob/glsl-sandbox/blob/master/static/index.html
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("backbuffer");

				// From several sources
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("bbuff");
				
				// Time
				if(m_timeLocation < 0)
					m_timeLocation = m_shader.FindUniform("time");
				
				// Param 2
				if(m_iParam2Location< 0)
					m_iParam2Location = m_shader.FindUniform("iParam2");
				// Param 3
				if(m_iParam3Location< 0)
					m_iParam3Location = m_shader.FindUniform("iParam3");
				// Param 4
				if(m_iParam4Location< 0)
					m_iParam4Location = m_shader.FindUniform("iParam4");

				// Mouse move
				if(m_mouseLocation < 0)
					m_mouseLocation = m_shader.FindUniform("mouse");

				// Screen size
				if(m_screenLocation < 0) // Vec2
					m_screenLocation = m_shader.FindUniform("resolution"); 

				// Mouse left drag
				if(m_surfaceSizeLocation < 0)
					m_surfaceSizeLocation = m_shader.FindUniform("surfaceSize");
				
				/*
				// TODO
				// surfacePosAttrib is the attribute, surfacePosition is the varying var
				m_surfacePositionLocation = m_shader.FindAttribute("surfacePosAttrib"); 
				if(m_surfacePositionLocation < 0) printf("surfacePosition attribute not found\n");
				if(m_surfacePositionLocation >= 0) {
					// enable the attribute
					m_extensions.glEnableVertexAttribArrayARB(m_surfacePositionLocation);
				}
				m_vertexPositionLocation = m_shader.FindAttribute("position");
				if(m_vertexPositionLocation < 0) printf("vertexPosition attribute not found\n");
				if(m_vertexPositionLocation >= 0) {
					// enable the attribute
					m_extensions.glEnableVertexAttribArrayARB(m_vertexPositionLocation);
				}
				*/

				//
				// Shadertoy
				//

				
				//
				// Texture inputs iChannelx
				//
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("iChannel0");
				
				if(m_inputTextureLocation1 < 0)
					m_inputTextureLocation1 = m_shader.FindUniform("iChannel1");

				if(m_inputTextureLocation2 < 0)
					m_inputTextureLocation2 = m_shader.FindUniform("iChannel2");

				if(m_inputTextureLocation3 < 0)
					m_inputTextureLocation3 = m_shader.FindUniform("iChannel3");

				// iResolution
				if(m_resolutionLocation < 0) // Vec3
					m_resolutionLocation = m_shader.FindUniform("iResolution");

				// iMouse
				if(m_mouseLocationVec4 < 0) // Shadertoy is Vec4
					m_mouseLocationVec4 = m_shader.FindUniform("iMouse");

				// iGlobalTime
				if(m_timeLocation < 0)
					m_timeLocation = m_shader.FindUniform("iGlobalTime");
				 

				// iDate
				if(m_dateLocation < 0)
					m_dateLocation = m_shader.FindUniform("iDate");

				// iChannelTime
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[4]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[0]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[1]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[2]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[3]");

				// iChannelResolution
				if(m_channelresolutionLocation < 0) // Vec3 width, height, depth * 4
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[4]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[0]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[1]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[2]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[3]");

				// inputColour - linked to user input
				if(m_inputColourLocation < 0)
					m_inputColourLocation = m_shader.FindUniform("inputColour");

				m_shader.UnbindShader();

				// Delete the local texture because it might be a different size
				if(m_glTexture0 > 0) glDeleteTextures(1, &m_glTexture0);
				if(m_glTexture1 > 0) glDeleteTextures(1, &m_glTexture1);
				if(m_glTexture2 > 0) glDeleteTextures(1, &m_glTexture2);
				if(m_glTexture3 > 0) glDeleteTextures(1, &m_glTexture3);
				m_glTexture0 = 0;
				m_glTexture1 = 0;
				m_glTexture2 = 0;
				m_glTexture3 = 0;

				// Start the clock again to start from zero
				StartCounter();

				return true;

			} // bind shader OK
		} // compile shader OK
		// =============================================

		return false;
}

void ShaderMaker::StartCounter()
{

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    LARGE_INTEGER li;
	// Find frequency
    QueryPerformanceFrequency(&li);
    PCFreq = double(li.QuadPart)/1000.0;
	// Second call needed
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
#else
    // posix c++11
    start = std::chrono::steady_clock::now();
#endif

}

double ShaderMaker::GetCounter()
{

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
#else
    // posix c++11
    end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000.;
#endif
    return 0;
}

void ShaderMaker::CreateRectangleTexture(FFGLTextureStruct Texture, FFGLTexCoords maxCoords, GLuint &glTexture, GLenum texunit, GLuint &fbo, GLuint hostFbo)
{
	// First create an fbo and a texture the same size if they don't exist
	if(fbo == 0) {
		m_extensions.glGenFramebuffersEXT(1, &fbo); 
	}

	// The texture ID will be zero if not created yet or if it has been deleted
	// due to size or ID change of the incoming FreeFrame texture
	if(glTexture == 0) {
		glGenTextures(1, &glTexture);
		m_extensions.glActiveTexture(texunit);
		glBindTexture(GL_TEXTURE_2D, glTexture);
		glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGBA, Texture.Width, Texture.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_extensions.glActiveTexture(GL_TEXTURE0);
	} // endif created a new texture
				
	// Render the incoming texture to the local one via the fbo
	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	m_extensions.glFramebufferTexture2DEXT(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, glTexture, 0);
	glBindTexture(GL_TEXTURE_2D, Texture.Handle);
				
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	//
	// Must refer to maxCoords here because the texture
	// could be smaller than the hardware size containing it
	//
	//lower left
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1.0, -1.0);
	//upper left
	glTexCoord2f(0.0, (float)maxCoords.t);
	glVertex2f(-1.0, 1.0);
	// upper right
	glTexCoord2f((float)maxCoords.s, (float)maxCoords.t);
	glVertex2f(1.0, 1.0);
	//lower right
	glTexCoord2f((float)maxCoords.s, 0.0);
	glVertex2f(1.0, -1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// unbind the input texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// unbind the fbo
	if(hostFbo > 0)
		m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, hostFbo);
	else
		m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}



void ShaderMaker::getEulerMatrix(float matrix[],float x,float y,float z){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(x,1,0,0);
	glRotatef(y,0,1,0);
	glRotatef(z,0,0,1);

	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glLoadIdentity();

}

void ShaderMaker::getRotMatrix(float matrix[],float angle,float x,float y,float z){
	glMatrixMode(GL_MODELVIEW);
	float length=sqrt(x*x+y*y+z*z);
	glLoadIdentity();
	glRotatef(angle,x/length,y/length,z/length);

	glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
	glLoadIdentity();

}

void ShaderMaker::getRotMatrixFromSpherical(float matrix[],float angle,float phi,float theta){

		float rotmat[16];
		//getEulerMatrix(rotmat,rotation.x*360,rotation.y*360,rotation.z*360);

		// use spherical coordinates to define an axis with 2 input values
		float rotx=sin(phi*PI)+cos(theta*2*PI);
		float roty=sin(phi*PI)+cos(theta*2*PI);
		float rotz=cos(phi*PI);


		getRotMatrix(matrix,angle*360,rotx,roty,rotz); 

}
