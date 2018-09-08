#include "Shader_Downscale.h"
#include <stdio.h>
char *vertexShaderCodeDownscale =
	"void main()"
	"{"
	"  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
	"  gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;"
	"  gl_FrontColor = gl_Color;"
	"}";


char *fragmentShaderCodeDownscale = 
	
	"float PI=3.14159265359;"
	"uniform float aspectRatio;"
	"uniform sampler2D iChannel0;"  
	"uniform vec2 maxCoords;" 
	"uniform float luminanceBiasMin;" 

	" uniform float Brightness;"
" uniform float Power;"
" uniform float PowerAdd;"
" uniform float BrightnessAdd;"
" uniform vec3 Color;"
"\n" 
"\tuniform float tolerance;\n"
"\n"
"\n"
"\n"
"\tvec3 rgb2hsv(vec3 c)\n"
"{\n"
"    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
"    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
"    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"
"\n"
"    float d = q.x - min(q.w, q.y);\n"
"    float e = 1.0e-10;\n"
"    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
"}\n"
"  vec3 hsv2rgb(vec3 c)\n"
"{\n"
"    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
"    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
"    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
"}\n" 
"\n"
" float getBlurStrength(vec3 color){\n"
" \n"
" vec3 hsv=color.xyz;\n"
" vec3 diff=abs(Color-hsv);\n"
" if((diff.x<=tolerance)&&(diff.y<=tolerance)&&(diff.z<=tolerance)){\n"
" return 1.0;\n"
" }else{\n"
"\t return 0.0;\n"
" }\n"
" \n"
" }\n"
""
 

	"void main() "
	"{"
	"vec2 uv = gl_TexCoord[1].st;" 
	"vec4 col = texture2D(iChannel0, uv );"  
"   gl_FragColor =getBlurStrength(col.xyz)*col;" 

	"}";



// Initialise With the Extension, shader sources are contained within class
void ShaderDownscale::createShaders(FFGLExtensions *e)
{

	SetExtensions(e);
	Compile(vertexShaderCodeDownscale,fragmentShaderCodeDownscale);

	//  Compile(vertexShaderCode,fragmentShaderCodeDownscaleBias);

	m_biasLocation= FindUniform("luminanceBiasMin"); 
	m_inputTextureLocation = FindUniform("iChannel0");
	m_maxCoordsLocation = FindUniform("maxCoords"); 

	m_BrightnessLocation = FindUniform("Brightness"); 
	m_PowerLocation = FindUniform("Power"); 
	m_PowerAddLocation = FindUniform("PowerAdd"); 
	m_BrigthnessAddLocation = FindUniform("BrightnessAdd"); 
	m_ColorLocation = FindUniform("Color");   
	m_toleranceLocation = FindUniform("tolerance");   

	char numstr[21]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "Dingens ist: %d", m_biasLocation);
	// MessageBox(NULL,numstr,NULL,MB_OK);
	if(m_linkStatus==GL_FALSE){
	
	  MessageBox(NULL,"Fehler",NULL,MB_OK);
	}
	
	BindShader();
	// Set Default Values
 	m_extensions->glUniform1iARB(m_inputTextureLocation, 0); 
	m_extensions->glUniform1fARB(m_biasLocation, 0.5f);
} 

void ShaderDownscale::setMaxCoords(FFGLTexCoords maxCoords){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
}

void ShaderDownscale::setLuminanceBias(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform1fARB(m_biasLocation, newBias); 
}



	
	void ShaderDownscale::setBrightness(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_BrightnessLocation, newBias); 
}
	void ShaderDownscale::setPower(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_PowerLocation, newBias); 
}
	void ShaderDownscale::setPowerAdd(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform1fARB(m_PowerAddLocation, newBias); 
}
	void ShaderDownscale::setBrightnessAdd(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_BrigthnessAddLocation, newBias); 
}
	void ShaderDownscale::setColor(float red,float green,float blue){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform3fARB(m_ColorLocation, red,green,blue); 
}
	void ShaderDownscale::setTolerance(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_toleranceLocation, newBias); 
}