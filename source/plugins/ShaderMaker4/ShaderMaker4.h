#pragma once

#include <Windows.h>
#include <FFGLSDK.h>
#include <vector>

// For resource loading
#include "resource.h" // See shaderfiles.h
#include "SOIL_EXT\src\soil.h" // SOIL_EXT - Simple OpenGL Image Library - http://www.lonesock.net/soil.html

// To get the dll hModule since there is no main
#ifndef _delayimp_h
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif

using namespace ffglex;
using namespace ffglqs;

struct uniform {
	std::string type; // "float" or "bool"
	std::string name; // Uniform name
	float value; // Uniform value
};


class ShaderMaker4: public ffglqs::Plugin
{

public:

	ShaderMaker4();

	// CFFGLPlugin
	FFResult InitGL( const FFGLViewportStruct* vp ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* pGL ) override;
	FFResult DeInitGL() override;

private:

	// Viewport variables for resolution
	float m_vpWidth;
	float m_vpHeight;

	// FFGL user parameters
	float m_UserSpeed;
	float m_UserMouseX;
	float m_UserMouseY;
	float m_UserMouseLeftX;
	float m_UserMouseLeftY;
	float m_UserRed;
	float m_UserGreen;
	float m_UserBlue;
	float m_UserAlpha;

	// Shader uniforms
	float m_time;
	float m_mouseX;
	float m_mouseY;
	float m_mouseLeftX;
	float m_mouseLeftY;
	// Channel playback time (in seconds)
	float m_channelTime[4];
	// Channel resolution in pixels - 4 channels with width, height, depth each
	float m_channelResolution[4][3];
	// Extras
	double m_frame; // frame number
	double m_timedelta; // time elapsed since last frame
	double m_framerate;	// framerate - 1.f / deltaTime
	double m_samplerate;

	// User uniforms
	std::vector<uniform> m_UserUniforms;

	// iChanellResolution Uniform location For glUniform3fv
	GLint m_channelresolutionLocation;

	// The fragment shader sting
	std::string m_shaderString;

	// Parameter pointers
	std::shared_ptr< Param > Speed;
	std::shared_ptr< Param > Xmouse;
	std::shared_ptr< Param > Ymouse;
	std::shared_ptr< Param > XmouseLeft;
	std::shared_ptr< Param > YmouseLeft;
	std::shared_ptr< Param > Red;
	std::shared_ptr< Param > Green;
	std::shared_ptr< Param > Blue;
	std::shared_ptr< Param > Alpha;
	// std::vector< std::shared_ptr<Param> > FFPARAM_UNIFORMS;
	std::vector< std::shared_ptr<Param> > UserUniforms;

	// FFGL shader and quad draw
	ffglex::FFGLShader m_shader;
	ffglex::FFGLScreenQuad m_quad;
	
	// Shader used for texture draw
	ffglex::FFGLShader m_drawShader;

	void BindTextures();
	void UnbindTextures();
	void SetDefaults();
	void Cleanup();
	void StartCounter();
	double GetCounter();
	std::string LoadShaderString(std::string fragmentShader);
	bool LoadShader(std::string shaderString);
	void HandleParams();
	void CreateGLtexture(GLuint &texID, unsigned int width, unsigned int height, void *data);


	// Date
	double m_dateYear;
	double m_dateMonth;
	double m_dateDay;
	double m_dateTime;

	// Time
	double elapsedTime;
	double lastTime;
	double lastProcessTime;
	double processTime;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	// windows
	double PCFreq;
	__int64 CounterStart;
#else
	// posix c++11
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
#endif

	// Functions for resource loading
	HMODULE m_hModule; // A handle to the dll module
	char m_HostName[MAX_PATH];
	std::string m_vertexShaderCode;
	std::string m_fragmentShaderCode;
	HMODULE GetCurrentModule();
	bool LoadShaderResource(std::string &shaderString);
	void CleanShaderString(std::string &shaderString);
	bool LoadResourceImage(GLuint ResourceID, GLuint &TextureID);
	bool FlipVertical(unsigned char *src, unsigned int w, unsigned int h);

	// Textures
	GLuint m_glTexture;
	GLuint m_glTexture0;
	GLuint m_glTexture1;
	GLuint m_glTexture2;
	GLuint m_glTexture3;
	GLuint m_fbo;
	float m_Scale;

	// Flags
	bool bGlslSandbox;
	bool bInitialized;


};
