

#include <FFGLShader.h>

#include <FFGL.h>
#include <FFGLLib.h>

class ShaderDownscale :
	public FFGLShader
{

public:
	// Initialise With the Extension, shader sources are contained within class
	void createShaders(FFGLExtensions *e);
	void setMaxCoords(FFGLTexCoords maxCoords);
	
	void setLuminanceBias(float newBias);

	 
	void setLightPos(float x,float y);
	

void setLightRadius(float newBias);

private:

	float lightposx;
float lightposy;


	GLuint m_inputTextureLocation;
	GLuint m_biasLocation;
	GLuint m_maxCoordsLocation; 
	GLuint m_lightposLocation; 
	GLuint m_lightradiusLocation; 
};
