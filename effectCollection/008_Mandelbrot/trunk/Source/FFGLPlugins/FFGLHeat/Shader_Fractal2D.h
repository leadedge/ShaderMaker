

#include <FFGLShader.h>

#include <FFGL.h>
#include <FFGLLib.h>
 
class ShaderBlur :
	public FFGLShader
{

public:
	// Initialise With the Extension, shader sources are contained within class
	void createShaders(FFGLExtensions *e);
	void setMaxCoords(FFGLTexCoords maxCoords);
	void setResolution(int width,int height);
	
	void setLuminanceBias(float newBias);	
	void setLightPos(float x,float y);	
	void setDecay(float newVal);
	void setDensity(float newVal);
	void setWeight(float newVal);
	void setExposure(float newVal);


private:
	  
	GLint m_inputTextureLocation;
	GLint m_heatTextureLocation;
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation; 
	GLint m_locationLocation; 
	GLint m_imageblendLocation; 	
	GLint m_HSVScaleLocation;
	GLint m_seed1Location;
	GLint m_seed2Location;
	GLint m_seed3Location;
	GLint m_modstepLocation; 
};
