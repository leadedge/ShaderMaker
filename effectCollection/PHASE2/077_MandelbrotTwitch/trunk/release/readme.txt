-----------------------------------------------------------------------------------------------
VJ Spack-O-Mat FFGL Plugin Collection - Mandelbrot Twitch Hybrid  - v1.0 - September 2015
-----------------------------------------------------------------------------------------------

http://spack-o-mat.digitalgott.com 

FFGL Mandelbrot Twitch Hybrid Plugin 

A twitch hybrid mandelbrot is obtained by adding values at certain positions in the iteration process


Video Tutorial
http://youtu.be/YDlCRBqWo24


VJ Spack-O-Mat Youtube Channel
https://www.youtube.com/channel/UCiPE2hlkXqv1AZAlz5qIvIQ

If you want to support what i do, spread the word, invite me to your sets
subscribe the channel, use the plugins, display credits in your set 
(an build in greeting button will come soon)

Web
http://spack-o-mat.digitalgott.com/

facebook
https://www.facebook.com/VJSpackOMat


Installation

copy the .dll file into the [resolume]/plugins/vfx folder of your resolume installation, 
or in the apropriate folder for any ffgl enabled software.

Parameters Description
Julia - switches between julia/mandelbrot mode
Iterations - number of iterations, can improve performance on very slow systems
Rotation - rotation angle around screen center
PosX/Y - center of screen
Seed1..3 Speed - animation speed of this seed around the seedXY position
Seed1..3 Radius - distance around the seedXY position at wich the anim param orbits (not circular)
Seed1..3 X - Seed X position
Seed1..3 Y - Seed Y position
ScaleHueInner - Scale factor for the hue of the inner parts of the mandelbrot (the points that do not converge to infinity) 
ScaleSatInner - Scale factor for the saturation of the inner parts
ScaleValInner - Scale factor for the value of the inner parts
ShiftHueInner - Shift value for the hue value of the resulting colors inner parts, good for color cycling/setting
ScaleHueOuter - Scale factor for the hue of the outer parts of the mandelbrot (the points that do converge to infinity) 
ScaleSatOuter - Scale factor for the saturation of the outer parts
ScaleValOuter - Scale factor for the value of the outer parts
ShiftHueOuter - Shift value for the hue value of the resulting colors outer parts, good for color cycling/setting
Twitch1..3 - The twitch value, this value defines the iteration at which the twitch happens
             Twitching only affects non-bailed out points, the earlier the twitching happens the stronger the effect
	     is


ChangeLog
KNOWN BUG: the plugin crashes on resolume exiting, a fix is in work, and will come next release

1.2 08-04-2015 - Made it a source plugin for saving presets in resolume
1.1 08-04-2015 - Glsl Compatibility Fix 
1.0 08-02-2015 - Initial Release



Credits:
ShaderMaker - https://github.com/leadedge/ShaderMaker




