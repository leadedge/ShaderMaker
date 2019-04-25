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
//		02.05.15	Note changes to project file for VS2012 :
//					  Compiler :
//						Optimization /O2 from /Od
//						Enable intrinsic functions (NO)
//					  Linker :
//						Debugging - Generate Debug Info (YES)
//					Version 1.005
//		21.11.17	- New shadertoy uniforms
//						iTime
//						iDeltaTime
//						iFrame
//						iFrameRate
//						iSampleRate
//					Version 1.006
//
//						
//		------------------------------------------------------------
//
//		Copyright (C) 2015-2017. Lynn Jarvis, Leading Edge. Pty. Ltd.
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

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
int (*cross_secure_sprintf)(char *, size_t, const char *,...) = sprintf_s;
#else 
// posix
int (*cross_secure_sprintf)(char *, size_t, const char *, ...) = snprintf;
#endif

#define FFPARAM_SPEED       (100000)



#define FFPARAM_MOUSEX      (10001)
#define FFPARAM_MOUSEY      (10002)
#define FFPARAM_MOUSELEFTX  (10003)
#define FFPARAM_MOUSELEFTY  (10004)
#define FFPARAM_RED         (10005)
#define FFPARAM_GREEN       (10006)
#define FFPARAM_BLUE        (10007)
#define FFPARAM_ALPHA       (10008)

#define FFPARAM_VECTOR1_X       (0)
#define FFPARAM_VECTOR1_Y       (1)
#define FFPARAM_VECTOR1_Z       (2)
#define FFPARAM_VECTOR1_W       (3)

#define FFPARAM_VECTOR5_X       (4)
#define FFPARAM_VECTOR5_Y       (5)
#define FFPARAM_VECTOR5_Z       (6)


#define FFPARAM_VECTOR2_X       (7)
#define FFPARAM_VECTOR2_Y       (8)
#define FFPARAM_VECTOR2_Z       (9)
#define FFPARAM_VECTOR2_W       (10)

#define FFPARAM_VECTOR6_X       (11)
#define FFPARAM_VECTOR6_Y       (12)

#define FFPARAM_VECTOR3_X       (13)
#define FFPARAM_VECTOR3_Y       (14)
#define FFPARAM_VECTOR3_Z       (15)
#define FFPARAM_VECTOR3_W       (16)

#define FFPARAM_VECTOR6_Z       (17)
#define FFPARAM_VECTOR6_W       (18)

#define FFPARAM_VECTOR4_X       (19)
#define FFPARAM_VECTOR4_Y       (20)
#define FFPARAM_VECTOR4_Z       (21)
#define FFPARAM_VECTOR4_W       (22)



#define FFPARAM_VECTOR5_W       (100020)



#define FFPARAM_VECTOR7_X       (23)
#define FFPARAM_VECTOR7_Y       (24)
#define FFPARAM_VECTOR7_Z       (1000027)
#define FFPARAM_VECTOR7_W       (100028)


#define FFPARAM_COLOR1_RED       (25)  
#define FFPARAM_COLOR1_GREEN     (26)  
#define FFPARAM_COLOR1_BLUE      (27)  
#define FFPARAM_COLOR1_ALPHA     (10028)  

#define FFPARAM_COLOR2_RED       (28)  
#define FFPARAM_COLOR2_GREEN     (29)  
#define FFPARAM_COLOR2_BLUE      (30)  
#define FFPARAM_COLOR2_ALPHA     (10032)   
#define STRINGIFY(A) #A

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++ IMPORTANT : DEFINE YOUR PLUGIN INFORMATION HERE +++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static CFFGLPluginInfo PluginInfo ( 
	ShaderMaker::CreateInstance,		// Create method
	"SX01",								// *** Plugin unique ID (4 chars) - this must be unique for each plugin
	"SoM ST Living XT",						// *** Plugin name - make it different for each plugin 
	1,						   			// API major version number 													
	006,								// API minor version number	
	1,									// *** Plugin major version number
	002,								// *** Plugin minor version number
	// FF_EFFECT,							// Plugin type can always be an effect
	FF_SOURCE,						// or change this to FF_SOURCE for shaders that do not use a texture
	"SoM Kali Living Creature Extended", // *** Plugin description - you can expand on this
	"https://www.shadertoy.com/view/Mtf3Rr"			// *** About - use your own name and details
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
// https://www.shadertoy.com/view/Mtf3Rr
// http://www.fractalforums.com/movies-showcase-%28rate-my-movie%29/very-rare-deep-sea-fractal-creature/


// util library
vec3 spherical(float inclination, float elevation) {
	return vec3(
		sin(inclination)*cos(elevation),
		sin(inclination)*sin(elevation),
		cos(inclination)
		);
}

mat3  rotationMatrix3Radian(vec3 v, float angleRadian)
{
	float c = cos(angleRadian);
	float s = sin(angleRadian);

	return mat3(c + (1.0 - c) * v.x * v.x, (1.0 - c) * v.x * v.y - s * v.z, (1.0 - c) * v.x * v.z + s * v.y,
		(1.0 - c) * v.x * v.y + s * v.z, c + (1.0 - c) * v.y * v.y, (1.0 - c) * v.y * v.z - s * v.x,
		(1.0 - c) * v.x * v.z - s * v.y, (1.0 - c) * v.y * v.z + s * v.x, c + (1.0 - c) * v.z * v.z
		);
}

mat3  rotationMatrix3(vec3 v, float angleDegree)
{
	return rotationMatrix3Radian(v, radians(angleDegree));
}

// util library end

const int Iterations = 25; 
const float detail = .02;
const vec3 lightdir = -vec3(0., 1., 0.);
  
float Scale  =   inputVector1.x*2.0 -2.0 ;




float objectScale = inputVector5.x *inputVector5.x*2.0;
vec3 objectTranslate = vec3(inputVector1.y*40.0-20.0, inputVector1.z*40.0 - 20.0, inputVector1.w*40.0 - 20.0);

mat3 objectRotation =rotationMatrix3(vec3(0.0, 1.0, 0.0), inputVector5.z*360 - 180.0)* rotationMatrix3(vec3(1.0, 0.0, 0.0), inputVector5.y*360 - 180.0);



vec3 Julias[3] = vec3[3](inputVector2.xyz*3.0 , inputVector3.xyz*3.0, inputVector4.xyz*3.0);
float RotAngles[3] = float [3](inputVector2.w*360.0, inputVector3.w*360.0, inputVector4.w*360.0);


vec3 RotVectors[3] = vec3[3](

	spherical(inputVector6.x*2.0*PI , inputVector6.y*2.0*PI ),
	spherical(inputVector6.z*2.0*PI , inputVector6.w*2.0*PI ),
	spherical(inputVector7.x*2.0*PI , inputVector7.y*2.0*PI )
	 
	);
mat2 rot;

float de(vec3 p);
vec3 color(vec3 p); 

vec3 normal(vec3 p) {
	vec3 e = vec3(0.0, detail, 0.0);

	return normalize(vec3(
		de(p + e.yxx) - de(p - e.yxx),
		de(p + e.xyx) - de(p - e.xyx),
		de(p + e.xxy) - de(p - e.xxy)
		)
		);
}

float calcAO(const vec3 pos, const vec3 nor) {
	float aodet = detail*3.;
	float totao = 0.0;
	float sca = 1.0;
	for (int aoi = 0; aoi<5; aoi++) {
		float hr = aodet*float(aoi*aoi);
		vec3 aopos = nor * hr + pos;
		float dd = de(aopos);
		totao += -(dd - hr)*sca;
		sca *= 0.7;
	}
	return clamp(1.0 - 5.0*totao, 0., 1.0);
}


float softshadow(in vec3 ro, in vec3 rd, float mint, float k)
{
	float res = 1.0;
	float t = mint;
	for (int i = 0; i<48; i++)
	{
		float h = de(ro + rd*t);
		h = max(h, 0.0);
		res = min(res, k*h / t);
		t += clamp(h, 0.01, 0.5);
	}
	return clamp(res, 0., 1.0);
}



 

mat3 rots[3] = mat3[3](
	rotationMatrix3(normalize(RotVectors[0] ), RotAngles[0]  ),
	rotationMatrix3(normalize(RotVectors[1]  ), RotAngles[1] ),
	rotationMatrix3(normalize(RotVectors[2]  ), RotAngles[2] ));

vec3 color(vec3 p) { 
	p = objectScale* (p + objectTranslate)* objectRotation;
	vec3 pp = p;
	float l;
	float ot = 9999.;
	for (int i = 0; i < Iterations; i++) {
		p.xy = abs(p.xy);
		if (i % 3 == 0) {
			p = p * Scale + Julias[0];
			p *= rots[0];
		}
		else if (i % 3 == 1) {
			p = p * Scale  + Julias[1];
			p *= rots[1];
		}
		else {
			p = p * Scale  + Julias[2];
			p *= rots[2];
		}

		l = length(p.x);
		ot = min(l, ot);
	}
	return mix(inputColor1.xyz, inputColor2.xyz, max(0., 2. - ot) / 2.);
}

vec3 light(in vec3 p, in vec3 dir) {
	vec3 ldir = normalize(lightdir);
	vec3 n = normal(p);
	float sh =    softshadow(p, -ldir, 1., 20.);
	
	float diff = max(.1, dot(ldir, -n));
	vec3 r = reflect(ldir, n);
	float spec = max(0., dot(dir, -r));
	vec3 colo = color(p);
	return (diff*sh + .15*max(0., dot(normalize(dir), -n))*calcAO(p, n))*colo + pow(spec, 30.)*.75*sh;
}

 

vec3 raymarch(in vec3 from, in vec3 dir, vec2 fragCoord)
{
	vec3 col = vec3(0.);
	float st =  0.0;
	float d = 1.0;
	float  totdist = 0; 
	vec3 p; 
	for (int i = 0; i<70; i++) {
		if (d>detail && totdist<150.)
		{
			p = from + totdist*dir ;
			d = de(p);
			totdist += d *0.9;
		}
		 
	}
	// vec3 backg = vec3(.4, .5, .55)*(1. + fragCoord.y / iResolution.y*1.5);
	if (d<detail) {
		col = light(p - detail*dir, dir)+0.1;
	}
	else {
		col = vec3(0.0,0.0,0.0);
		// col = dir;
	}
	// col = mix(col, vec3(backg), 1.0 - exp(-.002*totdist*totdist));
	return col; // +k*.002;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	
	float t =1.0;
	vec2 uv = -1.0*(fragCoord.xy / iResolution.xy*2. - 1.); 
	uv.y *= iResolution.y / iResolution.x;
	vec3 from = vec3(0., -.7, -25.  );
	vec3 dir = normalize(vec3(uv, 1.));
	rot = mat2(cos(-.5), sin(-.5), -sin(-.5), cos(-.5));
	dir.yz = dir.yz*rot;
	from.yz = from.yz*rot; 
	vec3 col = raymarch(from, dir, fragCoord);
	col *= max(0., .1 - length(uv*.05)) / .1;
	fragColor = vec4(col, 1.);

}
 



float de(vec3 p) { 
	p = objectScale* (p + objectTranslate)* objectRotation; 
	float l;
	for (int i = 0; i<Iterations; i++) {
		p.xy = abs(p.xy);
		if (i % 3 == 0) {
			p = p * Scale + Julias[0];
			p *= rots[0];
		}
		else if (i % 3 == 1) {
			p = p * Scale + Julias[1];
			p *= rots[1];

		}
		else  { 
			p = p * Scale + Julias[2];
			p *= rots[2];
		}

	}
	l = length(p); 
	return l*pow(-Scale, -float(Iterations));
}



// ==================== END OF SHADER CODE PASTE =======================


);


#define DEBUG_

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////
ShaderMaker::ShaderMaker() :CFreeFrameGLPlugin()
{

#ifdef DEBUG
	// Debug console window so printf works
	FILE* pCout; // should really be freed on exit 
	AllocConsole();
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	printf("Shader Maker Vers 1.004\n");
	printf("GLSL version [%s]\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif

	printf("id: '%s' name: '%s'", PluginInfo.GetPluginInfo()->PluginUniqueID, PluginInfo.GetPluginInfo()->PluginName);
	printf(" version: '%i.%i'\n", PluginInfo.GetPluginExtendedInfo()->PluginMajorVersion, PluginInfo.GetPluginExtendedInfo()->PluginMinorVersion);

	// Input properties allow for no texture or for four textures
	SetMinInputs(0);
	SetMaxInputs(2); // TODO - 4 inputs

	// Parameters
//	SetParamInfo(FFPARAM_SPEED,         "Speed",         FF_TYPE_STANDARD, 0.0f); m_UserSpeed = 0.5f;
//	SetParamInfo(FFPARAM_MOUSEX,        "X mouse",       FF_TYPE_STANDARD, 0.5f); m_UserMouseX = 0.5f;
//	SetParamInfo(FFPARAM_MOUSEY,        "Y mouse",       FF_TYPE_STANDARD, 0.5f); m_UserMouseY = 0.5f;
//	SetParamInfo(FFPARAM_MOUSELEFTX,    "X mouse left",  FF_TYPE_STANDARD, 0.5f); m_UserMouseLeftX = 0.5f;
//	SetParamInfo(FFPARAM_MOUSELEFTY,    "Y mouse left",  FF_TYPE_STANDARD, 0.5f); m_UserMouseLeftY = 0.5f;
//	SetParamInfo(FFPARAM_RED,           "Red",           FF_TYPE_STANDARD, 0.5f); m_UserRed = 0.5f;
	
//	SetParamInfo(FFPARAM_GREEN,         "Green",         FF_TYPE_STANDARD, 0.5f); m_UserGreen = 0.5f;
//	SetParamInfo(FFPARAM_BLUE,          "Blue",          FF_TYPE_STANDARD, 0.5f); m_UserBlue = 0.5f;
//	SetParamInfo(FFPARAM_ALPHA,         "Alpha",         FF_TYPE_STANDARD, 1.0f); m_UserAlpha = 1.0f;



	SetParamInfo(FFPARAM_VECTOR1_X, "kaliScale", FF_TYPE_STANDARD, 0.4f);
	SetParamInfo(FFPARAM_VECTOR1_Y, "Object X", FF_TYPE_STANDARD, 0.5f);
	SetParamInfo(FFPARAM_VECTOR1_Z, "Object Y", FF_TYPE_STANDARD, 0.5f);
	SetParamInfo(FFPARAM_VECTOR1_W, "Object Z", FF_TYPE_STANDARD, 0.5f);


	SetParamInfo(FFPARAM_VECTOR5_X, "Object Scale ", FF_TYPE_STANDARD, 0.45f);
	SetParamInfo(FFPARAM_VECTOR5_Y, "Object Inclination", FF_TYPE_STANDARD, 0.2f);
	SetParamInfo(FFPARAM_VECTOR5_Z, "Object Elevation", FF_TYPE_STANDARD, 0.1f);


	SetParamInfo(FFPARAM_VECTOR2_X, "Julia1 X", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR2_Y, "Julia1 Y", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR2_Z, "Julia1 Z", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR2_W, "Angle 1", FF_TYPE_STANDARD, 0.86f);

	SetParamInfo(FFPARAM_VECTOR6_X, "Azimut 1", FF_TYPE_STANDARD, 0.1f);
	SetParamInfo(FFPARAM_VECTOR6_Y, "Latitude 1", FF_TYPE_STANDARD, 0.1f);


	SetParamInfo(FFPARAM_VECTOR3_X, "Julia2 X", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR3_Y, "Julia2 Y", FF_TYPE_STANDARD, 0.65f); 
	SetParamInfo(FFPARAM_VECTOR3_Z, "Julia2 Z", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR3_W, "Angle 2", FF_TYPE_STANDARD, 0.3f);


	SetParamInfo(FFPARAM_VECTOR4_X, "Julia 3 X", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR4_Y, "Julia 3 Y", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR4_Z, "Julia 3 Z", FF_TYPE_STANDARD, 0.65f);
	SetParamInfo(FFPARAM_VECTOR4_W, "Angle 3", FF_TYPE_STANDARD, 0.26f);


	//SetParamInfo(FFPARAM_VECTOR5_W, "-------------", FF_TYPE_STANDARD, 0.5f);


	SetParamInfo(FFPARAM_VECTOR6_Z, "Azimut 2", FF_TYPE_STANDARD, 0.2f);
	SetParamInfo(FFPARAM_VECTOR6_W, "Latitude 2", FF_TYPE_STANDARD, 0.2f);

	SetParamInfo(FFPARAM_VECTOR7_X, "Azimut 3", FF_TYPE_STANDARD, 0.86f);
	SetParamInfo(FFPARAM_VECTOR7_Y, "Latitude 3", FF_TYPE_STANDARD, 0.4f);
	//SetParamInfo(FFPARAM_VECTOR7_Z, "------------ 7 Z", FF_TYPE_STANDARD, 0.5f);
	//SetParamInfo(FFPARAM_VECTOR7_W, "------------ 7", FF_TYPE_STANDARD, 0.5f);

	SetParamInfo(FFPARAM_COLOR1_RED, "Color 1 Red", FF_TYPE_RED, 1.0f);
	SetParamInfo(FFPARAM_COLOR1_GREEN, "Color 1 Green", FF_TYPE_GREEN, 1.0f);
	SetParamInfo(FFPARAM_COLOR1_BLUE, "Color 1 Blue", FF_TYPE_BLUE, 1.0f);
 // SetParamInfo(FFPARAM_COLOR1_ALPHA, "Color 1 Alpha", FF_TYPE_STANDARD, 0.50f);

	SetParamInfo(FFPARAM_COLOR2_RED, "Color 2 Red", FF_TYPE_RED, 1.0f);
	SetParamInfo(FFPARAM_COLOR2_GREEN, "Color 2 Green", FF_TYPE_GREEN, 0.0f);
	SetParamInfo(FFPARAM_COLOR2_BLUE, "Color 2 Blue", FF_TYPE_BLUE, 0.0f);
//	SetParamInfo(FFPARAM_COLOR2_ALPHA, "Color 1 Alpha", FF_TYPE_STANDARD, 0.50f);


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

	// Start the frame counter
	m_frame = 0.0f;

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

		// ShaderToy new uniforms
		m_frame = m_frame + 1.0f;
		m_timedelta = (float)(elapsedTime - lastTime); // seconds ?
		m_framerate = 1.0f / m_timedelta;
		m_samplerate = 44100.0f; // default

		// Just pass elapsed time for individual channel times
		m_channelTime[0] = m_time;
		m_channelTime[1] = m_time;
		m_channelTime[2] = m_time;
		m_channelTime[3] = m_time;

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

		// Elapsed time
		if(m_timeLocation >= 0) 
			m_extensions.glUniform1fARB(m_timeLocation, m_time);
	
		// ===========================================================
		// ShaderToy new uniforms
		// iTime - iGlobalTime

		// iFrame - frame number
		if (m_frameLocation >= 0)
			m_extensions.glUniform1fARB(m_frameLocation, m_frame);

		if (m_timedeltaLocation >= 0)
			m_extensions.glUniform1fARB(m_timedeltaLocation, m_timedelta);

		if (m_framerateLocation >= 0)
			m_extensions.glUniform1fARB(m_framerateLocation, m_framerate);

		if (m_samplerateLocation >= 0)
			m_extensions.glUniform1fARB(m_samplerateLocation, m_samplerate);
		// ===========================================================

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
		if (m_inputColourLocation >= 0)
			m_extensions.glUniform4fARB(m_inputColourLocation, m_UserRed, m_UserGreen, m_UserBlue, m_UserAlpha);

		if (m_inputVector1Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector1Location, m_vector1.x, m_vector1.y, m_vector1.z, m_vector1.w);
		if (m_inputVector2Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector2Location, m_vector2.x, m_vector2.y, m_vector2.z, m_vector2.w);
		if (m_inputVector3Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector3Location, m_vector3.x, m_vector3.y, m_vector3.z, m_vector3.w);
		if (m_inputVector4Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector4Location, m_vector4.x, m_vector4.y, m_vector4.z, m_vector4.w);
		if (m_inputVector5Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector5Location, m_vector5.x, m_vector5.y, m_vector5.z, m_vector5.w);
		if (m_inputVector6Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector6Location, m_vector6.x, m_vector6.y, m_vector6.z, m_vector6.w);
		if (m_inputVector7Location >= 0)
			m_extensions.glUniform4fARB(m_inputVector7Location, m_vector7.x, m_vector7.y, m_vector7.z, m_vector7.w);
		if (m_inputColor1Location >= 0)
			m_extensions.glUniform4fARB(m_inputColor1Location, m_color1.x, m_color1.y, m_color1.z, m_color1.w );
		if (m_inputColor2Location >= 0)
			m_extensions.glUniform4fARB(m_inputColor2Location, m_color2.x, m_color2.y, m_color2.z, m_color2.w );


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

		case FFPARAM_SPEED:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed*100.0));
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

		case FFPARAM_SPEED:
			return  m_UserSpeed;
	
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

		case FFPARAM_VECTOR1_X:
			return m_vector1.x;

		case FFPARAM_VECTOR1_Y:
			return m_vector1.y;

		case FFPARAM_VECTOR1_Z:
			return m_vector1.z;

		case FFPARAM_VECTOR1_W:
			return m_vector1.w;

		case FFPARAM_VECTOR2_X:
			return m_vector2.x;

		case FFPARAM_VECTOR2_Y:
			return m_vector2.y;

		case FFPARAM_VECTOR2_Z:
			return m_vector2.z;

		case FFPARAM_VECTOR2_W:
			return m_vector2.w;


		case FFPARAM_VECTOR3_X:
			return m_vector3.x;

		case FFPARAM_VECTOR3_Y:
			return m_vector3.y;

		case FFPARAM_VECTOR3_Z:
			return m_vector3.z;

		case FFPARAM_VECTOR3_W:
			return m_vector3.w;



		case FFPARAM_VECTOR4_X:
			return m_vector4.x;

		case FFPARAM_VECTOR4_Y:
			return m_vector4.y;

		case FFPARAM_VECTOR4_Z:
			return m_vector4.z;

		case FFPARAM_VECTOR4_W:
			return m_vector4.w;

		case FFPARAM_VECTOR5_X:
			return m_vector5.x;

		case FFPARAM_VECTOR5_Y:
			return m_vector5.y;

		case FFPARAM_VECTOR5_Z:
			return m_vector5.z;

		case FFPARAM_VECTOR5_W:
			return m_vector5.w;
		case FFPARAM_VECTOR6_X:
			return m_vector6.x;

		case FFPARAM_VECTOR6_Y:
			return m_vector6.y;

		case FFPARAM_VECTOR6_Z:
			return m_vector6.z;

		case FFPARAM_VECTOR6_W:
			return m_vector6.w;
		case FFPARAM_VECTOR7_X:
			return m_vector7.x;

		case FFPARAM_VECTOR7_Y:
			return m_vector7.y;

		case FFPARAM_VECTOR7_Z:
			return m_vector7.z;

		case FFPARAM_VECTOR7_W:
			return m_vector7.w;

		case FFPARAM_COLOR1_RED:
			return m_color1.x;
		case FFPARAM_COLOR1_GREEN:
			return m_color1.y;
		case FFPARAM_COLOR1_BLUE:
			return m_color1.z;
		case FFPARAM_COLOR1_ALPHA:
			return m_color1.w;

		case FFPARAM_COLOR2_RED:
			return m_color2.x;
		case FFPARAM_COLOR2_GREEN:
			return m_color2.y;
		case FFPARAM_COLOR2_BLUE:
			return m_color2.z;
		case FFPARAM_COLOR2_ALPHA:
			return m_color2.w;


		default:
			return FF_FAIL;
	}
}

FFResult ShaderMaker::SetFloatParameter(unsigned int index, float value)
{
		switch (index) {

			case FFPARAM_SPEED:
				m_UserSpeed = value;
				break;

			case FFPARAM_MOUSEX:
				m_UserMouseX = value;
				break;

			case FFPARAM_MOUSEY:
				m_UserMouseY = value;
				break;

			case FFPARAM_MOUSELEFTX:
				m_UserMouseLeftX = value;
				break;

			case FFPARAM_MOUSELEFTY:
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

			case FFPARAM_VECTOR1_X:
				m_vector1.x = value;
				break;

			case FFPARAM_VECTOR1_Y:
				m_vector1.y = value;
				break;

			case FFPARAM_VECTOR1_Z:
				m_vector1.z = value;
				break;

			case FFPARAM_VECTOR1_W:
				m_vector1.w = value;
				break;


			case FFPARAM_VECTOR2_X:
				m_vector2.x = value;
				break;

			case FFPARAM_VECTOR2_Y:
				m_vector2.y = value;
				break;

			case FFPARAM_VECTOR2_Z:
				m_vector2.z = value;
				break;

			case FFPARAM_VECTOR2_W:
				m_vector2.w = value;
				break;


			case FFPARAM_VECTOR3_X:
				m_vector3.x = value;
				break;

			case FFPARAM_VECTOR3_Y:
				m_vector3.y = value;
				break;

			case FFPARAM_VECTOR3_Z:
				m_vector3.z = value;
				break;

			case FFPARAM_VECTOR3_W:
				m_vector3.w = value;
				break;



			case FFPARAM_VECTOR4_X:
				m_vector4.x = value;
				break;

			case FFPARAM_VECTOR4_Y:
				m_vector4.y = value;
				break;

			case FFPARAM_VECTOR4_Z:
				m_vector4.z = value;
				break;

			case FFPARAM_VECTOR4_W:
				m_vector4.w = value;
				break;

			case FFPARAM_VECTOR5_X:
				m_vector5.x = value;
				break;

			case FFPARAM_VECTOR5_Y:
				m_vector5.y = value;
				break;

			case FFPARAM_VECTOR5_Z:
				m_vector5.z = value;
				break;

			case FFPARAM_VECTOR5_W:
				m_vector5.w = value;
				break;

			case FFPARAM_VECTOR6_X:
				m_vector6.x = value;
				break;

			case FFPARAM_VECTOR6_Y:
				m_vector6.y = value;
				break;

			case FFPARAM_VECTOR6_Z:
				m_vector6.z = value;
				break;

			case FFPARAM_VECTOR6_W:
				m_vector6.w = value;
				break;

			case FFPARAM_VECTOR7_X:
				m_vector7.x = value;
				break;

			case FFPARAM_VECTOR7_Y:
				m_vector7.y = value;
				break;

			case FFPARAM_VECTOR7_Z:
				m_vector7.z = value;
				break;

			case FFPARAM_VECTOR7_W:
				m_vector7.w = value;
				break;

			case FFPARAM_COLOR1_RED:
				// printf("Color 1 red %f\n", value);
				m_color1.x = value;
				break;
			case FFPARAM_COLOR1_GREEN:
				m_color1.y = value;
				break;
			case FFPARAM_COLOR1_BLUE:
				m_color1.z = value;
				break;

			case FFPARAM_COLOR1_ALPHA:
				m_color1.w = value;
				break;

			case FFPARAM_COLOR2_RED:
				m_color2.x = value;
				break;
			case FFPARAM_COLOR2_GREEN:
				m_color2.y = value;
				break;
			case FFPARAM_COLOR2_BLUE:
				m_color2.z = value;
				break;
			case FFPARAM_COLOR2_ALPHA:
				m_color2.w = value;
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
	m_dateYear             = 0.0;
	m_dateMonth            = 0.0;
	m_dateDay              = 0.0;
	m_dateTime             = 0.0;

	m_vector1.x = 0.0;
	m_vector1.y = 0.0;
	m_vector1.z = 0.0;
	m_vector1.w = 0.0;

	m_vector2.x = 0.0;
	m_vector2.y = 0.0;
	m_vector2.z = 0.0;
	m_vector2.w = 0.0;


	m_vector3.x = 0.0;
	m_vector3.y = 0.0;
	m_vector3.z = 0.0;
	m_vector3.w = 0.0;


	m_vector4.x = 0.0;
	m_vector4.y = 0.0;
	m_vector4.z = 0.0;
	m_vector4.w = 0.0;



	m_vector5.x = 0.0;
	m_vector5.y = 0.0;
	m_vector5.z = 0.0;
	m_vector5.w = 0.0;


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
		static char *extraUniforms = { "uniform vec4 inputColour;\nuniform vec4 inputVector1; \nuniform vec4 inputVector2; \nuniform vec4 inputVector3; \nuniform vec4 inputVector4; \nuniform vec4 inputVector5;\n\nuniform vec4 inputVector6; \nuniform vec4 inputVector7; " };
		
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
			//
			// 21.11.17 - new SharedToy uniforms
			// uniform float iTime;			// same as IGlobalTime - current time (in seconds)
			// uniform float iTimeDelta;	// _deltaTime
			// uniform float iFrame;		// Frame number ?
			// uniform float iFrameRate;	// 1.f / _deltaTime
			// uniform float iSampleRate;	// Audio
			//
			static char *uniforms = { "uniform vec3 iResolution;\n"
									  "uniform float iGlobalTime;\n"
									  "uniform float iTime;\n"
									  "uniform float iTimeDelta;\n"
									  "uniform float iFrame;\n"
									  "uniform float iFrameRate;\n"
									  "uniform float iSampleRate;\n"
									  "uniform vec4 inputColor1;\n"
									  "uniform vec4 inputColor2;\n"
									  "uniform vec4 iMouse;\n"
									  "const float PI=3.14159265359;\n"
									  "uniform vec4 iDate;\n"
									  "uniform float iChannelTime[4];\n"
									  "uniform vec3 iChannelResolution[4];\n"
									  "uniform sampler2D iChannel0;\n"
									  "uniform sampler2D iChannel1;\n"
									  "uniform sampler2D iChannel2;\n"
									  "uniform sampler2D iChannel3;\n" };
			
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

				// ===========================================================
				// ShaderToy new uniforms
				m_frameLocation				= -1; // iFrame - frame number
				m_timedeltaLocation			= -1; // iTimeDelta - time elapsed since last frame
				m_framerateLocation			= -1; // iFrameRate - 1.f / _deltaTime
				m_samplerateLocation		= -1; // iSampleRate - 44100.f default
				// ===========================================================

				// Extras
				// Input colour is linked to the user controls Red, Green, Blue, Alpha
				m_inputColourLocation = -1;

				// Extras Input Vectors
				// Input is linked to inputVector1..4
				m_inputVector1Location = -1;
				m_inputVector2Location = -1;
				m_inputVector3Location = -1;
				m_inputVector4Location = -1;
				m_inputVector5Location = -1;
				m_inputVector6Location = -1;
				m_inputVector7Location = -1;

					m_inputColor1Location =-1; 
					m_inputColor2Location = -1;
				// ===========================================================
				// ShaderToy new uniforms
				m_frameLocation				= -1; // iFrame - frame number
				m_timedeltaLocation			= -1; // iTimeDelta - time elapsed since last frame
				m_framerateLocation			= -1; // iFrameRate - 1.f / _deltaTime
				m_samplerateLocation		= -1; // iSampleRate - 44100.f default
				// ===========================================================

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
				if(m_timeLocation < 0) {
					m_timeLocation = m_shader.FindUniform("iGlobalTime");
					// iTime = iGlobalTime
					if (m_timeLocation < 0)
						m_timeLocation = m_shader.FindUniform("iTime");
				}

				// ===========================================================
				// ShaderToy new uniforms
				//
				// iTime = iGlobalTime

				// iFrame - frame number (integer saved as float)
				if (m_frameLocation < 0)
					m_frameLocation = m_shader.FindUniform("iFrame");

				if (m_timedeltaLocation < 0)
					m_timedeltaLocation = m_shader.FindUniform("iTimeDelta");

				if (m_framerateLocation < 0)
					m_framerateLocation = m_shader.FindUniform("iFrameRate");

				if (m_samplerateLocation > 0)
					m_samplerateLocation = m_shader.FindUniform("iSampleRate");
				// ===========================================================

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
				if (m_inputColourLocation < 0)
					m_inputColourLocation = m_shader.FindUniform("inputColour");


				if (m_inputVector1Location  < 0)
					m_inputVector1Location = m_shader.FindUniform("inputVector1");

				if (m_inputVector2Location  < 0)
					m_inputVector2Location = m_shader.FindUniform("inputVector2");

				if (m_inputVector3Location  < 0)
					m_inputVector3Location = m_shader.FindUniform("inputVector3");

				if (m_inputVector4Location  < 0)
					m_inputVector4Location = m_shader.FindUniform("inputVector4");

				if (m_inputVector5Location  < 0)
					m_inputVector5Location = m_shader.FindUniform("inputVector5");

				if (m_inputVector6Location  < 0)
					m_inputVector6Location = m_shader.FindUniform("inputVector6");

				if (m_inputVector7Location  < 0)
					m_inputVector7Location = m_shader.FindUniform("inputVector7");

				if (m_inputColor1Location  < 0)
					m_inputColor1Location = m_shader.FindUniform("inputColor1");

				if (m_inputColor2Location  < 0)
					m_inputColor2Location = m_shader.FindUniform("inputColor2");

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

				// Start the frame counter
				m_frame = 0.0f;

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

