#ifndef FFGLHeat_H
#define FFGLHeat_H

#include <FFGLShader.h>
#include "../FFGLPluginSDK.h"
 
struct Point2D{
	float x;
	float y;
};

struct Point3D{
	float x;
	float y;
	float z;
};
struct ColorData{
	float hue;
	float strength;
};
struct AxisAngle{
	float x;
	float y;
	float z;
	float scalex;
	float scaley;
	float scalez;
	float angle;
};

struct Anim{
	float countx;
		float speedx;
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
	
	Anim anim1; 
	
	ColorData colorX;
	ColorData colorY;
	ColorData colorZ;
	
	ColorData colorBase;
	float m_amplitude;
	float m_scale;
	 Point3D m_julia;
	 Point3D m_Eye;
	 Point3D m_Target;
	 Point3D m_Rotation;

	int m_initResources;
	AxisAngle rotAxis1;
	AxisAngle rotAxis2;
	AxisAngle rotAxis3;

	GLuint m_heatTextureId;

	FFGLExtensions m_extensions;
    FFGLShader m_shader;
	GLint m_inputTextureLocation;
	GLint m_heatTextureLocation;
	GLint m_maxCoordsLocation; 
	GLint m_timeLocation; 
	GLint m_aspectLocation; 
	GLint m_locationLocation; 
	  

	
	GLint m_bokehRadiusLocation; 
	GLint m_bokehAmountLocation; 
	GLint m_resolutionLocation;
	GLint m_speedLocation;
	GLint m_rotAngleLocation;
	GLint m_rotAngle2Location;
	GLint m_rotAngle3Location;
	GLint m_colorXLocation;
	GLint m_colorYLocation;
	GLint m_colorZLocation;
	GLint m_colorBaseLocation;
	GLint m_amplitudeLocation;
	GLint m_scaleLocation;
	GLint m_juliaLocation;
	GLint m_targetLocation;
	GLint m_eyeLocation;
	GLint m_rotationLocation;
};


#endif
