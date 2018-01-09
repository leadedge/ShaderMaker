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

	"uniform float aspectRatio;"
	"uniform sampler2D iChannel0;"  
	"uniform vec2 maxCoords;" 
	"uniform float luminanceBiasMin;" 
	"uniform vec2 lightpos;"
	" uniform float Brightness;"
" uniform float Power;"
" uniform float PowerAdd;"
" uniform float BrightnessAdd;"
" uniform vec3 Color;"
"uniform float radius;"
"float light(){"

"	float val=length( gl_TexCoord[1].st-lightpos);"
" if(val-radius >0)return 0;"
"return 1-val/radius;"
"}"

	"void main() "
	"{"
	"vec2 uv = gl_TexCoord[1].st;" 
	"vec4 col = texture2D(iChannel0, uv );" 
	"float luminance= 0.2126*col.x + 0.7152*col.y + 0.0722*col.z;"
	" if(luminance<luminanceBiasMin){"
	"gl_FragColor=light();"
	"}else{"
"   gl_FragColor =0;"    
	"}"


	"}";



// Initialise With the Extension, shader sources are contained within class
void ShaderDownscale::createShaders(FFGLExtensions *e)
{

	SetExtensions(e);
	Compile(vertexShaderCodeDownscale,fragmentShaderCodeDownscale);

	//  Compile(vertexShaderCode,fragmentShaderCodeDownscaleBias);
	
	m_lightposLocation= FindUniform("lightpos"); 
	m_biasLocation= FindUniform("luminanceBiasMin");  
	m_lightradiusLocation=FindUniform("radius");
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



void ShaderDownscale::setLightRadius(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform1fARB(m_lightradiusLocation, newBias); 
}



	
 
	void ShaderDownscale::setLightPos(float x,float y){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform2fARB(m_lightposLocation,x,y); 
}