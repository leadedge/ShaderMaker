	//
//		ShaderMaker.cpp
//
//		A source code template that can be used to compile FreeframeGL
//		plugins from shader source copied from "GLSL Sandbox" and "ShaderToy".
//
//		------------------------------------------------------------
//		Revisions :
//		21-01-15	Version 1.000
//		26.02.15	Changes for FFGL 1.6
//					change DWORD to FFResult
//					remove "Virtual" from destructor definition
//					Changes  for port to OSX
//					Version 1.001
//		06.03.15	Provided for revised SharedToy spec with mainImage instead of main
//                  See ShaderToy example 4
//					Version 1.002
//		30.03.15	Set m_glTextureXX to zero after delete
//					Check for incoming Texture ID change
//					Version 1.003
//		14.04.15	Corrected texture change test in ProcessOpenGL
//					Recommend setting PluginInfo to FF_SOURCE for shaders that do not require a texture
//					Version 1.004
//
//		------------------------------------------------------------
//
//		Copyright (C) 2015. Lynn Jarvis, Leading Edge. Pty. Ltd.
//		Ported to OSX by Amaury Hazan (amaury@billaboop.com)
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
//		--------------------------------------------------------------
//
//
#include "ShaderMaker.h"
 


#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
int (*cross_secure_sprintf)(char *, size_t, const char *,...) = sprintf_s;
#else 
// posix
int (*cross_secure_sprintf)(char *, size_t, const char *, ...) = snprintf; 
#endif

#define FFPARAM_SPEED       (0)
#define FFPARAM_SPEED2       (1)
#define FFPARAM_SPEED3       (2)
#define FFPARAM_SPEED4       (3)
#define FFPARAM_MOUSEX      (4)




#define FFPARAM_MOUSEY      (5)
#define FFPARAM_MOUSELEFTX  (6)
#define FFPARAM_MOUSELEFTY  (7)
#define FFPARAM_RED         (20)
#define FFPARAM_GREEN       (21)
#define FFPARAM_BLUE        (22)
#define FFPARAM_ALPHA       (23)

#define FFPARAM_PARAM2_X       (8)
#define FFPARAM_PARAM2_Y       (9)
#define FFPARAM_PARAM2_Z       (10)
#define FFPARAM_PARAM2_W       (11)


#define FFPARAM_PARAM3_X       (12)
#define FFPARAM_PARAM3_Y       (13)
#define FFPARAM_PARAM3_Z       (14)
#define FFPARAM_PARAM3_W       (15)


#define FFPARAM_PARAM4_X       (16)
#define FFPARAM_PARAM4_Y       (17)
#define FFPARAM_PARAM4_Z       (18)
#define FFPARAM_PARAM4_W       (19)


#define FFPARAM_PARAM3_CAMERA_X       (24)
#define FFPARAM_PARAM3_CAMERA_Y       (25)
#define FFPARAM_PARAM3_CAMERA_Z       (26)

#define FFPARAM_PARAM3_CAMERA_TARGET_X       (27)
#define FFPARAM_PARAM3_CAMERA_TARGET_Y       (28)
#define FFPARAM_PARAM3_CAMERA_TARGET_Z       (29)




#define STRINGIFY(A) #A

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++ IMPORTANT : DEFINE YOUR PLUGIN INFORMATION HERE +++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
static CFFGLPluginInfo PluginInfo ( 
	ShaderMaker::CreateInstance,		// Create method
	"S056",								// *** Plugin unique ID (4 chars) - this must be unique for each plugin
	"sOm-SpaceRace",						// *** Plugin name - make it different for each plugin 
	1,						   			// API major version number 													
	006,								// API minor version number	
	1,									// *** Plugin major version number
	004,								// *** Plugin minor version number
	FF_SOURCE,							// Plugin type can always be an effect
	// FF_SOURCE,						// or change this to FF_SOURCE for shaders that do not use a texture
	"sOm kali SPace Race....", // *** Plugin description - you can expand on this
	"spack-O-mat Shadertoy Collection"			// *** About - use your own name and details
);


// Common vertex shader code as per FreeFrame examples
char *vertexShaderCode = STRINGIFY (
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;

} );


// Important notes :

// The shader code is pasted into a section of the source file below
// which uses the Stringizing operator (#) (https://msdn.microsoft.com/en-us/library/7e3a913x.aspx)
// This converts the shader code into a string which is then used by the shader compiler on load of the plugin.
// There are some limitations of using the stringizing operator in this way because it depends on the "#" symbol,
// e.g. #( .. code ), Therefore there cannot be any # characters in the code itself.
//
// For example it is common to see :
//
//		#ifdef GL_ES
//		precision mediump float;
//		#endif
//
//	The #ifdef and #endif have to be removed.
//
//		// #ifdef GL_ES
//		// precision mediump float;
//		// #endif
//
// Compile the code as-is to start with and you should get a functioning plugin.
// Several examples can be used below. Only one can be compiled at a time.
//

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++ COPY/PASTE YOUR GLSL SANDBOX OR SHADERTOY SHADER CODE HERE +++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
char *fragmentShaderCode = STRINGIFY (
	
// lko res
//	#define RAY_STEPS 65
//	#define SHADOW_STEPS 0
//	#define ITERATIONS 5
//	#define MAX_DIST 30.

float RAY_STEPS=175;
float SHADOW_STEPS= 40;
float ITERATIONS= 8;
float MAX_DIST= 35.;


vec3 LIGHT_COLOR =vec3(1.,.85,.6);
vec3 AMBIENT_COLOR= vec3(.7,.85,1.);
vec3 SUN_COLOR= vec3(1.,.8,.5);
vec3 TUBE_COLOR= inputColour.xyz;
vec3 CAR_COLOR= vec3(.4,.7,1.);
vec3 TURBINES_COLOR =vec3(.6,.75,1.);
vec3 HUD_COLOR=vec3(0.6,1.,0.3);
vec3 PLANET_COLOR =vec3(101., 153., 189.)/256.;
	
float CAR_SCALE= iParam2.z*24+24;
float SPECULAR =0.4;
float DIFFUSE  =2.0;
float AMBIENT = 0.4;

float BRIGHTNESS= .9;
float GAMMA= 1.1;
float SATURATION =.85;


float DETAIL= .0004;
float SPEED =8.;
float t= (mod(iGlobalTime,500.)+10.)*SPEED;

vec3 LIGHTDIR= normalize(vec3(0.6,-0.2,-1.));

// ------------------------------------------------------------------
//    Global Variables
// ------------------------------------------------------------------

float FOLD=1.; // controls fractal fold and track width
const vec3 planetpos=vec3(-3.5,1.,-5.); // planet position
  vec2 tubepos=iParam4.xy; // light tubes pos relative to FOLD
mat2 trmx;//the turbine spinner
float det=0.; // detail level (varies with distance)
float backcam; // back cam flag
vec3 carpos; // car position
vec3 carvec; // car pointing vector
mat3 carrot; // car rotation
float hitcar; // ray-car hit flag
mat2 fractrot; // rot mat for fractal (set in main)
mat2 fractrot2; // rot mat for fractal (set in main)
mat2 cartilt; // secondary car rotation
float minT;
float minL; // min distance traps for glows of tube and turbines
float ref; // reflection flag
float tubeinterval; // tube tiling (for glow and lighting)


// ------------------------------------------------------------------
//    General functions 
// ------------------------------------------------------------------

mat2 rot(float a) {
	return mat2(cos(a),sin(a),-sin(a),cos(a));	
}

mat3 lookat(vec3 fw,vec3 up){
	fw=normalize(fw);vec3 rt=normalize(cross(fw,normalize(up)));return mat3(rt,cross(rt,fw),fw);
}

float smin(float a,float b,float k){return -log(exp(-k*a)+exp(-k*b))/k;}//from iq

float Sphere(vec3 p, vec3 rd, float r){
	float b = dot( -p, rd );
	float inner = b * b - dot( p, p ) + r * r;
	if( inner < 0.0 ) return -1.0;
	return b - sqrt( abs(inner) );
}

float triSin(float x){
return (sin(x)+sin(x*2)+sin(x*4))/3.0;
}
float triCos(float x){
return (cos(x)+cos(x*2)+cos(x*4))/3.0;
}
// ------------------------------------------------------------------
//    Track 
// ------------------------------------------------------------------

// the track function, just some curves
vec3 path(float ti) { 
	float freq=.25;
	float amp=2.; // for trying different settings
	ti*=freq;
	float x=triCos(triCos(ti*.35682)+ti*.2823)*triCos(ti*.1322)*1.5;
	float y=triSin(ti*.166453)*4.+triCos(triCos(ti*.125465)+ti*.17354)*triCos(ti*.05123)*2.;
	vec3  p=vec3(x,y,ti/freq);
	return p;
}

// see if we are in the tunnel, and used also by distance function
float tunnel(float z) {
return abs(100.-mod(z+15.,200.))-30.;
}


// ------------------------------------------------------------------
//    DE functions
// ------------------------------------------------------------------


// carcarspacecar by eiffie // (with some mods by Kali)
// a reconfig of the carcar's tires (someday I will have to do an animation from the original to this)
//the DE looks a bit long but there are actually fewer instructions than the car

float carcarDE(in vec3 p0){
	p0.xy*=cartilt;
	p0*=CAR_SCALE;
	vec3 p=p0;
	p.y+=1.24;
	float r=length(p.yz);
	float d=length(max(vec3(abs(p.x)-0.35,r-1.92,-p.y+1.4),0.0))-0.05;
	d=max(d,p.z-1.05);
	p=p0+vec3(0.0,-0.22,0.39);
	p.xz=abs(p.xz);
	p.xyz-=vec3(0.72,0.0,1.06);
	float w1=0.23;
	float w2=0.24;
	if(p0.z<0.0){//this is discontinuous yet works unless you stand in front of the rear turbines
		w1=0.23;
		w2=0.05; //where you would get sucked into the blades anyways
		p=p.xzy; //so i'm comfortable with it :)
	} 
	r=length(p.xy);
	d=smin(d,length(vec2(max(abs(p.z)-w2,0.0),r-w1))-0.02,8.0);//done with the car shape, the rest is just turbines and could be replaced with lights or something
	d=min(d,(length(p*vec3(1.,1.,.6))-.08-p0.z*.03));
	p.xy=trmx*p.xy;//spin
	float d2=min(abs(p.x),abs(p.y))*.15;//4 blades
	//p.xy=mat2(0.707,-0.707,0.707,0.707)*p.xy;//45 degree turn
	//d2=min(d2,min(abs(p.x),abs(p.y))*.2);//8 blades
	d2=max(r-w1-.05,max(d2-0.005,abs(p.z)-w2+0.04));
	d2=min(d2,(length(p)-.05-p0.z*.035)*.07);
	d2=min(d2,max(d+.02,max(abs(p0.y-.07),abs(p0.x)-.4+min(0.,p0.z)))*.18);
	minL=min(minL,d2);//catch the minimum distance to the glowing parts
	// I used d2 only for glows (Kali)
	return d/CAR_SCALE;// *carScale
}


vec3 carcarCE(in vec3 p0){//coloring
	p0*=CAR_SCALE;
	vec4 trpc=vec4(0.);//color trap (taking samples when finding the norm)// not for now (Kali)

	//right here you should inv-transform p0 as it gets used later
	//p0=(p0-carPos)*carRot/carScale;//or something like that??
	p0.xy*=cartilt;
	vec3 p=p0;
	p.y+=1.24;
	float r=length(p.yz);
	float d=length(max(vec3(abs(p.x)-0.35,r-1.92,-p.y+1.4),0.0))-0.05;
	d=max(d,p.z-1.0);
	p=p0+vec3(0.0,-0.22,0.39);
	p.xz=abs(p.xz);
	p.xyz-=vec3(0.72,0.0,1.06);
	float w1=0.2;
	float w2=0.24;
	if(p0.z<0.0){//this is discontinuous yet works unless you stand in front of the rear turbines
		w1=0.23;
		w2=0.05; //where you would get sucked into the blades anyways
		p=p.xzy; //so i'm comfortable with it :)
	}
	r=length(p.xy);
	d=smin(d,length(vec2(max(abs(p.z)-w2,0.0),r-w1))-0.02,8.0);//done with the car shape, the rest is just turbines and could be replaced with lights or something
	p.xy=trmx*p.xy;
	float d2=min(abs(p.x),abs(p.y));//4 blades
	p.xy=mat2(0.707,-0.707,0.707,0.707)*p.xy;//45 degrees
	d2=min(d2,min(abs(p.x),abs(p.y)));//8 blades
	d2=max(r-w1+0.02,max(d2-0.005,abs(p.z)-w2+0.04));
	//up to here it is the same as DE, now accumulate colors
	if(d2<d){d=d2;trpc+=vec4(1.,0.6,0.3,256.0);}//turbines
	else {//the car's body
		p0.x=abs(p0.x);
		if((abs(p0.y-0.58)>0.05-p0.z*0.09 || p0.z>0.25) && 
		   length(max(abs(p0.xz+vec2(-p0.z*.03,-0.5))-vec2(0.15+p0.z*.03,0.4),0.0))>0.1)
			trpc+=vec4(CAR_COLOR,16.0);
		else trpc+=vec4(CAR_COLOR*.4,2.0);//the windsheild
	}
	return trpc.xyz; // *carScale
}

//-------------------------------------------

// DE for tubes
float tubes(vec3 pos) {
	pos.x=abs(pos.x)-tubepos.x-FOLD;
	pos.y+=tubepos.y;
	return (length(pos.xy)-.05);
}

// ------------------------------------------------------------------
//    SCENE DE
// ------------------------------------------------------------------

float de(vec3 pos) {
	vec3 carp=pos-carpos; // scale car coordinates
	carp=carrot*carp; // rotate car
	pos.xy-=path(pos.z).xy; // transform coordinates to follow track
	FOLD=1.7+pow(abs(100.-mod(pos.z,200.))/100.,2.)*2.; //varies fractal fold & track width
	pos.x-=FOLD;
	float hid=0.;
	vec3 tpos=pos;
	tpos.z=abs(2.-mod(tpos.z,4.));
	vec4 p=vec4(tpos,1.);
	for (int i=0; i<ITERATIONS; i++) { // calculate fractal
		p.xz=clamp(p.xz,-vec2(FOLD,2.),vec2(FOLD,2.))*2.0-p.xz;
		p=p*2.5/clamp(dot(p.xyz,p.xyz),.5,1.)+iParam3*2.0;//-vec4(1.2,0.5,0.,-0.5);
		

		p.xy*=fractrot;

		p.xz*=fractrot2;


	}
	pos.x+=FOLD;
	float fr=min(max(pos.y+.4,abs(pos.x)-.15*FOLD),(max(p.x,p.y)-1.)/p.w); // fractal+pit
	float tub=tubes(pos);  
	minT=min(minT,tub*.5); // trap min distance to tubes	
	float car=carcarDE(carp); 
	float d=tub;
	d=min(fr,d);
	d=min(d,max(abs(pos.y-1.35+cos(3.1416+pos.x*.8)*.5)-.1,tunnel(pos.z))); // tunnel DE
	if (ref<1.) d=min(d,car);
	d=max(d,abs(pos.x)-FOLD*2.);
	if (car<det) hitcar=1.; // ray hits the car!
	return d;
}


// ------------------------------------------------------------------
//    General Shading Functions
// ------------------------------------------------------------------


vec3 normal(vec3 p) {
	vec3 e = vec3(0.0,det,0.0);
	
	return normalize(vec3(
			de(p+e.yxx)-de(p-e.yxx),
			de(p+e.xyx)-de(p-e.xyx),
			de(p+e.xxy)-de(p-e.xxy)
			)
		);	
}

float shadow(vec3 pos, vec3 sdir) {
	float sh=1.0;
	float totdist = DETAIL*10.;
	float dist=1000.;
		for (int steps=0; steps<SHADOW_STEPS; steps++) {
			if (totdist<MAX_DIST && dist>DETAIL) {
				vec3 p = pos - totdist * sdir;
				dist = de(p);
				sh = min(sh, 10.*max(0.0,dist)/totdist);
				sh*= sign(max(0.,dist-DETAIL));
				totdist += max(0.02,dist);
			}
		else break;
		}
	
	return clamp(sh,0.,1.0);
}


float calcAO(vec3 pos, vec3 nor) {
	float hr;
	float dd;
	float aoi=0.;
	float sca=1.;
	float totao=0.;
	hr = .075*aoi*aoi;dd = de(nor * hr + pos);totao += (hr-dd)*sca;sca*=.6;aoi++;
	hr = .075*aoi*aoi;dd = de(nor * hr + pos);totao += (hr-dd)*sca;sca*=.55;aoi++;
	hr = .075*aoi*aoi;dd = de(nor * hr + pos);totao += (hr-dd)*sca;sca*=.55;aoi++;
	hr = .075*aoi*aoi;dd = de(nor * hr + pos);totao += (hr-dd)*sca;sca*=.55;aoi++;
	return clamp( 1.0 - 4.*totao, 0., 1.0 );
}


// ------------------------------------------------------------------
//    Light and Coloring
// ------------------------------------------------------------------



vec3 shade(in vec3 p, in vec3 dir, in vec3 n) {

	float savehitcar=hitcar;

	vec3 trackoffset=-vec3(path(p.z).xy,0.);
	vec3 pos=p;
	vec3 col=vec3(.5); // main color
	vec3 carp=pos-carpos; //scaled coordinates for the car
	carp=carrot*carp; // rotate car
	pos+=trackoffset; // apply track transformation to the coordinates
	// track lines
	if (pos.y<.5) col+=pow(max(0.,.2-abs(pos.x))/.2*abs(sin(pos.z*2.)),8.)*TUBE_COLOR*2.;
	pos.x=abs(pos.x);
	// fake AO for the tunnel's upper corners
	if(tunnel(pos.z)<0.)
		col*=1.-pow(max(0.5,1.-length(pos.xy+vec2(-FOLD*1.5,-.85))),5.)*max(0.,1.+pos.y);
	if (tubes(pos)<det) col=TUBE_COLOR*(iParam4.w*0.5+0.5); // hit tubes
	if (carcarDE(carp)<det) col=carcarCE(carp); // hit car, get coloring

	float ao=calcAO(p,n); // calc AO
	float camlight=max(0.,dot(dir,-n)); // camlight used for ambient

	// --- Tube lights ---

	vec3 tpos1=vec3((tubepos+vec2(FOLD,0.)),0.)+trackoffset; // get tube positions
	vec3 tpos2=tpos1-vec3((tubepos.x+FOLD)*2.,0.,0.);
	// light direction
	vec3 tube1lightdir=normalize(vec3(p.xy,0.)+vec3(tpos1.xy,0.)); 
	vec3 tube2lightdir=normalize(vec3(p.xy,0.)+vec3(tpos2.xy,0.));
	// light falloffs
	float falloff1;
	float falloff2;	
	if (savehitcar>0.) {
		falloff1=pow(max(0.,1.-.15*distance(vec3(p.xy,0.),vec3(-tpos1.xy,0.))),4.);
		falloff2=pow(max(0.,1.-.15*distance(vec3(p.xy,0.),vec3(-tpos2.xy,0.))),4.);
	} else {
		falloff1=pow(max(0.,1.-.2*distance(vec3(p.xy,0.),vec3(-tpos1.xy,0.))),4.);
		falloff2=pow(max(0.,1.-.2*distance(vec3(p.xy,0.),vec3(-tpos2.xy,0.))),4.);
	}
	
	float diff;
	float spec;
	
	vec3 r=reflect(LIGHTDIR,n);
	
	// tube1 calcs
	diff=max(0.,dot(tube1lightdir,-n)); 
	diff+=max(0.,dot(normalize(tube1lightdir+vec3(0.,0.,.2)),-n))*.5; // add 2 more 
	diff+=max(0.,dot(normalize(tube1lightdir-vec3(0.,0.,.2)),-n))*.5; // with Z shifted
	spec=pow(max(0.,dot(tube1lightdir+vec3(0.,0.,.4),r)),15.)*.7;
	spec+=pow(max(0.,dot(tube1lightdir-vec3(0.,0.,.4),r)),15.)*.7;
	float tl1=(falloff1*ao+diff+spec)*falloff1;

	// tube2 calcs
	diff=max(0.,dot(tube2lightdir,-n));
	diff+=max(0.,dot(normalize(tube2lightdir+vec3(0.,0.,.2)),-n))*.5;
	diff+=max(0.,dot(normalize(tube2lightdir-vec3(0.,0.,.2)),-n))*.5;
	spec=pow(max(0.,dot(tube2lightdir+vec3(0.,0.,.4),r)),15.)*.7;
	spec+=pow(max(0.,dot(tube2lightdir-vec3(0.,0.,.4),r)),15.)*.7;
	float tl2=(falloff2*ao+diff+spec)*falloff2;

	// sum tube lights - add ambient - apply tube intervall
	vec3 tl=((tl1+tl2)*(.5+tubeinterval*.5))*TUBE_COLOR;//*(1.+tun*.5);


	// --- Car lights ---

	// get the car turbines direction (aproximate)
	vec3 carlightdir1=normalize(p-carpos+vec3(.2,0.06,.15));
	vec3 carlightdir2=normalize(p-carpos+vec3(-.2,0.06,.15));
	vec3 carlightdir3=normalize(p-carpos+vec3(.2,0.06,-.35));
	vec3 carlightdir4=normalize(p-carpos+vec3(-.2,0.06,-.35));

	float cfalloff=pow(max(0.,1.-.1*distance(p,carpos)),13.); // car light falloff

	// accumulate diffuse
	diff=max(0.,dot(carlightdir1,-n))*.5;
	diff+=max(0.,dot(carlightdir2,-n))*.5;
	diff+=max(0.,dot(carlightdir3,-n))*.5;
	diff+=max(0.,dot(carlightdir4,-n))*.5;

	if (savehitcar<1.) diff*=clamp(1.-carlightdir1.y,0.,1.);
	
	// add ambient and save car lighting
	vec3 cl=TURBINES_COLOR*((diff+spec*.0)*cfalloff+cfalloff*.3)*1.2;
 	
	// --- Main light
	float sh=shadow(p, LIGHTDIR); // get shadow


	diff=max(0.,dot(LIGHTDIR,-n))*sh*1.3; // diffuse
	float amb=(.4+.6*camlight)*.6; // ambient+camlight
	spec=pow(max(0.,dot(dir,-r))*sh,20.)*SPECULAR; //specular
	if (savehitcar>0.) {diff*=.8;amb*=.3;}
	vec3 l=(amb*ao*AMBIENT_COLOR+diff*LIGHT_COLOR)+spec*LIGHT_COLOR;	

	if (col==TUBE_COLOR) l=.3+vec3(camlight)*.7; // special lighting for tubes

	return col*(l+cl+tl); // apply all lights to the color
}

// the planet shading...
// very simple and fast made, but for low res windowed mode it does the job :)
vec3 shadeplanet(vec3 pos, vec3 k) { 

	vec3 n=normalize(planetpos+pos+.2); // tweaked sphere normal
	float c=max(0.,dot(LIGHTDIR,normalize(k-n))); // surface value
	vec3 col=PLANET_COLOR+vec3(c,c*c,c*c*c)*.7; // surface coloring
	// add some noise
	float noi=max(0.,texture2D(iChannel1,n.yz*.5).x-.6);
	noi+=max(0.,texture2D(iChannel1,n.yz).x-.6);
	noi+=max(0.,texture2D(iChannel1,n.yz*2.).x-.6);
	col+=noi*(1.5-c)*.7;
	return col*max(0.,dot(LIGHTDIR,-n)); // diff light
}

// ------------------------------------------------------------------
//    Raymarching + FX rendering
// ------------------------------------------------------------------


vec3 raymarch(in vec3 from, in vec3 dir) 

{
	hitcar=0.;
	ref=0.;
	float totdist=0.;
	float glow=0.;
	float d=1000.;
	vec3 p=from;
	vec3 col=vec3(0.5);

	float deta=DETAIL*(1.+backcam); // lower detail for HUD cam
	vec3 carp=vec3(0.); // coordinates for car hit
	vec3 carn=vec3(0.); // normal for car
	float cardist=0.; // ray length for car
	vec3 odir=dir; // save original ray direction

	for (int i=0; i<RAY_STEPS; i++) {
		if (d>det && totdist<MAX_DIST) {
			d=de(p);
			p+=d*dir;
			det=max(deta,deta*totdist*.5*(1.+ref)); // scale detail with distance or reflect
			totdist+=d; 
			float gldist=det*8.; // background glow distance 
			if(d<gldist&&totdist<20.) glow+=max(0.,gldist-d)/gldist*exp(-.1*totdist); //accum glow
		}
		else break;
	}

	tubeinterval=abs(1.+cos(p.z*3.14159*.5))*.5; // set light tubes interval
	float cglow=1./(1.0+minL*minL*5000.0); // car glow
	float tglow=1./(1.0+minT*minT*5000.0)*(iParam4.w*.5+.5); // tubes glow
	float l=max(0.,dot(normalize(-dir),normalize(LIGHTDIR))); // lightdir gradient
	vec3 backg=AMBIENT_COLOR*.4*max(0.1,pow(l,5.)); // background
	float lglow=pow(l,50.)*.5+pow(l,200.)*.5; // sun glow

	if (d<.5) { // hit surface
		vec3 norm=normal(p); // get normal
		p=p-abs(d-det)*dir; // backstep
		col=shade(p, dir, norm); // get shading 
		col+=tglow*TUBE_COLOR*pow(tubeinterval,1.5)*4.*(iParam4.w*0.5+0.5); // add tube glow
		col = mix(backg, col, exp(-.015*pow(abs(totdist),1.5))); // distance fading

	} else { // hit background
	col=vec3(0,0,0);
		
col=backg;

	}
	// car shading

		// add turbine glows

	if (ref>0.) {
		ref=0.;
		col=shade(carp,odir,carn)+col*.3; // car shade + reflection
		// I wanted a lighter background for backward reflection
		l=max(0.,dot(normalize(-odir),normalize(LIGHTDIR)));
		backg=AMBIENT_COLOR*.4*max(0.1,pow(l,5.)); 
		col = mix(backg, col,exp(-.015*pow(abs(cardist),1.5))); // distance fading
	}

	
	col+=TURBINES_COLOR*pow(abs(cglow),2.)*.4;
	col+=TURBINES_COLOR*cglow*.15;


	return col; 
}

// simple HUD track graph with transparency
vec4 showtrack(vec2 p) {
	p.x+=.25;
	vec2 uv=p;
	float uvpos=uv.x+1.5;
	vec3 pth=path((uvpos-1.5)*30.+t)*.05;
	float curpos=path(t).x*.05;
	float curs=pow(max(0.,1.-length(uv+vec2(0.,curpos))*2.),10.)*max(0.5,sin(iGlobalTime*10.))*2.;
	uv.xy=uv.xy-(vec2(uvpos,0.))*rot(pth.x/uvpos);
	float dotline=pow(max(0.,1.-abs(uv.y)*5.),30.);
	float graph=(curs+dotline);
	return vec4((graph+.4)*HUD_COLOR,1.-.5*pow(abs(.025-mod(p.y*2.,.05))/.025,2.));
}


// ------------------------------------------------------------------
//    Main code - Camera - HUD 
// ------------------------------------------------------------------


void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	minL=minT=1000.; // initialize min distance glows
	fractrot=rot(iParam2.x*PI+triSin(iChannelTime[1])*2*PI*(iMouse.z/iResolution.x)); // mat2D for the fractal formula
	fractrot2=rot(iParam2.y*PI+triSin(iChannelTime[2])*2*PI*(iMouse.w/iResolution.y)); // mat2D for the fractal formula
	vec3 carpos0=vec3(0.,-0.2,.0); // set relative car pos (unused now, only sets height)
	carpos=vec3(carpos0+vec3(0.,0.,t)); // real car pos
	vec3 carmove=path(carpos.z); carmove.x*=1.+FOLD*.1; // get pos, exagerate x pos based on track width.
	carvec=normalize((path(carpos.z+2.)-carmove)*vec3(FOLD*.25,1.,1.)); // car fwd vector
	carrot=lookat(-carvec,vec3(0.,1.,0.)); // car rotation
	cartilt=rot(-carvec.x*2.); // xy rotation based on distance from center
	carpos.xy+=carmove.xy-vec2(carvec.x,0.)*FOLD*.5; // move away from center when turning
	float tim=iGlobalTime*12.0;
	trmx=mat2(cos(tim),-sin(tim),sin(tim),cos(tim));//the turbine spinner

	// --- camera & mouse ---
	
	vec2 uv = fragCoord.xy / iResolution.xy*2.-1.;
	uv.y*=iResolution.y/iResolution.x;
	vec2 mouse=(iMouse.xy/iResolution.xy-.5)*vec2(7.,1.5);
/*
	if (iMouse.z<1.) { // if no mouse, alternate rear and back cam
		mouse=vec2(sin(iGlobalTime)*.7,2.+sin(iGlobalTime*.2)*.22)
			*min(0.,sign(10.-mod(iGlobalTime,20.)));
	}*/
	vec3 dir=normalize(vec3(uv*.8,1.));

	vec3 campos=vec3(0.,0.2,-.6); // original relative camera position
	//rotate camera with mouse
	campos.yz=(campos.yz-carpos0.yz)*rot(mouse.y)+carpos0.yz;
	campos.xz=(campos.xz-carpos0.xz)*rot(mouse.x)+carpos0.xz;
	campos.x-=carvec.x*FOLD; // follow car x pos a bit when turning

	vec3 from;
	
	float fixcam=5.;
	float mt=mod(t/SPEED,fixcam*2.);
	//fixed cam every 15 seconds, random position, point at car position

		from=path(t+campos.z)+campos;
		dir.y-=.3*campos.z;
		dir=lookat(normalize(carpos-from),vec3(0.,1.,0.))*normalize(dir);

	vec4 hud=vec4(0.);

	vec3 color=raymarch(from,dir); 	// Raymarch scene
	color=clamp(color,vec3(.0),vec3(1.));
	if (backcam>0.) { //if HUD cam, apply post effect
		color=(.2+pow(length(color),1.7)*.5)*HUD_COLOR
			*(1.-.5*pow(abs(.025-mod(uv.y*.9,.05))/.025,2.))*.9;
	}
	
	color=hud.rgb*hud.a+color*(1.-hud.a);//HUD transparency

	//color adjustments
	color=pow(abs(color),vec3(GAMMA))*BRIGHTNESS;
	color=mix(vec3(length(color)),color,SATURATION);
	fragColor = vec4(color,1.);
}


);


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////
ShaderMaker::ShaderMaker():CFreeFrameGLPlugin()
{

	/*
	// Debug console window so printf works
	FILE* pCout; // should really be freed on exit 
	AllocConsole();
	freopen_s(&pCout, "CONOUT$", "w", stdout); 
	printf("Shader Maker Vers 1.004\n");
	printf("GLSL version [%s]\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	*/

	// Input properties allow for no texture or for four textures
	SetMinInputs(0);
	SetMaxInputs(2); // TODO - 4 inputs

	// Parameters
	SetParamInfo(FFPARAM_SPEED,         "Speed",         FF_TYPE_STANDARD, 0.5f); m_UserSpeed = 0.0f;
	SetParamInfo(FFPARAM_SPEED2,         "Speed 2",         FF_TYPE_STANDARD, 0.5f); m_UserSpeed2 = 0.0f;
	SetParamInfo(FFPARAM_SPEED3,         "Speed 3",         FF_TYPE_STANDARD, 0.5f); m_UserSpeed3 = 0.0f;
	SetParamInfo(FFPARAM_SPEED4,         "Speed 4",         FF_TYPE_STANDARD, 0.5f); m_UserSpeed4 = 0.0f;

	SetParamInfo(FFPARAM_MOUSEX,        "CamrotX",       FF_TYPE_STANDARD, 0.0f); m_UserMouseX = 0.0f;
	SetParamInfo(FFPARAM_MOUSEY,        "CamrotY",       FF_TYPE_STANDARD, 0.0f); m_UserMouseY = 0.0f;
	SetParamInfo(FFPARAM_MOUSELEFTX,    "RotSway1",  FF_TYPE_STANDARD, 0.0f); m_UserMouseLeftX = 0.0f;
	SetParamInfo(FFPARAM_MOUSELEFTY,    "RotSway2",  FF_TYPE_STANDARD, 0.1f); m_UserMouseLeftY = 0.5f;
	

	SetParamInfo(FFPARAM_RED,           "Red",           FF_TYPE_STANDARD, 0.75f); m_UserRed = 07.5f;
	SetParamInfo(FFPARAM_GREEN,         "Green",         FF_TYPE_STANDARD, 0.5f); m_UserGreen = 0.5f;
	SetParamInfo(FFPARAM_BLUE,          "Blue",          FF_TYPE_STANDARD, 0.5f); m_UserBlue = 0.5f;
	SetParamInfo(FFPARAM_ALPHA,         "Alpha",         FF_TYPE_STANDARD, 1.0f); m_UserAlpha = 1.0f;
	
	SetParamInfo(FFPARAM_PARAM2_X,         "Angle1",         FF_TYPE_STANDARD, 0.5f); m_UserParam2x = 0.0f;
	SetParamInfo(FFPARAM_PARAM2_Y,         "Angle2",         FF_TYPE_STANDARD, 0.75f); m_UserParam2y = 0.0f;
	SetParamInfo(FFPARAM_PARAM2_Z,         "Car Scale",         FF_TYPE_STANDARD, 0.75f); m_UserParam2z = 0.0f;	
	SetParamInfo(FFPARAM_PARAM2_W,         "unused",         FF_TYPE_STANDARD, 0.5f); m_UserParam2w = 0.25f;

	SetParamInfo(FFPARAM_PARAM3_X,         "JuliaX",         FF_TYPE_STANDARD, 0.750f); m_UserParam3x = 0.750f;
	SetParamInfo(FFPARAM_PARAM3_Y,         "JuliaY",         FF_TYPE_STANDARD, 0.750f); m_UserParam3y = 0.750f;
	SetParamInfo(FFPARAM_PARAM3_Z,         "JuliaZ",         FF_TYPE_STANDARD, 0.750f); m_UserParam3z = 0.750f;	
	SetParamInfo(FFPARAM_PARAM3_W,         "JuliaW",         FF_TYPE_STANDARD, 0.70f); m_UserParam3w = 0.75f;
	
	SetParamInfo(FFPARAM_PARAM4_X,         "Tube Pos X",         FF_TYPE_STANDARD, 0.750f); m_UserParam4x = 0.0f;
	SetParamInfo(FFPARAM_PARAM4_Y,         "Tube Pos Y ",         FF_TYPE_STANDARD, 0.250f); m_UserParam4y = 0.0f;
	SetParamInfo(FFPARAM_PARAM4_Z,         "unused",         FF_TYPE_STANDARD, 0.750f); m_UserParam4z = 0.0f;	
	SetParamInfo(FFPARAM_PARAM4_W,         "unused",         FF_TYPE_STANDARD, 0.75f); m_UserParam4w = 0.75f;
	/*
	SetParamInfo(FFPARAM_PARAM3_CAMERA_X,"Camera X",         FF_TYPE_STANDARD, 0.750f); m_Camera_x = .75f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_Y,"Camera Y",         FF_TYPE_STANDARD, 0.50f); m_Camera_y= 0.5f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_Z,"Camera Z",         FF_TYPE_STANDARD, 0.40f); m_Camera_z = .45f;


	// 0.5 means the middle = because its multiplied *2 and -1 ;)
	SetParamInfo(FFPARAM_PARAM3_CAMERA_TARGET_X,"Camera Target X",         FF_TYPE_STANDARD, 0.5f); m_CameraTarget_x= 0.50f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_TARGET_Y,"Camera Target Y",         FF_TYPE_STANDARD, 0.50f); m_CameraTarget_y = 0.50f;
	SetParamInfo(FFPARAM_PARAM3_CAMERA_TARGET_Z,"Camera Target Z",         FF_TYPE_STANDARD, 0.50f); m_CameraTarget_z = 0.50f;
	*/

	// Set defaults
	SetDefaults();

	// Flags
	bInitialized = false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////
FFResult ShaderMaker::InitGL(const FFGLViewportStruct *vp)
{
	// initialize gl extensions and make sure required features are supported
	m_extensions.Initialize();
	if (m_extensions.multitexture==0 || m_extensions.ARB_shader_objects==0)
		return FF_FAIL;

	// Set the viewport size
	// Actually it has to be checked in ProcessOpenGL because it could change
	m_vpWidth  = (float)vp->width;
	m_vpHeight = (float)vp->height;

	// Start the clock
	StartCounter();

	// Load the shader
	std::string shaderString = fragmentShaderCode;
	bInitialized = LoadShader(shaderString);

	return FF_SUCCESS;
}

ShaderMaker::~ShaderMaker()
{
	// Not using this but it is here just in case
}


FFResult ShaderMaker::DeInitGL()
{
	if(bInitialized)
		m_shader.UnbindShader();

	m_shader.FreeGLResources();

	if(m_fbo) m_extensions.glDeleteFramebuffersEXT(1, &m_fbo);
	if(m_glTexture0) glDeleteTextures(1, &m_glTexture0);
	if(m_glTexture1) glDeleteTextures(1, &m_glTexture1);
	if(m_glTexture2) glDeleteTextures(1, &m_glTexture2);
	if(m_glTexture3) glDeleteTextures(1, &m_glTexture3);
	m_glTexture0 = 0;
	m_glTexture1 = 0;
	m_glTexture2 = 0;
	m_glTexture3 = 0;
	m_fbo = 0;
	bInitialized = false;

	return FF_SUCCESS;
}

FFResult ShaderMaker::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
	FFGLTextureStruct Texture0;
	FFGLTextureStruct Texture1;
	// TODO
	// FFGLTextureStruct Texture2;
	// FFGLTextureStruct Texture3;
	FFGLTexCoords maxCoords;
	time_t datime;
	struct tm tmbuff;

	if(bInitialized) {

		// To the host this is an effect plugin, but it can be either a source or an effect
		// and will work without any input, so we still start up if even there is no input texture

		// Set the global viewport resolution from OpenGL now for certainty
		// It could be different to that receieved by InitGL
		float vpdim[4];
		glGetFloatv(GL_VIEWPORT, vpdim);
		m_vpWidth  = vpdim[2];
		m_vpHeight = vpdim[3];

		// Is there is texture needed by the shader ?
		if(m_inputTextureLocation >= 0 || m_inputTextureLocation1 >= 0) {

			// Is there a texture available ?
			if(m_inputTextureLocation >= 0 && pGL->numInputTextures > 0 && pGL->inputTextures[0] != NULL) {

				Texture0 = *(pGL->inputTextures[0]);
				maxCoords = GetMaxGLTexCoords(Texture0);

				// Delete the local texture if the incoming size is different
				if((int)m_channelResolution[0][0] != Texture0.Width 
				|| (int)m_channelResolution[0][1] != Texture0.Height) {
					if(m_glTexture0 > 0) {
						glDeleteTextures(1, &m_glTexture0);
						m_glTexture0 = 0; // This is needed or the local texture is not re-created in CreateRectangleTexture (30.03.15)
					}
				}

				// Set the resolution of the first texture size
				m_channelResolution[0][0] = (float)Texture0.Width;
				m_channelResolution[0][1] = (float)Texture0.Height;

				// For a power of two texture, the size will be different to the hardware size.
				// The shader will not compensate for this, so we have to create another texture
				// the same size as the resolution we set to the shader.  Also the shader needs
				// textures created with wrapping REPEAT rather than CLAMP to edge. So we ALWAYS
				// create such a texture and use it for every frame. The texture is re-created
				// if the texture size changes 
				CreateRectangleTexture(Texture0, maxCoords, m_glTexture0, GL_TEXTURE0, m_fbo, pGL->HostFBO);
				// Now we have a local texture of the right size and type
				// Filled with the data from the incoming Freeframe texture
			}

			// Repeat if there is a second incoming texture and the shader needs it
			if(m_inputTextureLocation1 >= 0 && pGL->numInputTextures > 1 && pGL->inputTextures[1] != NULL) {
				Texture1 = *(pGL->inputTextures[1]);
				maxCoords = GetMaxGLTexCoords(Texture1);
				if((int)m_channelResolution[1][0] != Texture1.Width 
				|| (int)m_channelResolution[1][1] != Texture1.Height) {
					if(m_glTexture1 > 0) {
						glDeleteTextures(1, &m_glTexture1);
						m_glTexture1 = 0;
					}
				}
				// Set the channel resolution of the second texture size
				m_channelResolution[1][0] = (float)Texture1.Width;
				m_channelResolution[1][1] = (float)Texture1.Height;
				CreateRectangleTexture(Texture1, maxCoords, m_glTexture1, GL_TEXTURE1, m_fbo, pGL->HostFBO);
			}

			/*
			// And for textures 3 and 4
			if(m_inputTextureLocation2 >= 0 && pGL->numInputTextures > 2 && pGL->inputTextures[2] != NULL) {
				Texture2 = *(pGL->inputTextures[2]);
				maxCoords = GetMaxGLTexCoords(Texture2);
				if((int)m_channelResolution[2][0] != Texture2.Width 
				|| (int)m_channelResolution[2][1] != Texture2.Height) {
					if(m_glTexture2 > 0) {
						glDeleteTextures(1, &m_glTexture2);
						m_glTexture2 = 0;
					}
				}				
				m_channelResolution[2][0] = (float)Texture2.Width;
				m_channelResolution[2][1] = (float)Texture2.Height;
				CreateRectangleTexture(Texture2, maxCoords, m_glTexture2, GL_TEXTURE1, m_fbo, pGL->HostFBO);
			}

			if(m_inputTextureLocation3 >= 0 && pGL->numInputTextures > 3 && pGL->inputTextures[3] != NULL) {
				Texture3 = *(pGL->inputTextures[3]);
				maxCoords = GetMaxGLTexCoords(Texture3);
				if((int)m_channelResolution[3][0] != Texture3.Width 
				|| (int)m_channelResolution[3][1] != Texture3.Height) {
					if(m_glTexture3 > 0) {
						glDeleteTextures(1, &m_glTexture3);
						m_glTexture3 = 0;
					}
				}
				m_channelResolution[3][0] = (float)Texture3.Width;
				m_channelResolution[3][1] = (float)Texture3.Height;
				CreateRectangleTexture(Texture3, maxCoords, m_glTexture3, GL_TEXTURE1, m_fbo, pGL->HostFBO);
			}
			*/

		} // endif shader uses a texture
	
		// Calculate elapsed time
		lastTime = elapsedTime;
		elapsedTime = GetCounter()/1000.0; // In seconds - higher resolution than timeGetTime()
		m_time = m_time + (float)(elapsedTime-lastTime)*(m_UserSpeed*8.0f-4.0); // increment scaled by user input 0.0 - 2.0

		m_time2 = m_time2 + (float)(elapsedTime-lastTime)*(m_UserSpeed2*8.0f-4.0); // increment scaled by user input 0.0 - 2.0

		m_time3 = m_time3 + (float)(elapsedTime-lastTime)*(m_UserSpeed3*8.0f-4.0); // increment scaled by user input 0.0 - 2.0

		m_time4= m_time4 + (float)(elapsedTime-lastTime)*(m_UserSpeed4*8.0f-4.0); // increment scaled by user input 0.0 - 2.0

		// Just pass elapsed time for individual channel times
		m_channelTime[0] = m_time;
		m_channelTime[1] = m_time2;
		m_channelTime[2] = m_time3;
		m_channelTime[3] = m_time4;

		// Calculate date vars
		time(&datime);
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
		localtime_s(&tmbuff, &datime);
#else
        localtime_r(&datime, &tmbuff);
#endif
		m_dateYear = (float)tmbuff.tm_year;
		m_dateMonth = (float)tmbuff.tm_mon+1;
		m_dateDay = (float)tmbuff.tm_mday;
		m_dateTime = (float)(tmbuff.tm_hour*3600 + tmbuff.tm_min*60 + tmbuff.tm_sec);

		// activate our shader
		m_shader.BindShader();

		//
		// Assign values and set the uniforms to the shader
		//

		//
		// Common
		//

		// First input texture
		// The shader will use the first texture bound to GL texture unit 0
		if(m_inputTextureLocation >= 0 && Texture0.Handle > 0) {
			m_extensions.glUniform1iARB(m_inputTextureLocation, 0);
		}

		// Second input texture
		// The shader will use the texture bound to GL texture unit 1
		if(m_inputTextureLocation1 >= 0 && Texture1.Handle > 0)
			m_extensions.glUniform1iARB(m_inputTextureLocation1, 1);

		/*
		// 4 channels
		if(m_inputTextureLocation2 >= 0 && Texture2.Handle > 0)
			m_extensions.glUniform1iARB(m_inputTextureLocation2, 2);

		if(m_inputTextureLocation3 >= 0 && Texture3.Handle > 0)
			m_extensions.glUniform1iARB(m_inputTextureLocation3, 3);
		*/

		// Elapsed time
		if(m_timeLocation >= 0) 
			m_extensions.glUniform1fARB(m_timeLocation, m_time);
	 	
		m_extensions.glUniform4fARB(m_iParam1Location, m_UserMouseX*2.0-1.0, m_UserMouseY*2.0-1.0, m_UserMouseLeftX*2.0-1.0, m_UserMouseLeftY*2.0-1.0); 
		m_extensions.glUniform4fARB(m_iParam2Location, m_UserParam2x*2.0-1.0,m_UserParam2y*2.0-1.0,m_UserParam2z*2.0-1.0,m_UserParam2w*2.0-1.0); 
		m_extensions.glUniform4fARB(m_iParam3Location, m_UserParam3x*2.0-1.0,m_UserParam3y*2.0-1.0,m_UserParam3z*2.0-1.0,m_UserParam3w*2.0-1.0); 
		m_extensions.glUniform4fARB(m_iParam4Location, m_UserParam4x*2.0-1.0,m_UserParam4y*2.0-1.0,m_UserParam4z*2.0-1.0,m_UserParam4w*2.0-1.0); 
		//
		// GLSL sandbox
		//
		// resolution (viewport size)
		if(m_screenLocation >= 0) 
			m_extensions.glUniform2fARB(m_screenLocation, m_vpWidth, m_vpHeight); 

		// mouse - Mouse position
		if(m_mouseLocation >= 0) { // Vec2 - normalized
			m_mouseX = m_UserMouseX;
			m_mouseY = m_UserMouseY;
			m_extensions.glUniform2fARB(m_mouseLocation, m_mouseX, m_mouseY); 
		}
		if(m_iCameraLocation >= 0) { // Vec3- normalized 
			m_extensions.glUniform3fARB(m_iCameraLocation, m_Camera_x*2.0f-1.0f,m_Camera_y*2.0f-1.0f,m_Camera_z*2.0f-1.0f); 
		}
		if(m_iCameraTargetLocation >= 0) { // Vec3- normalized 
			m_extensions.glUniform3fARB(m_iCameraTargetLocation, m_CameraTarget_x*2.0f-1.0f,m_CameraTarget_y*2.0f-1.0f,m_CameraTarget_z*2.0f-1.0f); 
		}

		// surfaceSize - Mouse left drag position - in pixel coordinates
		if(m_surfaceSizeLocation >= 0) {
			m_mouseLeftX = m_UserMouseLeftX*m_vpWidth;
			m_mouseLeftY = m_UserMouseLeftY*m_vpHeight;
			m_extensions.glUniform2fARB(m_surfaceSizeLocation, m_mouseLeftX, m_mouseLeftY);
		}

		//
		// Shadertoy

		// iMouse
		// xy contain the current pixel coords (if LMB is down);
		// zw contain the click pixel.
		// Modified here equivalent to mouse unclicked or left button dragged
		// The mouse is not being simulated, they are just inputs that can be used within the shader.
		if(m_mouseLocationVec4 >= 0) {
			// Convert from 0-1 to pixel coordinates for ShaderToy
			// Here we use the resolution rather than the screen
			m_mouseX     = m_UserMouseX*m_vpWidth;
			m_mouseY     = m_UserMouseY*m_vpHeight;
			m_mouseLeftX = m_UserMouseLeftX*m_vpWidth;
			m_mouseLeftY = m_UserMouseLeftY*m_vpHeight;
			m_extensions.glUniform4fARB(m_mouseLocationVec4, m_mouseX, m_mouseY, m_mouseLeftX, m_mouseLeftY); 
		}

		// iResolution - viewport resolution
		if(m_resolutionLocation >= 0) // Vec3
			m_extensions.glUniform3fARB(m_resolutionLocation, m_vpWidth, m_vpHeight, 1.0); 

		// Channel resolutions are linked to the actual texture resolutions - the size is set in ProcessOpenGL
		// Global resolution is the viewport
		if(m_channelresolutionLocation >= 0) {
			// uniform vec3	iChannelResolution[4]
			// 4 channels Vec3. Float array is 4 rows, 3 cols
			// TODO - 4 channels - 2 & 3 are unused so will not have a texture anyway
			m_channelResolution[2][0] = m_vpWidth;
			m_channelResolution[2][1] = m_vpHeight;
			m_channelResolution[2][2] = 1.0;
			m_channelResolution[3][0] = m_vpWidth;
			m_channelResolution[3][1] = m_vpHeight;
			m_channelResolution[3][2] = 1.0;
			m_extensions.glUniform3fvARB(m_channelresolutionLocation, 4, (GLfloat *)m_channelResolution);
		}

		// iDate - vec4
		if(m_dateLocation >= 0) 
			m_extensions.glUniform4fARB(m_dateLocation, m_dateYear, m_dateMonth, m_dateDay, m_dateTime);

		// Channel elapsed time - vec4
		if(m_channeltimeLocation >= 0)
			m_extensions.glUniform1fvARB(m_channeltimeLocation, 4, m_channelTime);

		// Extras - input colour is linked to the user controls Red, Green, Blue, Alpha
		if(m_inputColourLocation >= 0)
			m_extensions.glUniform4fARB(m_inputColourLocation, m_UserRed, m_UserGreen, m_UserBlue, m_UserAlpha);


		// Bind a texture if the shader needs one
		if(m_inputTextureLocation >= 0 && Texture0.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE0);
			// Has the local texture been created
			// TODO - it should have been always created so this logic can be changed
			if(m_glTexture0 > 0) 
				glBindTexture(GL_TEXTURE_2D, m_glTexture0);
			else 
				glBindTexture(GL_TEXTURE_2D, Texture0.Handle);
		}

		// If there is a second texture, bind it to texture unit 1
		if(m_inputTextureLocation1 >= 0 && Texture1.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE1);
			if(m_glTexture1 > 0)
				glBindTexture(GL_TEXTURE_2D, m_glTexture1);
			else
				glBindTexture(GL_TEXTURE_2D, Texture1.Handle);
		}

		/*
		// Texture units 2 and 3
		if(m_inputTextureLocation2 >= 0 && Texture2.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE2);
			if(m_glTexture2 > 0)
				glBindTexture(GL_TEXTURE_2D, m_glTexture2);
			else
				glBindTexture(GL_TEXTURE_2D, Texture2.Handle);
		}

		if(m_inputTextureLocation3 >= 0 && Texture3.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE3);
			if(m_glTexture3 > 0)
				glBindTexture(GL_TEXTURE_2D, m_glTexture3);
			else
				glBindTexture(GL_TEXTURE_2D, Texture3.Handle);
		}
		*/

		// Do the draw for the shader to work
		glEnable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);	
		glVertex2f(-1.0, -1.0);
		glTexCoord2f(0.0, 1.0);	
		glVertex2f(-1.0,  1.0);
		glTexCoord2f(1.0, 1.0);	
		glVertex2f( 1.0,  1.0);
		glTexCoord2f(1.0, 0.0);	
		glVertex2f( 1.0, -1.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);

		/*
		// unbind input texture 3
		if(m_inputTextureLocation3 >= 0 && Texture3.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// unbind input texture 2
		if(m_inputTextureLocation2 >= 0 && Texture2.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		*/

		// unbind input texture 1
		if(m_inputTextureLocation1 >= 0 && Texture1.Handle > 0) {
			m_extensions.glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// unbind input texture 0
		m_extensions.glActiveTexture(GL_TEXTURE0); // default
		if(m_inputTextureLocation >= 0 && Texture0.Handle > 0)
			glBindTexture(GL_TEXTURE_2D, 0);

		// unbind the shader
		m_shader.UnbindShader();

	} // endif bInitialized

	return FF_SUCCESS;
}

char * ShaderMaker::GetParameterDisplay(DWORD dwIndex) {

	memset(m_DisplayValue, 0, 15);
	switch (dwIndex) {
		
	case FFPARAM_PARAM3_CAMERA_X:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_Camera_x));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_Y:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_Camera_y));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_Z:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_Camera_z));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_X:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_CameraTarget_x));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Y:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_CameraTarget_y));
			return m_DisplayValue;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Z:
		cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_CameraTarget_z));
			return m_DisplayValue;
	
		case FFPARAM_SPEED:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed*100.0));
			return m_DisplayValue;
	
		case FFPARAM_SPEED2:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed2*100.0));
			return m_DisplayValue;
	
		case FFPARAM_SPEED3:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed3*100.0));
			return m_DisplayValue;
	
		case FFPARAM_SPEED4:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserSpeed4*100.0));
			return m_DisplayValue;
	
		case FFPARAM_PARAM2_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam2x));
			return m_DisplayValue;
		case FFPARAM_PARAM2_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam2y));
			return m_DisplayValue;
		case FFPARAM_PARAM2_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam2z));
			return m_DisplayValue;
		case FFPARAM_PARAM2_W:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam2w));
			return m_DisplayValue;
			
		case FFPARAM_PARAM3_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3x));
			return m_DisplayValue;
		case FFPARAM_PARAM3_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3y));
			return m_DisplayValue;
		case FFPARAM_PARAM3_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3z));
			return m_DisplayValue;
		case FFPARAM_PARAM3_W:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam3w));
			return m_DisplayValue;
			
		case FFPARAM_PARAM4_X:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4x));
			return m_DisplayValue;
		case FFPARAM_PARAM4_Y:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4y));
			return m_DisplayValue;
		case FFPARAM_PARAM4_Z:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4z));
			return m_DisplayValue;
		case FFPARAM_PARAM4_W:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserParam4w));
			return m_DisplayValue;

		case FFPARAM_MOUSEX:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseX*m_vpWidth));
			return m_DisplayValue;

		case FFPARAM_MOUSEY:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseY*m_vpHeight));
			return m_DisplayValue;

		case FFPARAM_MOUSELEFTX:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseLeftX*m_vpWidth));
			return m_DisplayValue;

		case FFPARAM_MOUSELEFTY:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserMouseLeftY*m_vpHeight));
			return m_DisplayValue;

		case FFPARAM_RED:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserRed*256.0));
			return m_DisplayValue;

		case FFPARAM_GREEN:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserGreen*256.0));
			return m_DisplayValue;

		case FFPARAM_BLUE:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserBlue*256.0));
			return m_DisplayValue;

		case FFPARAM_ALPHA:
			cross_secure_sprintf(m_DisplayValue, 16, "%d", (int)(m_UserAlpha*256.0));
			return m_DisplayValue;

		default:
			return m_DisplayValue;
	}
	return NULL;
}

FFResult ShaderMaker::GetInputStatus(DWORD dwIndex)
{
	DWORD dwRet = FF_INPUT_NOTINUSE;

	switch (dwIndex) {

		case 0 :
			if(m_inputTextureLocation >= 0)
				dwRet = FF_INPUT_INUSE;
			break;

		case 1 :
			if(m_inputTextureLocation1 >= 0)
				dwRet = FF_INPUT_INUSE;
			break;

		/* TODO - 4 channels
		case 2 :
			if(m_inputTextureLocation2 >= 0)
				dwRet = FF_INPUT_INUSE;
			break;

		case 3 :
			if(m_inputTextureLocation3 >= 0)
				dwRet = FF_INPUT_INUSE;
			break;
		*/


		default :
			break;

	}

	return dwRet;

}

float ShaderMaker::GetFloatParameter(unsigned int index)
{
	switch (index) {
		
	case FFPARAM_PARAM3_CAMERA_X:
		return  m_Camera_x;
		
	case FFPARAM_PARAM3_CAMERA_Y:
		return  m_Camera_y;
		
	case FFPARAM_PARAM3_CAMERA_Z:
		return  m_Camera_z;
			
	case FFPARAM_PARAM3_CAMERA_TARGET_X:
		return  m_CameraTarget_x;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Y:
		return  m_CameraTarget_y;
		
	case FFPARAM_PARAM3_CAMERA_TARGET_Z:
		return  m_CameraTarget_z;
	
		case FFPARAM_SPEED:
			return  m_UserSpeed;
	
		case FFPARAM_SPEED2:
			return  m_UserSpeed2;
	
		case FFPARAM_SPEED3:
			return  m_UserSpeed3;
	
		case FFPARAM_SPEED4:
			return  m_UserSpeed4;
	
		case FFPARAM_MOUSEX:
			return  m_UserMouseX;

		case FFPARAM_MOUSEY:
			return  m_UserMouseY;

		case FFPARAM_MOUSELEFTX:
			return m_UserMouseLeftX;

		case FFPARAM_MOUSELEFTY:
			return m_UserMouseLeftY;

		case FFPARAM_RED:
			return m_UserRed;

		case FFPARAM_GREEN:
			return m_UserGreen;

		case FFPARAM_BLUE:
			return m_UserBlue;

		case FFPARAM_ALPHA:
			return m_UserAlpha;

		case FFPARAM_PARAM2_X:
			return m_UserParam2x;
case FFPARAM_PARAM2_Y:
			return m_UserParam2y;
case FFPARAM_PARAM2_Z:
			return m_UserParam2z;
case FFPARAM_PARAM2_W:
			return m_UserParam2w;


		case FFPARAM_PARAM3_X:
			return m_UserParam3x;
case FFPARAM_PARAM3_Y:
			return m_UserParam3y;
case FFPARAM_PARAM3_Z:
			return m_UserParam3z;
case FFPARAM_PARAM3_W:
			return m_UserParam3w;


		case FFPARAM_PARAM4_X:
			return m_UserParam4x;
case FFPARAM_PARAM4_Y:
			return m_UserParam4y;
case FFPARAM_PARAM4_Z:
			return m_UserParam4z;
case FFPARAM_PARAM4_W:
			return m_UserParam4w;

		default:
			return FF_FAIL;
	}
}

FFResult ShaderMaker::SetFloatParameter(unsigned int index, float value)
{
		switch (index) {
			
		case FFPARAM_PARAM3_CAMERA_X:
			m_Camera_x = value;
				break;

		case FFPARAM_PARAM3_CAMERA_Y:
			m_Camera_y = value;
				break;

		case FFPARAM_PARAM3_CAMERA_Z:
			m_Camera_z = value;
				break;

			
		case FFPARAM_PARAM3_CAMERA_TARGET_X:
			m_CameraTarget_x = value;
				break;

		case FFPARAM_PARAM3_CAMERA_TARGET_Y:
			m_CameraTarget_y = value;
				break;

		case FFPARAM_PARAM3_CAMERA_TARGET_Z:
			m_CameraTarget_z = value;
				break;

			
			case FFPARAM_SPEED:
				m_UserSpeed = value;
				break;
				
			case FFPARAM_SPEED2:
				m_UserSpeed2 = value;
				break;

			case FFPARAM_SPEED3:
				m_UserSpeed3 = value;
				break;

			case FFPARAM_SPEED4:
				m_UserSpeed4 = value;
				break;
			
			case FFPARAM_PARAM2_X:
				m_UserParam2x = value;
				break;
			case FFPARAM_PARAM2_Y:
				m_UserParam2y = value;
				break;
			case FFPARAM_PARAM2_Z:
				m_UserParam2z = value;
				break;
			case FFPARAM_PARAM2_W:
				m_UserParam2w = value;
				break;

			
			case FFPARAM_PARAM3_X:
				m_UserParam3x = value;
				break;
			case FFPARAM_PARAM3_Y:
				m_UserParam3y = value;
				break;
			case FFPARAM_PARAM3_Z:
				m_UserParam3z = value;
				break;
			case FFPARAM_PARAM3_W:
				m_UserParam3w = value;
				break;

			
			case FFPARAM_PARAM4_X:
				m_UserParam4x = value;
				break;
			case FFPARAM_PARAM4_Y:
				m_UserParam4y = value;
				break;
			case FFPARAM_PARAM4_Z:
				m_UserParam4z = value;
				break;
			case FFPARAM_PARAM4_W:
				m_UserParam4w = value;
				break;


			case FFPARAM_MOUSEX:
				m_UserMouseX = value;
				break;

			case FFPARAM_MOUSEY:
				m_UserMouseY = value;
				break;

			case FFPARAM_MOUSELEFTX:
				m_UserMouseLeftX = value;
				break;

			case FFPARAM_MOUSELEFTY:
				m_UserMouseLeftY = value;
				break;

			case FFPARAM_RED:
				m_UserRed = value;
				break;

			case FFPARAM_GREEN:
				m_UserGreen = value;
				break;

			case FFPARAM_BLUE:
				m_UserBlue = value;
				break;

			case FFPARAM_ALPHA:
				m_UserAlpha = value;
				break;

			default:
				return FF_FAIL;
		}
		return FF_SUCCESS;
}

void ShaderMaker::SetDefaults() {

    elapsedTime            = 0.0;
    lastTime               = 0.0;
#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
	PCFreq                 = 0.0;
	CounterStart           = 0;
#else
    start = std::chrono::steady_clock::now();
#endif

	m_mouseX               = 0.5;
	m_mouseY               = 0.5;
	m_mouseLeftX           = 0.5;
	m_mouseLeftY           = 0.5;

	m_UserMouseX           = 0.5;
	m_UserMouseY           = 0.5;
	m_UserMouseLeftX       = 0.5;
	m_UserMouseLeftY       = 0.5;
	
	m_time                 = 0.0;
	m_time2                 = 0.0;
	m_time3                 = 0.0;
	m_time4                 = 0.0;
	m_dateYear             = 0.0;
	m_dateMonth            = 0.0;
	m_dateDay              = 0.0;
	m_dateTime             = 0.0;

	m_channelTime[0]       = 0.0;
	m_channelTime[1]       = 0.0;
	m_channelTime[2]       = 0.0;
	m_channelTime[3]       = 0.0;

	// ShaderToy -  Vec3 - 4 channels 
	m_channelResolution[0][0] = 0.0; // 0 is width
	m_channelResolution[0][1] = 0.0; // 1 is height
	m_channelResolution[0][2] = 1.0; // 2 is depth

	m_channelResolution[1][0] = 0.0;
	m_channelResolution[1][1] = 0.0;
	m_channelResolution[1][2] = 1.0;

	m_channelResolution[2][0] = 0.0;
	m_channelResolution[2][1] = 0.0;
	m_channelResolution[2][2] = 1.0;

	m_channelResolution[3][0] = 0.0;
	m_channelResolution[3][1] = 0.0;
	m_channelResolution[4][2] = 1.0;
	
	m_UserSpeed               = 0.5;
	m_UserSpeed2               = 0.5;
	m_UserSpeed3               = 0.5;
	m_UserSpeed4               = 0.5;
	m_UserMouseX              = 0.5;
	m_UserMouseY              = 0.5;
	m_UserMouseLeftX          = 0.5;
	m_UserMouseLeftY          = 0.5;

	// OpenGL
	m_glTexture0              = 0;
	m_glTexture1              = 0;
	m_glTexture2              = 0;
	m_glTexture3              = 0;
	m_fbo                     = 0;

}

bool ShaderMaker::LoadShader(std::string shaderString) {
		
		std::string stoyUniforms;

		//
		// Extra uniforms specific to ShaderMaker for buth GLSL Sandbox and ShaderToy
		// For GLSL Sandbox, the extra "inputColour" uniform has to be typed into the shader
		//		uniform vec4 inputColour
		static char *extraUniforms = { "uniform vec4 inputColour;\n" };
		
		// Is it a GLSL Sandbox file?
		// look for "uniform float time;". If it does not exist it is a ShaderToy file
		// This is an exact string, so the shader has to have it.
		if(strstr(shaderString.c_str(), "uniform float time;") == 0) {
			//
			// ShaderToy file
			//
			// Shadertoy does not include uniform variables in the source file so add them here
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
			static char *uniforms = { 
				
				"uniform vec3 iResolution;\n"
									  "uniform float iGlobalTime;\n"
									  "uniform vec4 iMouse;\n"
									  "uniform vec4 iDate;\n"
									  "uniform float iChannelTime[4];\n"
									  "uniform vec3 iChannelResolution[4];\n"
									  "uniform sampler2D iChannel0;\n"
									  "uniform sampler2D iChannel1;\n"
									  "uniform sampler2D iChannel2;\n"
									  "uniform sampler2D iChannel3;\n"
									  // ck hackss
									  "#define PI 3.14159265359 \n"
									  // param1 is mapped input mouse without resolution hassle and ranging from -1 to 1
									  "uniform vec4 iParam1;\n"
									  "uniform vec4 iParam2;\n"
									  "uniform vec4 iParam3;\n"
									  "uniform vec4 iParam4;\n"
									  "uniform vec3 iCamera;\n"
									  "uniform vec3 iCameraTarget;\n"
			
			};
			
			stoyUniforms = uniforms;
			stoyUniforms += extraUniforms;
			stoyUniforms += shaderString; // add the rest of the shared content

			// It might be a revised ShaderToy file with "mainImage" instead of "main"
			if(strstr(shaderString.c_str(), "void mainImage") != 0) {
				//
				// If it is a revised spec ShaderToy file, add a fix at the end for GLSL compatibility
				//
				// Credit Eric Newman 
				// http://magicmusicvisuals.com/forums/viewtopic.php?f=2&t=196
				//
				static char *stoyMainFunction = { "void main(void) {\n"
												  "    mainImage(gl_FragColor, gl_FragCoord.xy);\n"
												  "}\n" };
				stoyUniforms += stoyMainFunction;
			}

			shaderString = stoyUniforms; // the final string
		}
	
		// initialize gl shader
		m_shader.SetExtensions(&m_extensions);
		if (!m_shader.Compile(vertexShaderCode, shaderString.c_str())) {
			// SelectSpoutPanel("Shader compile error");
		 


			return false;
		}
		else {
			// activate our shader
			bool success = false;
			if (m_shader.IsReady()) {
				if (m_shader.BindShader())
					success = true;
			}

			if (!success) {
				// SelectSpoutPanel("Shader bind error");
				return false;
			}
			else {
				// Set uniform locations to -1 so that they are only used if necessary
				m_timeLocation				 = -1;
				m_channeltimeLocation		 = -1;
				m_mouseLocation				 = -1;
				m_mouseLocationVec4			 = -1;
				m_dateLocation				 = -1;
				m_resolutionLocation		 = -1;
				m_channelresolutionLocation  = -1;
				m_inputTextureLocation		 = -1;
				m_inputTextureLocation1		 = -1;
				m_inputTextureLocation2		 = -1;
				m_inputTextureLocation3		 = -1;
				m_screenLocation			 = -1;
				m_surfaceSizeLocation		 = -1;
				m_iParam1Location= -1;
				m_iParam2Location= -1;
				m_iParam3Location= -1;
				m_iParam4Location= -1;
				// m_surfacePositionLocation	= -1; // TODO
				// m_vertexPositionLocation    = -1; // TODO

				// Extras
				// Input colour is linked to the user controls Red, Green, Blue, Alpha
				m_inputColourLocation        = -1;


				// lookup the "location" of each uniform

				//
				// GLSL Sandbox
				//
				// Normalized mouse position. Components of this vector are always between 0.0 and 1.0.
				//	uniform vec2 mouse;
				// Screen (Viewport) resolution.
				//	uniform vec2 resolution;
				// Used for mouse left drag currently
				//	uniform vec2 surfaceSize;
				//  TODO uniform vec2 surfacePosition;

				// Input textures do not appear to be in the GLSL Sandbox spec
				// but are allowed for here

				// From source of index.html on GitHub
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("texture");

				// Preferred names tex0 and tex1 which are commonly used
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("tex0");
				
				if(m_inputTextureLocation1 < 0)
					m_inputTextureLocation1 = m_shader.FindUniform("tex1");

				if(m_iCameraLocation< 0)
					m_iCameraLocation					= m_shader.FindUniform("iCamera");

				if(m_iCameraTargetLocation< 0)
					m_iCameraTargetLocation					= m_shader.FindUniform("iCameraTarget");

				// TODO tex2 and tex3 ?

				// Backbuffer is not supported and is mapped to Texture unit 0
				// From source of index.html on GitHub
				// https://github.com/mrdoob/glsl-sandbox/blob/master/static/index.html
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("backbuffer");

				// From several sources
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("bbuff");
				
				// Time
				if(m_timeLocation < 0)
					m_timeLocation = m_shader.FindUniform("time");
				
				// Param 1
				if(m_iParam1Location< 0)
					m_iParam1Location = m_shader.FindUniform("iParam1");

				// Param 2
				if(m_iParam2Location< 0)
					m_iParam2Location = m_shader.FindUniform("iParam2");
				// Param 3
				if(m_iParam3Location< 0)
					m_iParam3Location = m_shader.FindUniform("iParam3");
				// Param 4
				if(m_iParam4Location< 0)
					m_iParam4Location = m_shader.FindUniform("iParam4");

				// Mouse move
				if(m_mouseLocation < 0)
					m_mouseLocation = m_shader.FindUniform("mouse");

				// Screen size
				if(m_screenLocation < 0) // Vec2
					m_screenLocation = m_shader.FindUniform("resolution"); 

				// Mouse left drag
				if(m_surfaceSizeLocation < 0)
					m_surfaceSizeLocation = m_shader.FindUniform("surfaceSize");
				
				/*
				// TODO
				// surfacePosAttrib is the attribute, surfacePosition is the varying var
				m_surfacePositionLocation = m_shader.FindAttribute("surfacePosAttrib"); 
				if(m_surfacePositionLocation < 0) printf("surfacePosition attribute not found\n");
				if(m_surfacePositionLocation >= 0) {
					// enable the attribute
					m_extensions.glEnableVertexAttribArrayARB(m_surfacePositionLocation);
				}
				m_vertexPositionLocation = m_shader.FindAttribute("position");
				if(m_vertexPositionLocation < 0) printf("vertexPosition attribute not found\n");
				if(m_vertexPositionLocation >= 0) {
					// enable the attribute
					m_extensions.glEnableVertexAttribArrayARB(m_vertexPositionLocation);
				}
				*/

				//
				// Shadertoy
				//

				
				//
				// Texture inputs iChannelx
				//
				if(m_inputTextureLocation < 0)
					m_inputTextureLocation = m_shader.FindUniform("iChannel0");
				
				if(m_inputTextureLocation1 < 0)
					m_inputTextureLocation1 = m_shader.FindUniform("iChannel1");

				if(m_inputTextureLocation2 < 0)
					m_inputTextureLocation2 = m_shader.FindUniform("iChannel2");

				if(m_inputTextureLocation3 < 0)
					m_inputTextureLocation3 = m_shader.FindUniform("iChannel3");

				// iResolution
				if(m_resolutionLocation < 0) // Vec3
					m_resolutionLocation = m_shader.FindUniform("iResolution");

				// iMouse
				if(m_mouseLocationVec4 < 0) // Shadertoy is Vec4
					m_mouseLocationVec4 = m_shader.FindUniform("iMouse");

				// iGlobalTime
				if(m_timeLocation < 0)
					m_timeLocation = m_shader.FindUniform("iGlobalTime");
				 

				// iDate
				if(m_dateLocation < 0)
					m_dateLocation = m_shader.FindUniform("iDate");

				// iChannelTime
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[4]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[0]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[1]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[2]");
				if(m_channeltimeLocation < 0)
					m_channeltimeLocation = m_shader.FindUniform("iChannelTime[3]");

				// iChannelResolution
				if(m_channelresolutionLocation < 0) // Vec3 width, height, depth * 4
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[4]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[0]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[1]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[2]");
				if(m_channelresolutionLocation < 0)
					m_channelresolutionLocation = m_shader.FindUniform("iChannelResolution[3]");

				// inputColour - linked to user input
				if(m_inputColourLocation < 0)
					m_inputColourLocation = m_shader.FindUniform("inputColour");

				m_shader.UnbindShader();

				// Delete the local texture because it might be a different size
				if(m_glTexture0 > 0) glDeleteTextures(1, &m_glTexture0);
				if(m_glTexture1 > 0) glDeleteTextures(1, &m_glTexture1);
				if(m_glTexture2 > 0) glDeleteTextures(1, &m_glTexture2);
				if(m_glTexture3 > 0) glDeleteTextures(1, &m_glTexture3);
				m_glTexture0 = 0;
				m_glTexture1 = 0;
				m_glTexture2 = 0;
				m_glTexture3 = 0;

				// Start the clock again to start from zero
				StartCounter();

				return true;

			} // bind shader OK
		} // compile shader OK
		// =============================================

		return false;
}

void ShaderMaker::StartCounter()
{

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    LARGE_INTEGER li;
	// Find frequency
    QueryPerformanceFrequency(&li);
    PCFreq = double(li.QuadPart)/1000.0;
	// Second call needed
    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
#else
    // posix c++11
    start = std::chrono::steady_clock::now();
#endif

}

double ShaderMaker::GetCounter()
{

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
#else
    // posix c++11
    end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000.;
#endif
    return 0;
}

void ShaderMaker::CreateRectangleTexture(FFGLTextureStruct Texture, FFGLTexCoords maxCoords, GLuint &glTexture, GLenum texunit, GLuint &fbo, GLuint hostFbo)
{
	// First create an fbo and a texture the same size if they don't exist
	if(fbo == 0) {
		m_extensions.glGenFramebuffersEXT(1, &fbo); 
	}

	// The texture ID will be zero if not created yet or if it has been deleted
	// due to size or ID change of the incoming FreeFrame texture
	if(glTexture == 0) {
		glGenTextures(1, &glTexture);
		m_extensions.glActiveTexture(texunit);
		glBindTexture(GL_TEXTURE_2D, glTexture);
		glTexImage2D(GL_TEXTURE_2D, 0,  GL_RGBA, Texture.Width, Texture.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_extensions.glActiveTexture(GL_TEXTURE0);
	} // endif created a new texture
				
	// Render the incoming texture to the local one via the fbo
	m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fbo);
	m_extensions.glFramebufferTexture2DEXT(GL_READ_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, glTexture, 0);
	glBindTexture(GL_TEXTURE_2D, Texture.Handle);
				
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	//
	// Must refer to maxCoords here because the texture
	// could be smaller than the hardware size containing it
	//
	//lower left
	glTexCoord2f(0.0, 0.0);
	glVertex2f(-1.0, -1.0);
	//upper left
	glTexCoord2f(0.0, (float)maxCoords.t);
	glVertex2f(-1.0, 1.0);
	// upper right
	glTexCoord2f((float)maxCoords.s, (float)maxCoords.t);
	glVertex2f(1.0, 1.0);
	//lower right
	glTexCoord2f((float)maxCoords.s, 0.0);
	glVertex2f(1.0, -1.0);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	// unbind the input texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// unbind the fbo
	if(hostFbo > 0)
		m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, hostFbo);
	else
		m_extensions.glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

}

