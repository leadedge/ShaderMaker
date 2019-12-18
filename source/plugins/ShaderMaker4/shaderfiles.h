//
// ========================= OPTIONS =============================
//
// Define "EFFECT_PLUGIN" here to build the plugin as an effect rather than a source
//
// #define EFFECT_PLUGIN

// Optionally define the resolution width to be used by a source plugin.
// The default is the host viewport which can affect fps for some shaders.
// If the plugin is an effect this is not used.
// Only the width is needed. The height is scaled to the aspect ratio of the host viewport
#define RESOLUTION_WIDTH 640


// ======================= SHADER FILES ===========================
//
// The shader file path is entered here and the entire file is
// embedded in resources when the program is compiled. 
// Thereafter, the plugin is independent of the file itself.
//
// IMAGES
//
// Define any images that a shader uses here (IMAGEFILE0 to IMAGEFILE3).
// If "EFFECT_PLUGIN" is defined IMAGEFILE0 will be ignored
// and the texture provided by the host will be used.
//
// ==================================================================

#ifdef EFFECT_PLUGIN

//
// EFFECT SHADERS
//

// Thermal imaging
// #define SHADERFILE "SHADERS/EFFECT/thermal-imaging.txt"

// Pass through
// #define SHADERFILE "SHADERS/EFFECT/pass-through.txt"

// CRT video effect
// #define SHADERFILE "SHADERS/EFFECT/crt-video-effect.txt"

// TV glitch effect
// #define SHADERFILE "SHADERS/EFFECT/tv-damage-glitch.txt"

//
// EFFECT SHADERS using images (IMAGEFILE1 to IMAGEFILE3)
//

// Raining screen effect
// IMAGEFILE0 - the distortion image
// #define SHADERFILE "SHADERS/EFFECT/raining-screen.txt";
// #define IMAGEFILE1 "TEXTURES/tex03.jpg"

#else

//
// SOURCE SHADERS
//

// Voronoi Distances
#define SHADERFILE "SHADERS/SOURCE/voronoi-distances.txt"

// Disk intersection
// #define SHADERFILE "SHADERS/SOURCE/disk_intersection.txt"

// Clouds
// This is an example where reduced resolution is necessary because
// the shader significantly reduces fps at composition resolution.
// Try RESOLUTION_WIDTH 320. It's only clouds.
// Reacts to mouse X and Y.
// #define SHADERFILE "SHADERS/SOURCE/clouds.txt"

// Bokehlicious
// This is an example where ShaderToy mouse control of aperture and focus
// has been replaced by user float uniforms. Details in the shader file.
// You could remove Mouse X and Mouse Y controls for this one.
// #define SHADERFILE "SHADERS/SOURCE/bokehlicious2.txt"

//
// SOURCE SHADERS with images (IMAGEFILE0 to IMAGEFILE3)
//

// Flakes
// #define SHADERFILE "SHADERS/SOURCE/flakes.txt"
// IMAGEFILE0 - image to generate the flakes
// #define IMAGEFILE0 "TEXTURES/tex03.jpg"

// Infinite city
// IMAGEFILE0 - a noise image on Channel0 - tex12.png
// IMAGEFILE1 - an image used for reflections - tex06.jpg
// Can be slow, so reduce resolution. Try RESOLUTION_WIDTH 640.
// Reacts to mouse X and Y.
// #define SHADERFILE "SHADERS/SOURCE/infinite_city.txt"
// #define IMAGEFILE0 "TEXTURES/tex12.png"
// #define IMAGEFILE1 "TEXTURES/tex06.jpg"

// Hexagons distance
// IMAGEFILE0 - a noise image on Channel0 - tex12.png
// #define SHADERFILE "SHADERS/SOURCE/hexagons_distance.txt"
// #define IMAGEFILE0 "TEXTURES/tex12.png"

#endif







