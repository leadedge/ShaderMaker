

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


	
	void setBrightness(float newBias);
	void setPower(float newBias);
	void setPowerAdd(float newBias);
	void setBrightnessAdd(float newBias);
	void setTolerance(float newTolerance);
	void setColor(float red,float green,float blue);


private:


	GLuint m_inputTextureLocation;
	GLuint m_biasLocation;
	GLuint m_maxCoordsLocation;
	GLuint m_BrightnessLocation;
	GLuint m_PowerLocation;
	GLuint m_PowerAddLocation;
	GLuint m_BrigthnessAddLocation;
	GLuint m_ColorLocation;
	GLuint m_toleranceLocation;
};
