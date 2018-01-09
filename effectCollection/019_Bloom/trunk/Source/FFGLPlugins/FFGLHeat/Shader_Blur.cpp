#include "Shader_Blur.h"
#include <stdio.h>
char *vertexShaderCodeBlur =
"varying vec2 v_blurTexCoords[14];"
"uniform vec2 iResolution;"
"uniform vec2 horizVertical;"
	"void main()"
	"{"
	"  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
	"  gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;"
	"  gl_FrontColor = gl_Color;"
	" vec2 pixel=1.0/iResolution;"
" v_blurTexCoords[ 0].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*7, -pixel.y*7)*horizVertical;"
"    v_blurTexCoords[ 1].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*6,-pixel.y*6)*horizVertical;"
"    v_blurTexCoords[ 2].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*5, -pixel.y*5)*horizVertical;"
"    v_blurTexCoords[ 3].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*4, -pixel.y*4)*horizVertical;"
"    v_blurTexCoords[ 4].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*3, -pixel.y*3)*horizVertical;"
"    v_blurTexCoords[ 5].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*2, -pixel.y*2)*horizVertical;"
"    v_blurTexCoords[ 6].xy = gl_MultiTexCoord1.xy + vec2(-pixel.x*1, -pixel.y*1)*horizVertical;"
"    v_blurTexCoords[ 7].xy = gl_MultiTexCoord1.xy + vec2( pixel.x*1, pixel.y*1)*horizVertical;"
"    v_blurTexCoords[ 8].xy = gl_MultiTexCoord1.xy + vec2( pixel.x*2, pixel.y*2)*horizVertical;"
"    v_blurTexCoords[ 9].xy = gl_MultiTexCoord1.xy + vec2(pixel.x*3, pixel.y*3)*horizVertical;"
"    v_blurTexCoords[10].xy = gl_MultiTexCoord1.xy + vec2( pixel.x*4, pixel.y*4)*horizVertical;"
"    v_blurTexCoords[11].xy = gl_MultiTexCoord1.xy + vec2( pixel.x*5, pixel.y*5)*horizVertical;"
"    v_blurTexCoords[12].xy = gl_MultiTexCoord1.xy + vec2( pixel.x*6, pixel.y*6)*horizVertical;"
"    v_blurTexCoords[13].xy = gl_MultiTexCoord1.xy + vec2( pixel.x*7, pixel.y*7)*horizVertical;"


	"}";


char *fragmentShaderCodeBlur= 
"varying vec2 v_blurTexCoords[14];"
	"uniform float aspectRatio;"
	"uniform sampler2D iChannel0;"  
	"uniform vec2 maxCoords;" 
	"uniform float luminanceBiasMin;" 

	"\n"
"\tvec3 refColorHSV=vec3(0.5,1.0,1.0);\n"
"\tfloat range=0.75;\n"
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
" float getBlurStrength(vec3 color){\n"
" \n"
" vec3 hsv=rgb2hsv(color.xyz);\n"
" vec3 diff=abs(refColorHSV-hsv);\n"
" if(length(diff)<range){\n"
" return 1.0;\n"
" }else{\n"
"\t return 1.0;\n"
" }\n"
" \n"
" }\n"
""
"vec4 getTexture(vec2 texCoord){"

"    vec4 result= texture2D(iChannel0,texCoord);"
"return result*getBlurStrength(result.xyz);"
"}"

	"void main() "
	"{"
	
	"vec2 uv = gl_TexCoord[1].st;" 
"	   gl_FragColor = vec4(0.0);"
"    gl_FragColor += getTexture( v_blurTexCoords[ 0])*0.0044299121055113265;"
"    gl_FragColor += getTexture( v_blurTexCoords[ 1])*0.00895781211794;"
"    gl_FragColor += getTexture( v_blurTexCoords[ 2])*0.0215963866053;"
"    gl_FragColor += getTexture(v_blurTexCoords[ 3])*0.0443683338718;"
"    gl_FragColor += getTexture(v_blurTexCoords[ 4])*0.0776744219933;"
"    gl_FragColor += getTexture(v_blurTexCoords[ 5])*0.115876621105;"
"    gl_FragColor += getTexture(v_blurTexCoords[ 6])*0.147308056121;"
"    gl_FragColor += getTexture(uv         )*0.159576912161;"
"    gl_FragColor += getTexture(v_blurTexCoords[ 7])*0.147308056121;"
"    gl_FragColor += getTexture(v_blurTexCoords[ 8])*0.115876621105;"
"    gl_FragColor += getTexture( v_blurTexCoords[ 9])*0.0776744219933;"
"    gl_FragColor += getTexture(v_blurTexCoords[10])*0.0443683338718;"
"    gl_FragColor += getTexture(v_blurTexCoords[11])*0.0215963866053;"
"    gl_FragColor += getTexture(v_blurTexCoords[12])*0.00895781211794;"
"    gl_FragColor += getTexture(v_blurTexCoords[13])*0.0044299121055113265;" 
	"}";



// Initialise With the Extension, shader sources are contained within class
void ShaderBlur::createShaders(FFGLExtensions *e)
{

	SetExtensions(e);
	Compile(vertexShaderCodeBlur,fragmentShaderCodeBlur);

	//  Compile(vertexShaderCode,fragmentShaderCodeDownscaleBias);

	m_biasLocation= FindUniform("luminanceBiasMin"); 
	m_inputTextureLocation = FindUniform("iChanne1l0");
	m_resolutionLocation = FindUniform("iResolution");

	m_maxCoordsLocation = FindUniform("maxCoords"); 

	m_horizvertiLocation = FindUniform("horizVertical"); 

	char numstr[21]; // enough to hold all numbers up to 64-bits
	sprintf(numstr, "Dingens ist: %d", m_biasLocation);
	// MessageBox(NULL,numstr,NULL,MB_OK);
	if(m_linkStatus==GL_FALSE){
	
	  MessageBox(NULL,"Fehler",NULL,MB_OK);
	}
	
	BindShader();
	// Set Defaults
 	m_extensions->glUniform1iARB(m_inputTextureLocation, 1); 
	m_extensions->glUniform2fARB(m_horizvertiLocation, 1.0f,1.0f); 
//	m_extensions->glUniform1fARB(m_biasLocation, 0.5f);
} 

void ShaderBlur::setResolution(int width,int height)
{
	
	m_extensions->glUniform2fARB(m_resolutionLocation, (float) width, (float) height);

}
void ShaderBlur::setMaxCoords(FFGLTexCoords maxCoords){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
}

void ShaderBlur::setLuminanceBias(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform1fARB(m_biasLocation, newBias); 
}


void  ShaderBlur::setHorizonmtalBlur(){
	m_extensions->glUniform2fARB(m_horizvertiLocation, 1.0f,0.0f); 

}

	void  ShaderBlur::setVerticalBlur()
	{
	
	m_extensions->glUniform2fARB(m_horizvertiLocation, 0.0f,1.0f); 
	
	}