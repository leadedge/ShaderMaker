

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
	  

	GLuint m_exposureLocation;
	GLuint m_decay2Location;
	GLuint m_densityLocation;
	GLuint m_weightLocation;
	GLuint m_inputTextureLocation;
	GLuint m_biasLocation;
	GLuint m_maxCoordsLocation;
	GLuint m_resolutionLocation;
	GLuint m_horizvertiLocation;
	GLuint m_lightposLocation;
};
