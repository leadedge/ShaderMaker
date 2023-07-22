# VJ Spack-O-Mat FFGL Plugins 2023 Update
![one of i-love-chaos logos print your own more than endless possibilities](release/media/i-love-chaos-white-000000000000003.png "One of I-Love-Chaos")
          
this repository is a huge development repository for plugins by VJ-Spack-O-Mat
<!-- TOC -->
- [QuickStart](#quickstart)
- [Mission Statement](#mission-statement)
  - [Who is VJ Spack-O-Mat and why](#who-is-vj-spack-o-mat-and-why)

<!-- TOC END -->
# QuickStart

- for downloading binaries and documentation
    [Spack-O-Mat Plugins Binaries](release)
    
- for developing with this repository
    [Spack-O-Mat Plugin Development](DEVELOPMENT.md)

# Mission Statement

## Who is VJ Spack-O-Mat and why

__Christian Kleinhuis__ - former administrator of [www.fractalforums.com](http://fractalforums.com), succeeded by [fractalforums.org](http://fractalforums.org) - is __VJ Spack-O-Mat__. 

__Christian__ and the __VJ__ together with the folks at [ChaosTube](https://www.youtube.com/channel/UCej4aqqeusL5iUnKHjmKjLQ) Youtube channel - which are __Christian Kleinhuis__ as well (*speaker*,*artist*,*scientist*,*keeper*) - follow a huge goal, the goal is to create a popular stimulating didactory experience 
for introducing the [Chaos Theory](https://www.wikiwand.com/en/Chaos_theory) to the world. 

This shall be a physical space with musical resting and dancing opportunities - _the cafe_ -, a free for everyone area to introduce the abstract theory behind chaos theory using interactive tutorials - _the classroom_ -, an area where contemporary digital art is exhibited, providing the possibility for each participating artist in earning money from the exposure, hence this area will have an entry fee - _the exhibition_ - and finally the fourth level is christian's own massive digital playground featuring the stuff he has already experienced: [interactive kinect usage](https://youtu.be/sasvx7GcIT4?t=64) - safe laser experiments [https://github.com/trifox/laserapi](LaserApi), touch screen floors using various fractal generation techniques or plain [https://github.com/trifox/ShaderMaker](Resolume) plugins :) and everything that might be of his personal interest who knows ...  - _the mutatorkammer_.

The concept shall barry itself, selling drinks, food (broccoli) and merchandise should produce the income needed to supply the above
mentioned spaces around the world spreading knowledge and letting most of the money create either other spaces or sense full contributions to the development of the world - where not only art and free minded spaces carry on but following mostly the golden rule to participate in any outcomings ... 
 
show your affection on [i-love-chaos](https://i-love-chaos.org) 
=======
# ShaderMaker4
A source code template to build FreeframeGL 2.1 plugins for Resolume from GLSL Sandbox or ShaderToy shaders.\
During compilation, shader and image files are loaded into into resources and saved with the plugin. The plugin then accesses these resources when it loads. ShaderToy multiple buffers are not supported. This is a Windows project for Visual Studio 2017.

## How to install and build
1) Download a zip file of this master branch or from the latest release
2) Unzip it somewhere
3) Download the Resolume FFGL 2.1 repository zip file from the ffgl2.1 branch which is the most recent released version of the sdk - https://github.com/resolume/ffgl/tree/ffgl2.1
4) Unzip it somewhere
5) Copy the folder SHADERMAKER4\source\plugins\ShaderMaker4 to the "..\source\plugins\" folder.
6) Copy the two files in SHADERMAKER4\build\windows to the "..\build\windows" folder
7) Open "..\build\windows\FFGLPlugins.sln" in Visual Studio 2017
8) RH click on "sources" in the Solution Explorer and select "Add > Existing project"
9) Browse to "..\build\windows" and select "ShaderMaker4.vcxproj"
10) Change to "Release" and "x64" build
11) RH click on the ShaderMaker4 project and "Build"
12) The resulting ShaderMaker4.dll plugin is in "..\binaries\x64\Release".

## How to use
Copy the shader text file you require into the SHADERS folder, subfolder SOURCE or EFFECT.
Copy any images that the shader will use to the TEXTURES folder.
Edit "shaderfiles.h" to reference those files (see examples in "shaderfiles.h")..
- Define "EFFECT_PLUGIN" for an effect that uses a texture from the host.
- Define the RESOLUTION_WIDTH for a source plugin to balance performance with resolution.
- Change the plugin Name, ID and information in ShaderMaker.cpp, build and rename the resulting dll.\
Example shaders and details in "shaderfiles.h".

## FreeframeGL 1.6
The orginal ShaderMaker project for FreeframeGL 1.6 is preserved un-changed in the ffgl1.6 branch. It is self-contained and will build 32 bit and 64 bit plugins for both Windows and OSX. These are not compatible with Resolume 7 but are suitable for other host programs using FreeframeGL 1.6.

## Credits
Implementation by Lynn Jarvis http://spout.zeal.co/ - licence LGPL3\
OpenGL image loading library SOIL http://www.lonesock.net/soil.html - licence public domain.\
Shaders from the ShaderToy website, are licensed under Attribution - NonCommercial - ShareAlike 3.0 unless otherwise specified.
(https://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US) \
GLSL Sandbox is MIT licence. No default licence is documented for submitted shaders but, where specifically indicated, the licencing terms of the author must be observed.\
Include a link and credit to the author in the shader source and in the plugin description or about field where possible. 


