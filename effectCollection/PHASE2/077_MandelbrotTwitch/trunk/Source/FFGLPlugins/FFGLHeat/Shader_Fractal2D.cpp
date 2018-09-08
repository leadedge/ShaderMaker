#include "Shader_Fractal2D.h"
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


	"}";


char *fragmentShaderCodeBlur=  
"float PI=3.14159265359;"
"uniform float aspectRatio;"
"uniform sampler2D inputTexture;"
"uniform sampler1D heatTexture;"
"uniform vec2 maxCoords;"
"uniform vec4 sineFactors;"
"uniform vec4 sineFactors2;"
"uniform float heatAmount;"
"uniform float time;" 
"uniform vec3 angles;"
"uniform vec4 seed1;"
"uniform vec4 seed2;"
"uniform vec4 seed3;"
"uniform vec4 location;"
"uniform vec3 hsvScale;"
"uniform int  modStep;"
"uniform float imageBlend;"
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
"vec4 alphaBlend(vec4 col1,vec4 col2){"
"vec3 newcolor=col1.xyz+col2.xyz*col2.w;"
" return clamp(vec4(newcolor,col1.w+(1-col1.w)*col2.w),0,1);"
"}"
"vec2 kali(vec2 z,vec2 c,float Scaling,float MinRadius) {"
"	float m =dot(z,z);"
"	if (m<MinRadius) {"
"		z = abs(z)/(MinRadius*MinRadius);"
"	}else {"
"		z = abs(z)/m*Scaling;"
"	}"
"	return z+c;"
"}"
"vec2 mandelbrot(vec2 z, vec2 c,float par1,float par2){"
"   return vec2((z.x * z.x - z.y * z.y),(z.y * z.x *2))+c ;"
"}"
"float iterate(vec2 uv,vec4 inputcolor){"
"	  vec2 z, c;"
"c=uv;"
"	    int i;"
"	    z = c;"
" float maxIter=50;" 
"float mean=0.0;"
"	    for(i=0; i<maxIter; i++) {"  
" int modValue=i%modStep;"
"	        if(modValue<(modStep/3.0)){"
"	        	z=kali(z,seed1.xy,seed1.z,seed1.w);"
"	        }else  if(modValue<(modStep/3.0)*2){"
"	        	z=kali(z,seed2.xy,seed1.z,seed1.w);"
"	        }else{"
"	        	z=kali(z,seed3.xy,seed1.z,seed1.w);"
"	        }"
"z+=inputcolor.xy*imageBlend;"
" mean+=length(z);"
"	        if(dot(z,z) > 16.0){"
"	break;"
"}"
"	    }"
"	mean/=float(maxIter);"
"		float ci =  1.0 - log(.5*log(mean ));"
"return ci;"
"	    return length(z) ;"
"}"
"mat2 rot2d(float a) {"
"	return mat2(cos(a),sin(a),-sin(a),cos(a));"
"}"
"const vec4 grayScaleWeights = vec4(0.30, 0.59, 0.11, 0.0);"
"void main() "
"{"
"   vec2 texCoord = gl_TexCoord[1].st;" //lookup input color
"   vec4 c = texture2D(inputTexture, texCoord);"
"	texCoord=(texCoord/maxCoords)*2-1;"
"   texCoord.y*=aspectRatio;" 
"   texCoord*=location.z;"
"   texCoord+=location.xy;"
"   texCoord=texCoord*rot2d(location.w);"
" float itercount=iterate(texCoord*2,c);"
"   vec4 col_out=vec4(hsv2rgb(vec3(itercount*hsvScale.x,(itercount*hsvScale.y),(itercount*hsvScale.z))),1) ;"    
"  col_out.w=1;"
"   gl_FragColor = mix(col_out,c,imageBlend);"
"}";



// Initialise With the Extension, shader sources are contained within class
void ShaderBlur::createShaders(FFGLExtensions *e)
{

	SetExtensions(e);
	Compile(vertexShaderCodeBlur,fragmentShaderCodeBlur);

	//  Compile(vertexShaderCode,fragmentShaderCodeDownscaleBias);
	 



	// Fractal uniforms
  m_inputTextureLocation = FindUniform("inputTexture");
  m_heatTextureLocation = FindUniform("heatTexture");
  m_maxCoordsLocation = FindUniform("maxCoords"); 
  m_timeLocation = FindUniform("time"); 
  m_aspectLocation= FindUniform("aspectRatio");
 
  m_seed1Location=FindUniform("seed1");
  m_seed2Location=FindUniform("seed2");
  m_seed3Location=FindUniform("seed3");
  m_locationLocation=FindUniform("location");
  m_HSVScaleLocation=FindUniform("hsvScale");
  m_modstepLocation=FindUniform("modStep");
  m_imageblendLocation=FindUniform("imageBlend");


   
	// MessageBox(NULL,numstr,NULL,MB_OK);
	if(m_linkStatus==GL_FALSE){
	
	  MessageBox(NULL,"Fehler",NULL,MB_OK);
	}
	
	BindShader();
	// Set Defaults
 	m_extensions->glUniform1iARB(m_inputTextureLocation, 1);  
//	m_extensions->glUniform1fARB(m_biasLocation, 0.5f);
} 

void ShaderBlur::setResolution(int width,int height)
{
	
//	m_extensions->glUniform2fARB(m_resolutionLocation, (float) width, (float) height);

}
void ShaderBlur::setMaxCoords(FFGLTexCoords maxCoords){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	m_extensions->glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
}

void ShaderBlur::setLuminanceBias(float newBias){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
//	m_extensions->glUniform1fARB(m_biasLocation, newBias); 
}

 
 
	void ShaderBlur::setLightPos(float x,float y){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//	m_extensions->glUniform2fARB(m_lightposLocation,x,y); 
} 
	void ShaderBlur::setDecay(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//	m_extensions->glUniform1fARB(m_decay2Location,newVal); 
}  
	void ShaderBlur::setDensity(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//	m_extensions->glUniform1fARB(m_densityLocation,newVal); 
}  
	void ShaderBlur::setWeight(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//	m_extensions->glUniform1fARB(m_weightLocation,newVal); 
}  
	void ShaderBlur::setExposure(float newVal){

	//assign the maxCoords value in the shader
	//(glUniform2f assigns to a vec2)
	//	m_extensions->glUniform1fARB(m_exposureLocation,newVal); 
}   