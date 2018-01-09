#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"

struct SineWave2d{
	float count;
	float frequency;
	float amplitude;
	float wavelength;
	float angle;
};
struct Point2D{
	float x;
	float y;
};

class FFGLSineWave2D :


public CFreeFrameGLPlugin
{
public:
	FFGLSineWave2D();
  virtual ~FFGLSineWave2D() {}

	///////////////////////////////////////////////////
	// FreeFrameGL plugin methods
	///////////////////////////////////////////////////
	
	DWORD	SetParameter(const SetParameterStruct* pParam);		
	DWORD	GetParameter(DWORD dwIndex);					
	DWORD	ProcessOpenGL(ProcessOpenGLStruct* pGL);
  DWORD InitGL(const FFGLViewportStruct *vp);
  DWORD DeInitGL();
  	DWORD SetTime(double time);
	

double GetTimeInSecondsSinceStart();
	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static DWORD __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
  {
  	*ppOutInstance = new FFGLSineWave2D();
	  if (*ppOutInstance != NULL)
      return FF_SUCCESS;
	  return FF_FAIL;
  }
double systemTimeToDouble(SYSTEMTIME *time);
protected:	
	// Time
	double m_Time;
	SYSTEMTIME m_SystemTimeStart;
	SYSTEMTIME m_SystemTimeLastFrame;
	double timeCounter;
	double sinCounter1;
	double sinCounter2;
	double sinCounter3;
	double sinCounter4; 
	float m_sinXSpeed;
	float m_sinXSpeed2;
	float m_sinXVariance; 
	float m_sinYSpeed;
	float m_sinYSpeed2;
	float m_sinYVariance; 
	float m_dotcount;
	float m_dotsize;
	float m_beatdotsize;
	float m_beatdotrange;
	Point2D seed1;
	Point2D seed2;
	
	 SineWave2d sineWaveX1;
	 SineWave2d sineWaveX2;
	 SineWave2d sineWaveX3;
	
	 SineWave2d sineWaveY1;
	 SineWave2d sineWaveY2;
	 SineWave2d sineWaveY3;
	
	int m_initResources;

	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation;
	GLint m_heatTextureLocation;
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation;
	GLint m_sineFactorsLocation;
	GLint m_sineFactors2Location;
	GLint m_dotcountLocation;
	GLint m_dotsizeLocation;
	
	GLint m_beatdotsizeLocation;
	GLint m_beatdotrangeLocation;
	GLint m_sinewaveX1Location;
	GLint m_sinewaveX2Location;
	GLint m_sinewaveX3Location;
	GLint m_sinewaveY1Location;
	GLint m_sinewaveY2Location;
	GLint m_sinewaveY3Location;
	GLint m_anglesLocation;

	
	GLint m_seed1Location;
	GLint m_seed2Location;

};


#endif
