

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

	void setHorizonmtalBlur();

	void setVerticalBlur();

private:


	GLuint m_inputTextureLocation;
	GLuint m_biasLocation;
	GLuint m_maxCoordsLocation;
	GLuint m_resolutionLocation;
	GLuint m_horizvertiLocation;
};
