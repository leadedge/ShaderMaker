#include "Shader_Blur.h"
#include <stdio.h>


#include "Headers.h"
char *vertexShaderCodeBlur =STRINGIFY(
varying vec2 v_blurTexCoords[14];
uniform vec2 iResolution;
uniform vec2 horizVertical;
	void main()
	{
	  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	  gl_TexCoord[1].xy = gl_MultiTexCoord1.xy;
	  gl_FrontColor = gl_Color;
	 vec2 pixel=1.0/iResolution; 


	}
	);


	char *fragmentShaderCodeBlur=  STRINGIFY(
	uniform float aspectRatio;
	uniform sampler2D iChannel0;  
	uniform vec2 maxCoords; 
	uniform float useAlpha;
	uniform float luminanceBiasMin; 

	
uniform  float exposure;
uniform float decay2;
uniform float density;
uniform float weight;
uniform vec2 lightPositionOnScreen; 
const int NUM_SAMPLES = 150 ;
vec4 godRay(vec2 uv){
		vec2 deltaTextCoord = vec2( uv - lightPositionOnScreen.xy );
	    	vec2 textCoo =uv;
	    	deltaTextCoord *= 1.0 /  float(NUM_SAMPLES) * density;
	    	float illuminationDecay = 1.0;
			vec4 result=vec4(0);
	    	for(int i=0; i < NUM_SAMPLES ; i++)
	        {
	                 textCoo -= deltaTextCoord;
	                  vec4 samplex =texture2D(iChannel0,textCoo);
	                 samplex *= illuminationDecay * weight;
	                 result += samplex;
	                 illuminationDecay *= decay2;
	         }
	    	return result *exposure;
}

	void main() 
	{
	
	vec2 uv = gl_TexCoord[1].st; 
	   gl_FragColor = vec4(0.0); 
    gl_FragColor += godRay(uv)+1.0;  
	 
	}
	
	)
	;



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
	m_lightposLocation = FindUniform("lightPositionOnScreen"); 
	
	m_horizvertiLocation = FindUniform("horizVertical"); 

	
	m_decay2Location = FindUniform("decay2"); 
	m_densityLocation = FindUniform("density"); 
	m_weightLocation = FindUniform("weight"); 
	m_exposureLocation = FindUniform("exposure"); 


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

 
 
	void ShaderBlur::setLightPos(float x,float y){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform2fARB(m_lightposLocation,x,y); 
} 
	void ShaderBlur::setDecay(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_decay2Location,newVal); 
}  
	void ShaderBlur::setDensity(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_densityLocation,newVal); 
}  
	void ShaderBlur::setWeight(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_weightLocation,newVal); 
}  
	void ShaderBlur::setExposure(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
		m_extensions->glUniform1fARB(m_exposureLocation,newVal); 
}   