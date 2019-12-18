//
//		ShaderMaker4
//
//		A source code template that can be used to build FreeframeGL 2.1
//		plugins for Resolume 7 from GLSL Sandbox or ShaderToy shaders.
//		ShaderToy multiple buffers are not supported.
//
//		Revisions :
//		18.12.19	Version 1.000
//
//		------------------------------------------------------------
//
//		Copyright (C) 2019. Lynn Jarvis, Leading Edge. Pty. Ltd.
//
//		This program is free software: you can redistribute it and/or modify
//		it under the terms of the GNU Lesser General Public License as published by
//		the Free Software Foundation, either version 3 of the License, or
//		(at your option) any later version.
//
//		This program is distributed in the hope that it will be useful,
//		but WITHOUT ANY WARRANTY; without even the implied warranty of
//		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//		GNU Lesser General Public License for more details.
//
//		You will receive a copy of the GNU Lesser General Public License along 
//		with this program.  If not, see http://www.gnu.org/licenses/.
//
//		Shaders from the ShaderToy website, are licensed under
//		Attribution - NonCommercial - ShareAlike 3.0 unless otherwise specified.
//		(https://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US)
//
//		GLSL Sandbox is MIT licence.
//		No default licence is documented for submitted shaders
//		but, where specifically indicated, the licencing terms
//		of the author must be observed.
//
//		Include a link and credit to the author in the shader source
//		and in the plugin description or about field where possible. 
//
//		--------------------------------------------------------------
//
#include "ShaderMaker4.h"

//
// Define "EFFECT_PLUGIN" in shaderfiles.h
// to build the plugin as an effect rather than a source
//

// Here you set the name of the plugin that Resolume uses
// You can change that name, but then you also need to change
// the plugin dll name from "ShaderMaker4.dll" to whatever that name is.

// The 4 character plugin ID must also be unique for each plugin
// The description and about fields can also be modified
// but, at this time, Resolume does not show them

static CFFGLPluginInfo PluginInfo(
	PluginFactory< ShaderMaker4 >, // Create method
	"SHM4",                        // Plugin unique ID - make it different for each plugin
	"ShaderMaker4",                // Plugin name - change this as required
	2,                             // API major version number
	1,                             // API minor version number
	1,                             // Plugin major version number
	000,                           // Plugin minor version number
#ifdef EFFECT_PLUGIN
	FF_EFFECT,                     // Plugin is an effect
#else
	FF_SOURCE,                     // Plugin generates the visuals
#endif
	// Plugin description - add your own details
	"FFGL 2.1 plugin built using ShaderMaker4\n(C) 2019 Lynn Jarvis (http://spout.zeal.co/)",
	// Include a link and credit to the shader authors where possible
	"ShaderMaker4"
);


//
// Vertex shader
//
// Modifications from Resolume :
//	"size" is the resolution
//  "uv" is changed to "uvCoord" to avoid conflict in ShaderToy shaders
//
static const char _vertexShaderCode[] = R"(#version 410 core

uniform vec2 size;

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

out vec2 uvCoord;
out vec2 fragCoord;

void main()
{
	gl_Position = vPosition;
	uvCoord = vUV;
    fragCoord = uvCoord * size;
}
)";


//
// Basic draw shader for rendering a texture
//
static const char _drawShaderCode[] = R"(#version 410 core
uniform sampler2D inputTexture;
in vec2 uvCoord;
out vec4 fragColor;
void main()
{
	vec4 color = texture( inputTexture, uvCoord );
	fragColor = color;
}
)";



ShaderMaker4::ShaderMaker4()
{

	/*
	// Console window if you need it so that printf works
	FILE* pCout;
	AllocConsole();
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	printf("ShaderMaker4\n");
	*/

	// Input properties allow for up to two textures for an effect
	// For Resolume only one of these is used unless the plugin is a mix plugin
	// Here we limit to an effect by using only one texture.
#ifdef EFFECT_PLUGIN
	SetMinInputs(1); // Resolume requires a minimum of 1 for an effect
	SetMaxInputs(1); // Second input texture not used
#else
	SetMinInputs(0); // No texture inputs for a source
	SetMaxInputs(0);
#endif

	// Set defaults
	SetDefaults();

	//
	// Plugin parameters
	//

	// Load the fragment shader from resources.
	// The file path is entered in "shaderfiles.h"
	// Images used by the shader are also entered there
	m_hModule = GetCurrentModule(); // A handle to the dll module
	std::string fragmentShader;
	if (LoadShaderResource(fragmentShader)) {
		// Load the shader string now to add ShaderToy uniforms, 
		// make code changes and to get user uniform names (m_UserUniforms)
		m_shaderString = LoadShaderString(fragmentShader);
		if (!m_shaderString.empty()) {
			
			//
			// You can define uniforms within the shader itself
			//
			// for example if you put in the shader :
			//
			//    uniform float myValue
			//
			// ShaderMaker will find the name and it will become a "myValue" control in the plugin.
			// Currently only float (FF_TYPE_STANDARD) or int (FF_TYPE_BOOLEAN) are supported.
			// The range is 0.0-1.0. Default is 0.5 for float and 0 for boolean.
			//
			// The user uniform parameters are added here so they appear first but they can
			// also be last. The controls will appear in the same order as in the shader.
			if (m_UserUniforms.size() > 0) {
				for (int i = 0; i < m_UserUniforms.size(); i++) {
					FFUInt32 type;
					if (m_UserUniforms[i].type == "bool") type = FF_TYPE_BOOLEAN;
					else type = FF_TYPE_STANDARD;
					std::shared_ptr< Param > user = nullptr;
					AddParam(user = Param::Create(m_UserUniforms[i].name.c_str(), m_UserUniforms[i].value));
					UserUniforms.push_back(user);
				}
			}

			// These are the standard set for ShaderToy.
			// Use only the ones you need for a particular shader.
			// Change the names ("Speed" etc.) to whatever you like.
			// The control positions can also be changed here to any order.

			// Animation speed
			AddParam(Speed = Param::Create("Speed", (float)m_UserSpeed));

			// X and Y mouse movement
			AddParam(Xmouse = Param::Create("X mouse", m_UserMouseX));
			AddParam(Ymouse = Param::Create("Y mouse", m_UserMouseY));
			
			// The rest are not commonly used. Activate any that you want.
			
			// Left and Right mouse button down
			// A checkbox is necessary because X and Y mouse controls can't be used at the same time
			// AddParam(XmouseLeft = Param::Create("X mouse left", FF_TYPE_BOOLEAN, m_UserMouseLeftX));
			// AddParam(YmouseLeft = Param::Create("Y mouse left", FF_TYPE_BOOLEAN, m_UserMouseLeftY));
			
			// These are extras that can be used for colour input
			// and are linked to "uniform vec4 inputColour" in the shader
			// You can define them with the Freeframe types
			// FF_TYPE_RED, FF_TYPE_GREEN, FF_TYPE_BLUE and FF_TYPE_ALPHA
			// The display values are 0-225 or 0-100 for alpha
			// but the values returned are 0.0 - 1.0
			// AddParam(Red = Param::Create("Red", FF_TYPE_RED, m_UserRed));
			// AddParam(Green = Param::Create("Green", FF_TYPE_GREEN, m_UserGreen));
			// AddParam(Blue = Param::Create("Blue", FF_TYPE_BLUE, m_UserBlue));
			// AddParam(Alpha = Param::Create("Alpha", FF_TYPE_ALPHA, m_UserAlpha));
			
			// You can use them for any purpose together with a standard slider control
			// AddParam(Red = Param::Create("Red", m_UserRed));
			// AddParam(Green = Param::Create("Green", m_UserGreen));
			// AddParam(Blue = Param::Create("Blue", m_UserBlue));
			// AddParam(Alpha = Param::Create("Alpha", m_UserAlpha));

		}
	}

}

FFResult ShaderMaker4::InitGL( const FFGLViewportStruct* vp )
{
	// The shader string is already loaded,
	// so set up and compile the shader (m_shader)
	if (!LoadShader(m_shaderString))
		return FF_FAIL;

	// Initialize the shader for drawing a texture
	if (!m_drawShader.Compile(_vertexShaderCode, _drawShaderCode)) {
		DeInitGL();
		return FF_FAIL;
	}
	
	bInitialized = true; // Clear to use the shader

	// Images can be loaded for both source and effect plugins.
	// But for an effect the first one is reserved for the host.
	// If images are not defined in "shaderfiles.h" the load is ignored

	// Load images if they have been defined in shaderfiles.h
#ifndef EFFECT_PLUGIN
	LoadResourceImage(IDC_IMAGEFILE0, m_glTexture0);
#endif
	LoadResourceImage(IDC_IMAGEFILE1, m_glTexture1);
	LoadResourceImage(IDC_IMAGEFILE2, m_glTexture2);
	LoadResourceImage(IDC_IMAGEFILE3, m_glTexture3);

	// Set the viewport variables
	m_vpWidth = (float)vp->width;
	m_vpHeight = (float)vp->height;
	m_Scale = 1.0;

	//
	// Scaling for a source plugin
	//
#ifndef EFFECT_PLUGIN
	float resolutionwidth = m_vpWidth;
	// Set the source shader resolution width if it has been defined
#ifdef RESOLUTION_WIDTH
	resolutionwidth = RESOLUTION_WIDTH;
#endif
	// Set the scale factors relative to the viewport width if it is downscaled
	if (resolutionwidth < m_vpWidth)
		m_Scale = resolutionwidth / m_vpWidth;
#endif // endif not an effect plugin

	// Create an fbo for rendering to a texture
	glGenFramebuffersEXT(1, &m_fbo);

	// Create an empty render texture the size of the scaled viewport
	CreateGLtexture(m_glTexture, (unsigned int)(m_vpWidth*m_Scale), (unsigned int)(m_vpHeight*m_Scale), NULL);

	// Use base-class init as success result so that it retains the viewport.
	return CFFGLPlugin::InitGL( vp );
}

FFResult ShaderMaker4::ProcessOpenGL( ProcessOpenGLStruct* pGL )
{
	// Do not proceed if the shader did not compile
	if (!bInitialized)
		return FF_SUCCESS;

	// Restart the clock if the plugin is deactivated and re-activated
	lastProcessTime = processTime;
	processTime = GetCounter(); // msec
	if ((processTime - lastProcessTime) > 250)
		StartCounter();

	// Calculate date variables
	time_t datime;
	struct tm tmbuff;
	time(&datime);
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	localtime_s(&tmbuff, &datime);
#else
	localtime_r(&datime, &tmbuff);
#endif
	m_dateYear = (double)tmbuff.tm_year;
	m_dateMonth = (double)tmbuff.tm_mon + 1;
	m_dateDay = (double)tmbuff.tm_mday;
	m_dateTime = (double)(tmbuff.tm_hour * 3600 + tmbuff.tm_min * 60 + tmbuff.tm_sec);

	// Calculate elapsed time
	lastTime = elapsedTime;
	elapsedTime = GetCounter() / 1000.0; // seconds
	m_time = m_time + (float)(elapsedTime - lastTime)*m_UserSpeed*2.0f; // increment scaled by user input 0.0 - 2.0

	// Calculate ShaderToy variables based on time
	m_frame = m_frame + 1.0;
	m_timedelta = (elapsedTime - lastTime);
	m_framerate = 1.0 / m_timedelta;
	m_samplerate = 44100.0; // default
	// Use elapsed time for individual channel times
	m_channelTime[0] = m_time;
	m_channelTime[1] = m_time;
	m_channelTime[2] = m_time;
	m_channelTime[3] = m_time;

	// Default viewport size
	float vpWidth = m_vpWidth;
	float vpHeight = m_vpHeight;

	// Scale the viewport to reduce the size for complex source shaders
	vpWidth = m_vpWidth * m_Scale;
	vpHeight = m_vpHeight * m_Scale;

	// Calculate mouse coordinates based on the viewport
	// An Effect always has scale = 1.0
	// iMouse
	//    xy contain the current pixel coords (if LMB is down);
	//    zw contain the click pixel.
	// Modified here equivalent to mouse unclicked or left button dragged
	// The mouse is not being simulated, they are just inputs that can be used within the shader.
	// Convert from 0-1 to pixel coordinates for ShaderToy
	// Here we use the resolution rather than the screen
	m_mouseX = m_UserMouseX * vpWidth;
	m_mouseY = m_UserMouseY * vpHeight;

	// limits
	if (m_mouseX < 0.0f) m_mouseX = 0.0f;
	if (m_mouseY < 0.0f) m_mouseY = 0.0f;
	if (m_mouseX > vpWidth) m_mouseX = vpWidth;
	if (m_mouseY > vpHeight) m_mouseY = vpHeight;

	// zw
	m_mouseLeftX = m_UserMouseLeftX * vpWidth;
	m_mouseLeftY = m_UserMouseLeftY * vpHeight;

	// If this is an effect plugin and there is texture needed by the shader
	// look for an incoming texture from the host
#ifdef EFFECT_PLUGIN

	if (pGL->numInputTextures < 1)
		return FF_FAIL;

	if (pGL->inputTextures[0] == NULL)
		return FF_FAIL;

	// Set the channel resolution for the first texture size
	FFGLTextureStruct Texture0 = *(pGL->inputTextures[0]);
	m_channelResolution[0][0] = (float)Texture0.Width;
	m_channelResolution[0][1] = (float)Texture0.Height;

#endif // endif shader uses a texture

	// Bind the shader to use it

	// FFGL requires us to leave the context in a default state on return, 
	// so use this scoped binding to help us do that.
	ScopedShaderBinding shaderBinding(m_shader.GetGLID());

	// Set viewport size for the vertex shader
	m_shader.Set("size", m_vpWidth, m_vpHeight); // for fragCoord

	// Handle user input
	HandleParams();

	// Set the shader uniforms

	// GLSL sandbox
	if (bGlslSandbox) {
		m_shader.Set("time", (float)m_time);
		m_mouseX = m_UserMouseX;
		m_mouseY = m_UserMouseY;
		m_shader.Set("mouse", m_mouseX, m_mouseY); // 0-1 for GLSL sandbox
		m_shader.Set("resolution", m_vpWidth, m_vpHeight); // iResolution is vec2
		m_shader.Set("surfacesize", m_mouseLeftX, m_mouseLeftY);
		m_shader.Set("inputColour", m_UserRed, m_UserGreen, m_UserBlue, m_UserAlpha);
	}
	else {
		// ShaderToy
		m_shader.Set("iResolution", m_vpWidth, m_vpHeight, 1.0); // iResolution is vec3
		m_shader.Set("iMouse", m_mouseX, m_mouseY, m_mouseLeftX, m_mouseLeftY);
		// iGlobalTime has been replaced in the shader by iTime
		m_shader.Set("iTime", (float)m_time);
		m_shader.Set("iFrame", (float)m_frame);
		m_shader.Set("iTimeDelta", (float)m_timedelta);
		m_shader.Set("iFrameRate", (float)m_framerate);
		m_shader.Set("iSampleRate", (float)m_samplerate);
		m_shader.Set("iDate", (float)m_dateYear, (float)m_dateMonth, (float)m_dateDay, (float)m_dateTime);
		m_shader.Set("inputColour", m_UserRed, m_UserGreen, m_UserBlue, m_UserAlpha);
#ifdef EFFECT_PLUGIN
		// Use elapsed time for individual channel times
		m_shader.Set("iChannelTime[0]", (float)m_time);
		m_shader.Set("iChannelTime[1]", (float)m_time);
		m_shader.Set("iChannelTime[2]", (float)m_time);
		m_shader.Set("iChannelTime[3]", (float)m_time);
		// Channel resolutions
		// 4 channels, each Vec3 width, height, depth * 4
		// Used for textures and images
		if (m_channelresolutionLocation >= 0)
			glUniform3fv(m_channelresolutionLocation, 4, (GLfloat *)m_channelResolution);
#endif
	} // endif glslsandbox/shadertoy

	// User shader uniforms
	if (m_UserUniforms.size() > 0) {
		for (int i = 0; i < m_UserUniforms.size(); i++) {
			m_shader.Set(m_UserUniforms[i].name.c_str(), m_UserUniforms[i].value);
		}
	}

	// Bind textures if images have been loaded.
	// Effects do not load image0 which is reserved for the host.
	// If one is defined, the texture will not be used.
	BindTextures();

	//
	// Effect
	//

#ifdef EFFECT_PLUGIN

	// The shader's sampler is always bound to sampler index 0
	// so that's where we need to bind the texture.
	// Again, we're using the scoped bindings to help us keep the context in a default state.
	ScopedSamplerActivation activateSampler(0);

	// Bind the host texture
	Scoped2DTextureBinding textureBinding(pGL->inputTextures[0]->Handle);

	// Tell the shader the name of the texture
	if (bGlslSandbox)
		m_shader.Set("tex0", 0);
	else
		m_shader.Set("iChannel0", 0);
	
	// The input texture's dimension might change each frame and so might the content area.
	// We're adopting the texture's maxUV using a uniform because that way we dont have to
	// update our vertex buffer each frame.
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(*pGL->inputTextures[0]);
	m_shader.Set("MaxUV", maxCoords.s, maxCoords.t);

	m_quad.Draw();

#else

	// For source images we don't need to worry about changing dimensions
	m_shader.Set("MaxUV", 1.0, 1.0);

	// if scaling is not 1.0, change viewport size and render to fbo
	if (m_Scale != 1.0) {

		if (!m_fbo)
			glGenFramebuffersEXT(1, &m_fbo);

		if (!m_glTexture)
			CreateGLtexture(m_glTexture, (unsigned int)vpWidth, (unsigned int)vpHeight, NULL);

		glViewport(0, 0, (GLsizei)vpWidth, (GLsizei)vpHeight);

		// Render to the scaled texture (m_glTexture) via fbo
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		glFramebufferTexture2DEXT(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_glTexture, 0);
		ScopedSamplerActivation activateSampler(4); // Unused texture unit
		Scoped2DTextureBinding textureBinding(m_glTexture);
	}

	// All done so draw
	m_quad.Draw();

	// For resolution scaling, draw the fbo result in m_glTexture
	if (m_Scale != 1.0) {
		// unbind the fbo
		if (pGL->HostFBO > 0)
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, pGL->HostFBO);
		else
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

		// Restore the viewport
		glViewport(0, 0, (GLsizei)m_vpWidth, (GLsizei)m_vpHeight);

		// Draw the texture
		ScopedShaderBinding shaderBinding(m_drawShader.GetGLID());
		ScopedSamplerActivation activateSampler(4);
		Scoped2DTextureBinding textureBinding(m_glTexture);
		m_drawShader.Set("inputTexture", 4);
		m_drawShader.Set("size", m_vpWidth, m_vpHeight);
		m_drawShader.Set("MaxUV", 1.0, 1.0);
		m_quad.Draw();
	}

#endif

	// All finished with the image textures
	UnbindTextures();

	return FF_SUCCESS;
}


FFResult ShaderMaker4::DeInitGL()
{
	Cleanup();

	return FF_SUCCESS;
}


void ShaderMaker4::BindTextures()
{
	// TODO : Scoped bindings do not work for some reason
	if (bGlslSandbox) {
		if (m_glTexture0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_glTexture0);
			m_shader.Set("tex0", 0);
		}
		if (m_glTexture1) {
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, m_glTexture1);
			m_shader.Set("tex1", 1);
		}
		if (m_glTexture2) {
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, m_glTexture2);
			m_shader.Set("tex2", 2);
		}
		if (m_glTexture3) {
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, m_glTexture3);
			m_shader.Set("tex3", 3);
		}
	}
	else {
		if (m_glTexture0) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_glTexture0);
			m_shader.Set("iChannel0", 0);
		}
		if (m_glTexture1) {
			glActiveTexture(GL_TEXTURE0 + 1);
			glBindTexture(GL_TEXTURE_2D, m_glTexture1);
			m_shader.Set("iChannel1", 1);
		}
		if (m_glTexture2) {
			glActiveTexture(GL_TEXTURE0 + 2);
			glBindTexture(GL_TEXTURE_2D, m_glTexture2);
			m_shader.Set("iChannel2", 2);
		}
		if (m_glTexture3) {
			glActiveTexture(GL_TEXTURE0 + 3);
			glBindTexture(GL_TEXTURE_2D, m_glTexture3);
			m_shader.Set("iChannel3", 3);
		}
	}
	glActiveTexture(GL_TEXTURE0);
}

void ShaderMaker4::UnbindTextures()
{
	if (m_glTexture3) {
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (m_glTexture2) {
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (m_glTexture3) {
		glActiveTexture(GL_TEXTURE0 + 3);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	if (m_glTexture0) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	glActiveTexture(GL_TEXTURE0);
}


void ShaderMaker4::SetDefaults() {

	elapsedTime = 0.0;
	lastTime = 0.0;
	processTime = 0.0;
	lastProcessTime = 0.0;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	PCFreq = 0.0;
	CounterStart = 0;
#else
	start = std::chrono::steady_clock::now();
#endif

	m_time = 0.0f;
	m_mouseX = 0.5f;
	m_mouseY = 0.5f;
	m_mouseLeftX = 0.0f;
	m_mouseLeftY = 0.0f;

	// User variables
	m_UserSpeed = 0.5f;
	m_UserMouseX = 0.5f;
	m_UserMouseY = 0.5f;
	m_UserMouseLeftX = 0.0f;
	m_UserMouseLeftY = 0.0f;
	m_UserRed = 0.5f;
	m_UserGreen = 0.5f;
	m_UserBlue = 0.5f;
	m_UserAlpha = 1.0f;
	m_UserUniforms.clear();

	m_channelresolutionLocation = -1;

	// Parameter pointers
	Speed = nullptr;
	Xmouse = nullptr;
	Ymouse = nullptr;
	XmouseLeft = nullptr;
	YmouseLeft = nullptr;
	Red = nullptr;
	Green = nullptr;
	Blue = nullptr;
	Alpha = nullptr;
	UserUniforms.clear();

	// Initialize all texture ID's to zero
	m_glTexture = 0;
	m_glTexture0 = 0;
	m_glTexture1 = 0;
	m_glTexture2 = 0;
	m_glTexture3 = 0;
	m_fbo = 0;

	// Flags
	bGlslSandbox = true; // default
	bInitialized = false;

} // end SetDefaults


void ShaderMaker4::Cleanup()
{
	m_shader.FreeGLResources();
	m_quad.Release();
	m_drawShader.FreeGLResources();

	if (m_fbo)
		glDeleteBuffers(1, &m_fbo);
	if (m_glTexture)
		glDeleteTextures(1, &m_glTexture);
	if (m_glTexture0)
		glDeleteTextures(1, &m_glTexture0);
	if (m_glTexture1)
		glDeleteTextures(1, &m_glTexture1);
	if (m_glTexture2)
		glDeleteTextures(1, &m_glTexture2);
	if (m_glTexture3)
		glDeleteTextures(1, &m_glTexture3);

	m_UserUniforms.clear();
	UserUniforms.clear();

}


void ShaderMaker4::StartCounter()
{

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	LARGE_INTEGER li;
	// Find frequency
	QueryPerformanceFrequency(&li);
	PCFreq = double(li.QuadPart) / 1000.0;
	// Second call needed
	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
#else
	// posix c++11
	start = std::chrono::steady_clock::now();
#endif

}

double ShaderMaker4::GetCounter()
{

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
#else
	// posix c++11
	end = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.;
#endif
	return 0;

}

std::string ShaderMaker4::LoadShaderString(std::string shaderString) {

	// Look for non-ShaderToy/Sandbox uniforms
	// before the shadertoy uniforms are added.
	m_UserUniforms.clear();

	if (!shaderString.empty()) {
		// Extract line by line to search for user uniforms
		std::istringstream ss(shaderString);
		std::string line;
		std::string str1;
		std::string str2;
		std::size_t startpos;
		std::size_t endpos;
		uniform unf;
		// loop till the end of the stream
		while (std::getline(ss, line)) {
			// Check for lines starting with "uniform"
			startpos = line.find("uniform");
			// Must be at the start of the line
			if (startpos != std::string::npos && startpos == 0) {
				// Discard anything after ";"
				endpos = line.find_first_of(";");
				if (endpos != std::string::npos) {
					line = line.substr(startpos, endpos + 1);
				}
				startpos = line.find_first_of(" ");
				endpos = line.find_last_of(" ");
				str1 = line.substr(startpos + 1, endpos - startpos - 1); // type
				unf.type = str1; // Only float and bool are supported
				str1 = line.substr(endpos + 1, line.length() - endpos - 2); // name
				unf.name = str1;
				// Discard GLSL sandbox uniforms
				if (str1 == "time") continue;
				if (str1 == "mouse") continue;
				if (str1 == "resolution") continue;
				if (str1 == "surfaceSize") continue;
				if (str1 == "surfacePosition") continue;
				if (str1 == "texture") continue;
				if (str1 == "tex0") continue;
				if (str1 == "tex1") continue;
				if (str1 == "tex2") continue;
				if (str1 == "tex3") continue;
				if (str1 == "backbuffer") continue;
				if (str1 == "bbuf") continue;
				if (unf.type == "int" || unf.type == "bool")
					unf.value = 0.0f;
				else
					unf.value = 0.5f;
				m_UserUniforms.push_back(unf);
			}
		}
	}

	// Include ShaderToy or GLSL Sandbox uniforms
	// and modify the fragment shader code for GL4
	std::string stoyUniforms;

	// Extra uniforms
	static char *extraUniforms = { "uniform vec4 inputColour;\n" };

	// Is it a GLSL Sandbox file?
	// look for "uniform float time;"
	// If it does not exist it's a ShaderToy file.
	// This is an exact string, so the shader has to have it.
	if (strstr(shaderString.c_str(), "uniform float time;") == 0) {

		bGlslSandbox = false;

		//
		// ShaderToy file
		//
		// Shadertoy does not include uniform variables in the source file so add them here.
		//
		// uniform vec3			iResolution;			// the rendering resolution (in pixels)
		// uniform float		iGlobalTime;			// current time (in seconds)
		// uniform vec4		 	iMouse;					// xy contain the current pixel coords (if LMB is down). zw contain the click pixel.
		// uniform vec4			iDate;					// (year, month, day, time in seconds)
		// uniform float		iChannelTime[4];		// channel playback time (in seconds)
		// uniform vec3			iChannelResolution[4];	// channel resolution (in pixels)
		// uniform sampler2D	iChannel0;				// sampler for input texture 0.
		// uniform sampler2D	iChannel1;				// sampler for input texture 1.
		// uniform sampler2D	iChannel2;				// sampler for input texture 2.
		// uniform sampler2D	iChannel3;				// sampler for input texture 3.
		// uniform float iTime;			// same as IGlobalTime - current time (in seconds)
		// uniform float iTimeDelta;	// _deltaTime
		// uniform float iFrame;		// Frame number ?
		// uniform float iFrameRate;	// 1.f / _deltaTime
		// uniform float iSampleRate;	// Audio
		//
		static char *uniforms = { "uniform vec3 iResolution;\n"
								  "uniform float iGlobalTime;\n"
								  "uniform float iTime;\n"
								  "uniform float iTimeDelta;\n"
								  "uniform float iFrame;\n"
								  "uniform float iFrameRate;\n"
								  "uniform float iSampleRate;\n"
								  "uniform vec4 iMouse;\n"
								  "uniform vec4 iDate;\n"
								  "uniform float iChannelTime[4];\n"
								  "uniform vec3 iChannelResolution[4];\n"
								  "uniform sampler2D iChannel0;\n"
								  "uniform sampler2D iChannel1;\n"
								  "uniform sampler2D iChannel2;\n"
								  "uniform sampler2D iChannel3;\n" };

		// GL4
		stoyUniforms = "#version 410 core\n";

		// For OpenGL4 set some ins and outs.
		static char *GL4 = { "\nin vec2 uvCoord;\n"
							 "in vec2 fragCoord;\n"
							 "out vec4 fragColor;\n\n" };

		stoyUniforms += GL4;
		stoyUniforms += uniforms;
		stoyUniforms += extraUniforms;

		// It might be a revised ShaderToy file with "mainImage" instead of "main".
		if (strstr(shaderString.c_str(), "void mainImage") != 0) {
			//
			// If it is a revised spec ShaderToy file, add a fix at the end for GLSL compatibility.
			//
			// Credit Eric Newman 
			// http://magicmusicvisuals.com/forums/viewtopic.php?f=2&t=196
			//
			static char *stoyMainFunction = { "void main(void) {\n"
											  "    mainImage(fragColor, fragCoord);\n"
											  "}\n" };

			shaderString += stoyMainFunction;
		}

		// Replace all #version with "// #version"
		ReplaceAll(shaderString, "#version", "// #version");
		// Replace all gl_FragCoord.xyz with "vec3(uvCoord, 1.0)"
		ReplaceAll(shaderString, "gl_FragCoord.xyz", "vec3(fragCoord, 1.0)");
		// gl_FragCoord.xy/iResolution.xy > uvCoord
		ReplaceAll(shaderString, "gl_FragCoord.xy/iResolution.xy", "uvCoord");
		// gl_FragCoord.xy / iResolution.xy > uvCoord
		ReplaceAll(shaderString, "gl_FragCoord.xy / iResolution.xy", "uvCoord");
		// gl_FragCoord.xy > uvCoord*iResolution.xy
		ReplaceAll(shaderString, "gl_FragCoord.xy", "uvCoord*iResolution.xy");
		// replace all occurences of gl_FragColor with fragColor
		ReplaceAll(shaderString, "gl_FragColor", "fragColor");
		// Replace gl_TexCoord[0].xy with uvCoord
		ReplaceAll(shaderString, "gl_TexCoord[0].xy", "uvCoord");

		// ShaderToy duplicates for time
		ReplaceAll(shaderString, "iGlobalTime", "iTime");


	} // end ShaderToy File
	else {

		//
		// GLSL sandbox file
		//

		bGlslSandbox = true;

		stoyUniforms = "#version 410 core\n";

		static char *GL4 = { "\nin vec2 uvCoord;\n"
							 "in vec2 fragCoord;\n"
							 "out vec4 fragColor;\n\n" };
		stoyUniforms += GL4;

		// Add inputColour
		stoyUniforms += extraUniforms;

		// replace all occurences of gl_FragColor with fragColor
		ReplaceAll(shaderString, "gl_FragColor", "fragColor");
		// Replace all gl_FragCoord.xyz with "vec3(gl_FragCoord.xy, 1.0)"
		ReplaceAll(shaderString, "gl_FragCoord.xyz", "vec3(fragCoord, 1.0)");
		// Replace gl_FragCoord.xy with fragCoord
		ReplaceAll(shaderString, "gl_FragCoord.xy", "fragCoord");
		// Replace gl_TexCoord[0].xy with uvCoord
		ReplaceAll(shaderString, "gl_TexCoord[0].xy", "uvCoord");
	}

	// add the rest of the shared content
	stoyUniforms += shaderString;

	// the final modified string
	shaderString = stoyUniforms;

	return shaderString;

} // end LoadShaderString


bool ShaderMaker4::LoadShader(std::string shaderString) {

	if (shaderString.empty())
		return false;

	// Compile shader
	if (!m_shader.Compile(_vertexShaderCode, shaderString.c_str()))
	{
		DeInitGL();
		return false;
	}
	if (!m_quad.Initialise())
	{
		DeInitGL();
		return false;
	}

	// FFGL requires us to leave the context in a default state on return,
	// so use this scoped binding to help us do that.
	ScopedShaderBinding shaderBinding(m_shader.GetGLID());

	//
	//ShaderToy channel resolution location is needed for glUniform3fv
	if (!bGlslSandbox) {
		// iChannelResolution
		if (m_channelresolutionLocation < 0) // Vec3 width, height, depth * 4
			m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[4]");
		if (m_channelresolutionLocation < 0)
			m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[3]");
		if (m_channelresolutionLocation < 0)
			m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[2]");
		if (m_channelresolutionLocation < 0)
			m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[1]");
		if (m_channelresolutionLocation < 0)
			m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[0]");
	}

	// Start the clock
	StartCounter();

	// Reset the frame counter
	m_frame = 0.0;

	return true;
}

void ShaderMaker4::HandleParams()
{

	if (Speed)
		m_UserSpeed= Speed->GetValue();

	if (Xmouse)
		m_UserMouseX = Xmouse->GetValue();
	
	if (Ymouse)
		m_UserMouseY = Ymouse->GetValue();

	if (XmouseLeft)
		m_UserMouseLeftX = XmouseLeft->GetValue();

	if (YmouseLeft)
		m_UserMouseLeftY = YmouseLeft->GetValue();

	if (Red)
		m_UserRed = Red->GetValue();

	if (Green)
		m_UserGreen = Green->GetValue();

	if (Blue)
		m_UserBlue = Blue->GetValue();

	if (Alpha)
		m_UserAlpha = Alpha->GetValue();

	// User shader uniforms
	if (m_UserUniforms.size() > 0) {
		for (int i = 0; i < m_UserUniforms.size(); i++) {
			m_UserUniforms[i].value = UserUniforms[i]->GetValue();
		}
	}

}

// Create an opengl texture and fill with the data provided
void ShaderMaker4::CreateGLtexture(GLuint &texID, unsigned int width, unsigned int height, void *data)
{
	if (texID != 0)
		glDeleteTextures(1, &texID);

	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

}

// ========================================
// Functions for resource loading
// ========================================

//
//
// http://www.codeguru.com/cpp/w-p/dll/tips/article.php/c3635/Tip-Detecting-a-HMODULEHINSTANCE-Handle-Within-the-Module-Youre-Running-In.htm
//
HMODULE ShaderMaker4::GetCurrentModule()
{
	return reinterpret_cast<HMODULE>(&__ImageBase);
}

//
// Load a shader from resources
//
bool ShaderMaker4::LoadShaderResource(std::string &shaderString)
{
	HRSRC hResource = NULL;
	DWORD size = 0;
	HGLOBAL rcShaderData = NULL;
	const char* shaderdata = NULL;

	// Load the shader code from resources - the shader file path is defined in shaderfiles.h
	hResource = FindResource(m_hModule, MAKEINTRESOURCE(IDC_SHADERFILE), RT_RCDATA);
	if (hResource) {
		size = SizeofResource(m_hModule, hResource);
		// printf("size = %d\n", size);
		rcShaderData = LoadResource(m_hModule, hResource);
		if (rcShaderData) {
			// printf("rcShaderData = %d\n", rcShaderData);
			shaderdata = static_cast<const char*>(LockResource(rcShaderData));
			shaderString = shaderdata; // the shader file string for processing
			// It is possible that the size of the string is one character larger
			// than the size of the resource (unknown cause) - reduce the size if so
			if (shaderString.size() > size)
				shaderString.resize(size);
			CleanShaderString(shaderString);
			shaderString += ""; // ensure a NULL terminator
			return true;
		}
		else
			printf("LoadResource failed - error = %d\n", GetLastError());
	}
	else 
		printf("FindResource failed - error = %d\n", GetLastError());

	return false;
}

void ShaderMaker4::CleanShaderString(std::string & shaderString)
{
	std::string newString;
	char c = 10; // start with something

	if (shaderString.empty()) {
		printf("CleanShaderString : shaderstring empty\n");
		return;
	}

	// Copy character by character to remove any out-of range
	for (int i = 0; i < (int)shaderString.size(); i++) {
		c = shaderString.at(i);
		if (c == 13 || c == 10 || c == 9 || (c >= 32 && c < 128)) {
			newString += c;
		}
	}
	shaderString = newString;
}


// =======================================================
//
// Load an image from resources
//
// Uses the SOIL library
// http://www.lonesock.net/soil.html
// See also : https://github.com/fenbf/SOIL_ext
// To avoid incompatibility, do not use SOIL OpenGL functions
// Extract an image file to a buffer then create our own texture
//
bool ShaderMaker4::LoadResourceImage(GLuint ResourceID, GLuint &TextureID)
{
	HMODULE hModule = NULL;
	DWORD size = 0;
	HRSRC hResInfo = NULL;
	HGLOBAL rcImageData = NULL;
	const unsigned char* imagedata = nullptr;
	unsigned char* buffer = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;

	int image_number = ResourceID - IDC_IMAGEFILE0;

	// If the resource (IMAGEFILE0 - IMAGEFILE3) is not defined,
	// FindResource fails and the function returns false.
	// See resource.rc
	hResInfo = FindResource(m_hModule, MAKEINTRESOURCE(ResourceID), RT_RCDATA);
	if (hResInfo) {
		size = SizeofResource(m_hModule, hResInfo);
		if (size > 0) {
			rcImageData = LoadResource(m_hModule, hResInfo);
			if (rcImageData != NULL) {
				imagedata = static_cast<const unsigned char*>(LockResource(rcImageData));
				// Load the image and create a texture
				buffer = SOIL_load_image_from_memory((const unsigned char *)imagedata,
					size, &width, &height, &channels, SOIL_LOAD_RGBA);
				if (buffer) {
					// Create an OpenGL texture from the image data
					FlipVertical(buffer, width, height);
					CreateGLtexture(TextureID, width, height, (void *)buffer);
					SOIL_free_image_data(buffer);

					// Set channel resolutions
					switch (image_number) {

					case 0:
						m_channelResolution[0][0] = (float)width;
						m_channelResolution[0][1] = (float)height;
						break;

					case 1:
						m_channelResolution[1][0] = (float)width;
						m_channelResolution[1][1] = (float)height;
						break;

					case 2:
						m_channelResolution[2][0] = (float)width;
						m_channelResolution[2][1] = (float)height;
						break;

					case 3:
						m_channelResolution[3][0] = (float)width;
						m_channelResolution[3][1] = (float)height;
						break;
					}
					// printf("LoadResourceImage(%d) OK \n", image_number);
					return true;
				}
			}
		}
	}
	// printf("LoadResourceImage(%d) failed\n", image_number);
	return false;

} // end LoadResourceImage


// Adapted from FreeImage function
// Flip the image vertically along the horizontal axis.
bool ShaderMaker4::FlipVertical(unsigned char *src, unsigned int w, unsigned int h)
{
	BYTE *From, *Mid;

	// swap the buffer
	int pitch = w * 4; // RGBA

	// copy between aligned memories
	Mid = (BYTE*)malloc(pitch * sizeof(BYTE));
	if (!Mid) return false;

	From = src;

	unsigned int line_s = 0;
	unsigned int line_t = (h - 1)*pitch;

	for (unsigned int y = 0; y < h / 2; y++) {
		CopyMemory(Mid, From + line_s, pitch);
		CopyMemory(From + line_s, From + line_t, pitch);
		CopyMemory(From + line_t, Mid, pitch);
		line_s += pitch;
		line_t -= pitch;
	}

	free((void *)Mid);

	return true;
}

