#include <FFGL.h>
#include <FFGLLib.h>
#include <math.h>       /* sin */
#include "FFGLHeat.h"
#define PI 3.14159265359
  

#define FFPARAM_CAMERA_X   (0) 
#define FFPARAM_CAMERA_Y   (1) 
#define FFPARAM_CAMERA_Z   (2)  

#define FFPARAM_CAMERA_YAW  (3) 
#define FFPARAM_CAMERA_PITCH   (4) 
#define FFPARAM_CAMERA_TAGRET_DIST   (5)  

#define FFPARAM_SPEED_X   (6) 
#define FFPARAM_SPEED_Y   (7)  


#define FFPARAM2_X  (8)  
#define FFPARAM2_Y   (9)  
#define FFPARAM2_Z   (10)  
#define FFPARAM2_W   (11)  


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo ( 
	FFGLSineWave2D::CreateInstance,	// Create method
//   .... <- max length
	"S026",								// Plugin unique ID											
//   ................ <- max length
	"sOm-CityScape",					// Plugin name											
	1,						   			// API major version number 													
	000,								  // API minor version number	
	1,										// Plugin major version number
	000,									// Plugin minor version number
	FF_SOURCE,						// Plugin type
	"sOm-CityScape",	// Plugin description
	"(c) by VJSpack-O-Mat aka Christian Kleinhuis - CityScape . " // About
);

char *vertexShaderCode =
"void main()"
"{"
"  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
"  gl_TexCoord[1] = gl_MultiTexCoord1;"
"  gl_FrontColor = gl_Color;"
"}";


char *fragmentShaderCode =
"float PI=3.14159265359;"
""  
"uniform vec3      iResolution;          "
"uniform float     iGlobalTime;          "   
"uniform vec4     iParam2;"

"" 
"vec3 camTarget;"
"vec3 camPos;"
"uniform vec3 camPosIn; "
""
""
"uniform vec3 targetYawPitchRoll;"
"uniform float targetDistance;"
"float houseHeight=1;"
""
"float I_MAX= 75;"
"float rand(vec2 n) {"
"  return fract(sin((n.x*1e2+n.y*1e4+1475.4526)*1e-4)*1e6);"
"}"
""
"float noise(vec2 p)"
"{"
"    p = floor(p*200.0);"
"return rand(p);"
"}"
"vec3 polygonXY(float z,vec2 vert1, vec2 vert2, vec3 camPos,vec3 rayDir){"
"    float t = -(camPos.z-z)/rayDir.z;"
"    vec2 cross = camPos.xy + rayDir.xy*t;"
"    if (cross.x>min(vert1.x,vert2.x) && "
"        cross.x<max(vert1.x,vert2.x) &&"
"       cross.y>min(vert1.y,vert2.y) &&"
"       cross.y<max(vert1.y,vert2.y) &&"
"       dot(rayDir,vec3(cross,z)-camPos)>0.0){"
"        float dist = length(camPos-vec3(cross,z));"
"            return vec3(dist, cross.x-min(vert1.x,vert2.x),cross.y-min(vert1.y,vert2.y));"
"        }"
"    "
"    return vec3(101.0,0.0,0.0);"
"}"
"vec3 polygonYZ(float x,vec2 vert1, vec2 vert2, vec3 camPos,vec3 rayDir){"
"    float t = -(camPos.x-x)/rayDir.x;"
"    vec2 cross1 = camPos.yz + rayDir.yz*t;"
"    if (cross1.x>min(vert1.x,vert2.x) && "
"        cross1.x<max(vert1.x,vert2.x) &&"
"       cross1.y>min(vert1.y,vert2.y) &&"
"       cross1.y<max(vert1.y,vert2.y)&&"
"       dot(rayDir,vec3(x,cross1)-camPos)>0.0){"
"        float dist = length(camPos-vec3(x,cross1));"
"            return vec3(dist, cross1.x-min(vert1.x,vert2.x),cross1.y-min(vert1.y,vert2.y));"
"        }"
"    "
"    return vec3(101.0,0.0,0.0);"
"}"
"vec3 polygonXZ(float y,vec2 vert1, vec2 vert2, vec3 camPos,vec3 rayDir){"
"    float t = -(camPos.y-y)/rayDir.y;"
"    vec2 cross1 = camPos.xz + rayDir.xz*t;"
"    if (cross1.x>min(vert1.x,vert2.x) && "
"        cross1.x<max(vert1.x,vert2.x) &&"
"       cross1.y>min(vert1.y,vert2.y) &&"
"       cross1.y<max(vert1.y,vert2.y)&&"
"       dot(rayDir,vec3(cross1.x,y,cross1.y)-camPos)>0.0){"
"        float dist = length(camPos-vec3(cross1.x,y,cross1.y));"
"            return vec3(dist, cross1.x-min(vert1.x,vert2.x),cross1.y-min(vert1.y,vert2.y));"
"        }"
"    "
"    return vec3(101.0,0.0,0.0);"
"}"
""
"vec3 textureWall(vec2 pos, vec2 maxPos, vec2 squarer,float s,float height,float dist,vec3 rayDir,vec3 norm){"

"    float randB = rand(squarer*2.0);"
"    vec3 windowColor =(-0.4+randB*0.8)*vec3(0.3,0.3,0.0)+(-0.4+fract(randB*10.0)*0.8)*vec3(0.0,0.0,0.3)+(-0.4+fract(randB*10000.0)*0.8)*vec3(0.3,0.0,0.0);"
"    float floorFactor = 1.0;"
"    vec2 windowSize = vec2(0.65,0.35);"
"    vec3 wallColor = s*(0.3+1.4*fract(randB*100.0))*vec3(0.1,0.1,0.1)+(-0.7+1.4*fract(randB*1000.0))*vec3(0.02,0.,0.);"
"wallColor*=1.3;"
"    "
"    vec3 color = vec3(0.0);"
"    vec3 conturColor = wallColor/1.5;"
"    if (height<0.51){"
"    windowColor += vec3(0.3,0.3,0.0);"
"        windowSize = vec2(0.4,0.4);"
"        floorFactor = 0.0;"
""
"    }"
"    if (height<0.6){floorFactor = 0.0;}"
"    if (height>0.75){"
"    windowColor += vec3(0.0,0.0,0.3);"
"    }"
"    windowColor*=1.5;"
"    float wsize = 0.02;"
"    wsize+=-0.007+0.014*fract(randB*75389.9365);"
"    windowSize+= vec2(0.34*fract(randB*45696.9365),0.50*fract(randB*853993.5783));"
"    "
"    vec2 contur=vec2(0.0)+(fract(maxPos/2.0/wsize))*wsize;"
"    if (contur.x<wsize){contur.x+=wsize;}"
"    if (contur.y<wsize){contur.y+=wsize;}"
"    "
"vec2 winPos = (pos-contur)/wsize/2.0-floor((pos-contur)/wsize/2.0);"
"    "
"    float numWin = floor((maxPos-contur)/wsize/2.0).x;"
"    "
"    if ( (maxPos.x>0.5&&maxPos.x<0.6) && ( ((pos-contur).x>wsize*2.0*floor(numWin/2.0)) && ((pos-contur).x<wsize*2.0+wsize*2.0*floor(numWin/2.0)) )){"
"        return (0.9+0.2*noise(pos))*conturColor;"
"    }"
"    "
"    if ( (maxPos.x>0.6&&maxPos.x<0.7) &&( ( ((pos-contur).x>wsize*2.0*floor(numWin/3.0)) && ((pos-contur).x<wsize*2.0+wsize*2.0*floor(numWin/3.0)) )||"
"                                          ( ((pos-contur).x>wsize*2.0*floor(numWin*2.0/3.0)) && ((pos-contur).x<wsize*2.0+wsize*2.0*floor(numWin*2.0/3.0)) )) ){"
"        return (0.9+0.2*noise(pos))*conturColor;"
"    }"
"    "
"    if ( (maxPos.x>0.7) &&( ( ((pos-contur).x>wsize*2.0*floor(numWin/4.0)) && ((pos-contur).x<wsize*2.0+wsize*2.0*floor(numWin/4.0)) )||"
"                                          ( ((pos-contur).x>wsize*2.0*floor(numWin*2.0/4.0)) && ((pos-contur).x<wsize*2.0+wsize*2.0*floor(numWin*2.0/4.0)) )||"
"                                          ( ((pos-contur).x>wsize*2.0*floor(numWin*3.0/4.0)) && ((pos-contur).x<wsize*2.0+wsize*2.0*floor(numWin*3.0/4.0)) )) ){"
"        return (0.9+0.2*noise(pos))*conturColor;"
"    }"
"    if ((maxPos.x-pos.x<contur.x)||(maxPos.y-pos.y<contur.y+2.0*wsize)||(pos.x<contur.x)||(pos.y<contur.y)){"
"            return (0.9+0.2*noise(pos))*conturColor;"
"        "
"    }"
"    if (maxPos.x<0.14) {"
"        return (0.9+0.2*noise(pos))*wallColor;"
"    }"
"    vec2 window = floor((pos-contur)/wsize/2.0);"
"    float random = rand(squarer*s*maxPos.y+window);"
"    float randomZ = rand(squarer*s*maxPos.y+floor(vec2((pos-contur).y,(pos-contur).y)/wsize/2.0));"
"    float windows = floorFactor*sin(randomZ*5342.475379+(fract(975.568*randomZ)*0.15+0.05)*window.x);"
"    "
" int index=int(round(abs(fract(10.0*random))*4.0));"
"float blH = 0.06*dist*600.0/iResolution.x/abs(dot(normalize(rayDir.xy),normalize(norm.xy)));"
"    float blV = 0.06*dist*600.0/iResolution.x/sqrt(abs(1.0-pow(abs(rayDir.z),2.0)));"
"    "
"windowColor +=vec3(1.0,1.0,1.0);"
"    windowColor *= smoothstep(0.5-windowSize.x/2.0-blH,0.5-windowSize.x/2.0+blH,winPos.x);"
"   windowColor *= smoothstep(0.5+windowSize.x/2.0+blH,0.5+windowSize.x/2.0-blH,winPos.x);"
"   windowColor *= smoothstep(0.5-windowSize.y/2.0-blV,0.5-windowSize.y/2.0+blV,winPos.y);"
"   windowColor *= smoothstep(0.5+windowSize.y/2.0+blV,0.5+windowSize.y/2.0-blV,winPos.y);"
"    "
"    if ((random <0.05*(3.5-2.5*floorFactor))||(windows>0.65)){"
"        if (winPos.y<0.5) {windowColor*=(1.0-0.4*fract(random*100.0));}"
"        if ((winPos.y>0.5)&&(winPos.x<0.5)) {windowColor*=(1.0-0.4*fract(random*10.0));}"
"            return (0.9+0.2*noise(pos))*wallColor+(0.9+0.2*noise(pos))*windowColor*iParam2[index];"
""
""
"    } "
"    else{"

"        windowColor*=0.08*fract(10.0*random);"
"    }"
"    "
"    return (0.9+0.2*noise(pos))*wallColor+windowColor*iParam2[index];"
""
"}"
""
"vec3 textureRoof(vec2 pos, vec2 maxPos,vec2 squarer){"
"    float wsize = 0.025;"
"    float randB = rand(squarer*2.0);"
"    vec3 wallColor = (0.3+1.4*fract(randB*100.0))*vec3(0.1,0.1,0.1)+(-0.7+1.4*fract(randB*1000.0))*vec3(0.02,0.,0.);"
"vec3 conturColor = wallColor*1.5/2.5;"
"    vec2 contur = vec2(0.02);"
"    if ((maxPos.x-pos.x<contur.x)||(maxPos.y-pos.y<contur.y)||(pos.x<contur.x)||(pos.y<contur.y)){"
"            return (0.9+0.2*noise(pos))*conturColor;"
"        "
"    }"
"    float step1 = 0.06+0.12*fract(randB*562526.2865);"
"    pos -=step1;"
"    maxPos -=step1*2.0;"
"    if ((pos.x>0.0&&pos.y>0.0&&pos.x<maxPos.x&&pos.y<maxPos.y)&&((abs(maxPos.x-pos.x)<contur.x)||(abs(maxPos.y-pos.y)<contur.y)||(abs(pos.x)<contur.x)||(abs(pos.y)<contur.y))){"
"            return (0.9+0.2*noise(pos))*conturColor;"
"        "
"    }"
"    pos -=step1;"
"    maxPos -=step1*2.0;"
"    if ((pos.x>0.0&&pos.y>0.0&&pos.x<maxPos.x&&pos.y<maxPos.y)&&((abs(maxPos.x-pos.x)<contur.x)||(abs(maxPos.y-pos.y)<contur.y)||(abs(pos.x)<contur.x)||(abs(pos.y)<contur.y))){"
"            return (0.9+0.2*noise(pos))*conturColor;"
"        "
"    }"
"    pos -=step1;"
"    maxPos -=step1*2.0;"
"    if ((pos.x>0.0&&pos.y>0.0&&pos.x<maxPos.x&&pos.y<maxPos.y)&&((abs(maxPos.x-pos.x)<contur.x)||(abs(maxPos.y-pos.y)<contur.y)||(abs(pos.x)<contur.x)||(abs(pos.y)<contur.y))){"
"            return (0.9+0.2*noise(pos))*conturColor;"
"        "
"    }"
"    "
"    return (0.9+0.2*noise(pos))*wallColor;"
"    "
"}"
"vec3 textureGround(vec2 squarer, vec2 pos,vec2 vert1,vec2 vert2,float dist){"
"    vec3 color = (0.9+0.2*noise(pos))*vec3(0.1,0.15,0.1);"
"    float randB = rand(squarer*2.0);"
""
"    vec3 wallColor = (0.3+1.4*fract(randB*100.0))*vec3(0.1,0.1,0.1)+(-0.7+1.4*fract(randB*1000.0))*vec3(0.02,0.,0.);"
"float fund = 0.03;"
"    float bl = 0.01;"
"    float f = smoothstep(vert1.x-fund-bl,vert1.x-fund,pos.x);"
"    f *= smoothstep(vert1.y-fund-bl,vert1.y-fund,pos.y);"
"    f *= smoothstep(vert2.y+fund+bl,vert2.y+fund,pos.y);"
"    f *= smoothstep(vert2.x+fund+bl,vert2.x+fund,pos.x);"
""
"    pos -= 0.0;"
"    vec2 maxPos = vec2(1.,1.);"
"    vec2 contur = vec2(0.06,0.06);"
"    if ((pos.x>0.0&&pos.y>0.0&&pos.x<maxPos.x&&pos.y<maxPos.y)&&((abs(maxPos.x-pos.x)<contur.x)||(abs(maxPos.y-pos.y)<contur.y)||(abs(pos.x)<contur.x)||(abs(pos.y)<contur.y))){"
"            color =  vec3(0.1,0.1,0.1)*(0.9+0.2*noise(pos));"
"        "
"    }"
"    pos -= 0.06;"
"    maxPos = vec2(.88,0.88);"
"    contur = vec2(0.01,0.01);"
"    if ((pos.x>0.0&&pos.y>0.0&&pos.x<maxPos.x&&pos.y<maxPos.y)&&((abs(maxPos.x-pos.x)<contur.x)||(abs(maxPos.y-pos.y)<contur.y)||(abs(pos.x)<contur.x)||(abs(pos.y)<contur.y))){"
"            color =  vec3(0.,0.,0.);"
"        "
"    }"
"    color = mix(color,(0.9+0.2*noise(pos))*wallColor*1.5/2.5,f);"
""
"    pos+=0.06;"
"    "
""
"    "
"    return color;"
"}"
""
""
"void main( void ) {"

"vec2 uv = gl_FragCoord.xy/iResolution.xy ;"
"    uv = uv * 2.0 - 1.0;"
"    uv.x *= iResolution.x / iResolution.y;    "
"    vec2 pos = uv ;"
"    float t = -iGlobalTime;"
"    float tt = -iGlobalTime-0.5;"
"    "
"    "
"    vec3  targetYawPitchRollPI=targetYawPitchRoll;"
"    float targetx=sin(targetYawPitchRollPI.y)*cos(targetYawPitchRollPI.x)*targetDistance;"
"    float targety=sin(targetYawPitchRollPI.y)*sin(targetYawPitchRollPI.x)*targetDistance;"
"    float targetz=cos(targetYawPitchRollPI.y)*targetDistance;"
"camTarget=camPosIn;"
""
"camPos=vec3(targetx,targety,targetz)+camPosIn;"
"    vec3 camDir = normalize(camTarget-camPos);"
"    vec3 camUp  = normalize(vec3(0.0, 0.0, -1.0));"
"    vec3 camSide = cross(camDir, camUp);"
"    camUp  = cross(camDir, camSide);"
"    vec3 rayDir = normalize(camSide*pos.x + camUp*pos.y + camDir*1.6);"
"    float angle = 0.03*pow(abs(acos(rayDir.x)),4.0);" 
"    vec3 color = vec3(0.0);"
"    vec2 square = floor(camPos.xy);"
"    square.x += 0.5-0.5*sign(rayDir.x);"
"    square.y += 0.5-0.5*sign(rayDir.y);"
"    float mind = 100.0;"
"    int k = 0;"
"    vec3 pol;"
"    vec2 maxPos;"
"    vec2 crossG;"
"    float tSky = -(camPos.z-3.9)/rayDir.z;"
"    vec2 crossSky = floor(camPos.xy + rayDir.xy*tSky);"
"float alpha=1;"
"    for (int i=1; i<I_MAX; i++){"
"                "
"        vec2 squarer = square-vec2(0.5,0.5)+0.5*sign(rayDir.xy);"
"        if (crossSky == squarer&&crossSky!=floor(camPos.xy))"
"        {"
"        color += vec3(vec2(0.5,0.15)*abs(angle)*exp(-rayDir.z*rayDir.z*30.0),0.2);"
" alpha=0;"
"            break;"
""
"        }"
"        float t;"
"        float random = rand(squarer);"
"        float height = 0.0;"
"        float quartalR = rand(floor(squarer/10.0));"
"        if (floor(squarer/10.0) == vec2(0.0,0.0)) quartalR = 0.399;"
"        if (quartalR<0.4) {"
"            height = -0.15+0.4*random+smoothstep(12.0,7.0,length(fract(squarer/10.0)*10.0-vec2(5.0,5.0)))*0.8*random+0.9*smoothstep(10.0,0.0,length(fract(squarer/10.0)*10.0-vec2(5.0,5.0)));"
"        height*=quartalR/0.4;"
""
"        }"
"        float maxJ=2.0;"
"        float roof = 1.0;"
"        if (height<0.3){"
"            height = 0.3*(0.7+1.8*fract(random*100.543264));maxJ = 2.0;"
"            if (fract(height*1000.0)<0.04) height*=1.3;"
"        }"
"        if (height>0.5) {maxJ=3.0;}"
"        if (height>0.85){maxJ = 4.0;}"
"        if (fract(height*100.0)<0.15){height = pow(maxJ-1.0,0.3)*height; maxJ = 2.0; roof = 0.0;}"
"height*=houseHeight;"
"        "
"        float maxheight = 1.5*pow((maxJ-1.0),0.3)*height+roof*0.07;"
"        if (camPos.z+rayDir.z*(length(camPos.xy - square) +0.71 - sign(rayDir.z)*0.71)/length(rayDir.xy)<maxheight){"
"vec2 vert1r;"
"        vec2 vert2r;"
"            float zz = 0.0;"
"            float prevZZ = 0.0;"
"            for(int nf=1;nf<8;nf++){"
"                float j = float(nf);"
"           if(j>maxJ){break;}"
"                prevZZ = zz;"
"           zz = 1.5*pow(j,0.3)*height;"
"               "
""
"        float dia = 1.0/pow(j,0.3);"
"            if(j==maxJ){"
"                    if (roof == 0.0) {break;}"
"            zz = 1.5*pow((j-1.0),0.3)*height+0.03+0.04*fract(random*1535.347);"
"                dia = 1.0/pow((j-1.0),0.3)-0.2-0.2*fract(random*10000.0);"
"           }"
"            "
"            vec2 v1 = vec2(0.0); "
"            vec2 v2 = vec2(0.0);"
"                float randomF = fract(random*10.0);"
"                if (randomF<0.25){ v1 = vec2(fract(random*1000.0),fract(random*100.0));}"
"                if (randomF>0.25&&randomF<0.5){ v1 = vec2(fract(random*100.0),0.0);v2 = vec2(0.0,fract(random*1000.0));}"
"                if (randomF>0.5&&randomF<0.75){ v2 = vec2(fract(random*1000.0),fract(random*100.0));}"
"                if (randomF>0.75){ v1 = vec2(0.0,fract(random*1000.0)); v2 = vec2(fract(random*100.0),0.0);}"
"            if (rayDir.y<0.0){"
"                float y = v1.y;"
"                v1.y = v2.y;"
"                v2.y = y;"
"            }"
"            if (rayDir.x<0.0){"
"                float x = v1.x;"
"                v1.x = v2.x;"
"                v2.x = x;"
"            }"
"            "
"        vec2 vert1 = square+sign(rayDir.xy)*(0.5-0.37*(dia*1.0-1.0*v1));"
"        vec2 vert2 = square+sign(rayDir.xy)*(0.5+0.37*(dia*1.0-1.0*v2));"
"                if (j==1.0){ "
"                    vert1r = vec2(min(vert1.x, vert2.x),min(vert1.y,vert2.y));"
"                    vert2r = vec2(max(vert1.x, vert2.x),max(vert1.y,vert2.y));"
"                }"
"            "
"        vec3 pxy = polygonXY(zz,vert1,vert2,camPos,rayDir);"
"            if (pxy.x<mind){mind = pxy.x; pol = pxy; k=1;maxPos = vec2(abs(vert1.x-vert2.x),abs(vert1.y-vert2.y));}"
"            "
"        vec3 pyz = polygonYZ(vert1.x,vec2(vert1.y,prevZZ),vec2(vert2.y,zz),camPos,rayDir);"
"            if (pyz.x<mind){mind = pyz.x; pol = pyz; k=2;maxPos = vec2(abs(vert1.y-vert2.y),zz-prevZZ);}"
""
"        vec3 pxz = polygonXZ(vert1.y,vec2(vert1.x,prevZZ),vec2(vert2.x,zz),camPos,rayDir);"
"            if (pxz.x<mind){mind = pxz.x; pol = pxz; k=3;maxPos = vec2(abs(vert1.x-vert2.x),zz-prevZZ);}"
"               "
""
"        }"
"            "
"        if ((mind<100.0)&&(k==1)){"
"            color += textureRoof(vec2(pol.y,pol.z),maxPos,squarer);"
"                if (mind>3.0){color*=sqrt(3.0/mind);}"
""
"            break;"
"        } "
"        if ((mind<100.0)&&(k==2)){"
"            color += textureWall(vec2(pol.y,pol.z),maxPos,squarer,1.2075624928,height,mind,rayDir,vec3(1.0,0.0,0.0));"
"            if (mind>3.0){color*=sqrt(3.0/mind);}"
"            break;"
"        } "
"        "
"        if ((mind<100.0)&&(k==3)){"
"            color += textureWall(vec2(pol.y,pol.z),maxPos,squarer,0.8093856205,height,mind,rayDir,vec3(0.0,1.0,0.0));"
"            if (mind>3.0){color*=sqrt(3.0/mind);}"
""
"            break;"
"        }"
"        t = -camPos.z/rayDir.z;"
"    crossG = camPos.xy + rayDir.xy*t;"
"        if (floor(crossG) == squarer)"
"        {"
"            mind = length(vec3(crossG,0.0)-camPos);"
"            color += textureGround(squarer,fract(crossG),fract(vert1r),fract(vert2r),mind);"
"            if (mind>3.0){color*=sqrt(3.0/mind);}"
""
"            break;"
"        }"
"        "
"        } "
"        "
"            "
"        if ((square.x+sign(rayDir.x)-camPos.x)/rayDir.x<(square.y+sign(rayDir.y)-camPos.y)/rayDir.y) {"
"            square.x += sign(rayDir.x)*1.0;"
"        } else {"
"            square.y += sign(rayDir.y)*1.0;"
"        }"
"        "
"        if(i==I_MAX-1&&rayDir.z>-0.1) {"
"color += vec3(vec2(0.5,0.15)*abs(angle)*exp(-rayDir.z*rayDir.z*30.0),0.2);"
"alpha=0;"
"}"
""
"    }"
"    gl_FragColor = vec4(color,alpha);"
"}"
"" ;

static const unsigned char g_heatTextureData[] = {0,0,255,0,3,253,0,5,251,0,7,248,0,9,245,0,12,243,0,16,240,0,18,237,0,21,234,0,25,230,0,28,227,0,32,224,0,36,219,0,40,215,0,43,211,0,47,207,0,52,203,0,56,199,0,60,195,0,65,190,0,70,186,0,74,181,0,79,176,0,83,172,0,88,167,0,93,162,0,97,157,0,103,152,0,108,147,0,113,142,0,117,137,0,122,132,0,127,128,0,132,122,0,138,118,0,143,113,0,148,107,0,152,103,0,158,98,0,163,93,0,167,88,0,171,84,0,176,79,0,181,74,0,185,69,0,190,65,0,195,60,0,198,56,0,203,52,0,207,47,0,211,44,0,216,39,0,220,36,0,223,32,0,227,28,0,230,25,0,234,21,0,236,18,0,240,15,0,243,12,0,246,9,0,249,6,0,251,5,0,253,2,0,255,0,2,255,0,4,255,0,7,255,0,9,255,0,12,255,0,15,255,0,18,255,0,21,255,0,25,255,0,28,255,0,32,255,0,36,255,0,40,255,0,44,255,0,48,255,0,52,255,0,56,255,0,61,255,0,65,255,0,70,255,0,74,255,0,79,255,0,83,255,0,88,255,0,93,255,0,98,255,0,103,255,0,108,255,0,113,255,0,117,255,0,123,255,0,127,255,0,133,255,0,138,255,0,142,255,0,147,255,0,152,255,0,158,255,0,162,255,0,166,255,0,171,255,0,177,255,0,181,255,0,186,255,0,190,255,0,194,255,0,199,255,0,204,255,0,207,255,0,211,255,0,216,255,0,219,255,0,224,255,0,226,255,0,230,255,0,233,255,0,237,255,0,240,255,0,243,255,0,245,255,0,248,255,0,250,255,0,253,255,0,255,255,0,255,254,0,255,253,0,255,252,0,255,251,0,255,250,0,255,248,0,255,246,0,255,245,0,255,244,0,255,242,0,255,240,0,255,239,0,255,236,0,255,235,0,255,233,0,255,231,0,255,229,0,255,227,0,255,225,0,255,223,0,255,221,0,255,219,0,255,216,0,255,214,0,255,212,0,255,210,0,255,208,0,255,205,0,255,203,0,255,201,0,255,198,0,255,196,0,255,194,0,255,192,0,255,189,0,255,187,0,255,185,0,255,182,0,255,180,0,255,177,0,255,175,0,255,173,0,255,171,0,255,169,0,255,167,0,255,164,0,255,162,0,255,160,0,255,158,0,255,156,0,255,154,0,255,152,0,255,150,0,255,148,0,255,146,0,255,144,0,255,143,0,255,141,0,255,139,0,255,138,0,255,136,0,255,135,0,255,134,0,255,133,0,255,132,0,255,130,0,255,129,0,255,128,0,255,126,0,255,126,0,255,125,0,255,123,0,255,121,0,255,120,0,255,118,0,255,116,0,255,115,0,255,112,0,255,111,0,255,109,0,255,107,0,255,104,0,255,102,0,255,100,0,255,97,0,255,95,0,255,93,0,255,90,0,255,87,0,255,85,0,255,82,0,255,80,0,255,78,0,255,75,0,255,72,0,255,69,0,255,67,0,255,64,0,255,61,0,255,59,0,255,56,0,255,53,0,255,51,0,255,48,0,255,45,0,255,43,0,255,41,0,255,38,0,255,36,0,255,33,0,255,31,0,255,28,0,255,26,0,255,24,0,255,22,0,255,19,0,255,17,0,255,16,0,255,13,0,255,12,0,255,10,0,255,9,0,255,6,0,255,5,0,255,3,0,255,2,0,255,1,0};
static const int g_heatTextureWidth = 256;

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLSineWave2D::FFGLSineWave2D()
:CFreeFrameGLPlugin(),
 m_initResources(1),
 m_heatTextureId(0),
 m_inputTextureLocation(-1), 
 m_maxCoordsLocation(-1)
{
	// Input properties
	SetMinInputs(1);
	SetMaxInputs(1);
	SetTimeSupported(false);
	// Parameters
	// Param swizzle, somehow BPM is not accepted ... right now .. ;) 
	SetParamInfo(FFPARAM_CAMERA_X, "Camera X", FF_TYPE_STANDARD, 0.51f);   
	SetParamInfo(FFPARAM_CAMERA_Y, "Camera Y", FF_TYPE_STANDARD, 0.61f);   
	SetParamInfo(FFPARAM_CAMERA_Z, "Camera Z", FF_TYPE_STANDARD, 0.61f);   

		SetParamInfo(FFPARAM_CAMERA_YAW, "Yaw", FF_TYPE_STANDARD, 0.25f);   
		SetParamInfo(FFPARAM_CAMERA_PITCH, "Pitch", FF_TYPE_STANDARD, 0.25f);   
		SetParamInfo(FFPARAM_CAMERA_TAGRET_DIST, "Distance Tárget", FF_TYPE_STANDARD, 0.5f);   

	 
		SetParamInfo(FFPARAM_SPEED_X, "Speed X", FF_TYPE_STANDARD, 0.51f);   
	SetParamInfo(FFPARAM_SPEED_Y, "Speed Y", FF_TYPE_STANDARD, 0.61f);   
		 
	SetParamInfo(FFPARAM2_X, "Light 1", FF_TYPE_STANDARD, 0.0f);   
	SetParamInfo(FFPARAM2_Y, "Light 2", FF_TYPE_STANDARD, 0.0f);   
	SetParamInfo(FFPARAM2_Z, "Light 3", FF_TYPE_STANDARD, 0.0f);   
	SetParamInfo(FFPARAM2_W, "Light 4", FF_TYPE_STANDARD, 0.0f);   
		 
cameraRotation.x=0;
cameraRotation.y=0;
cameraRotation.z=0;
  

positionAnimX.count=0;
positionAnimY.count=0;

	hsvScale.x=1; 
	hsvScale.y=1; 
	hsvScale.z=1; 

	 
	center.x=.25;
center.y=0.5;


	timeCounter=0;  
	GetSystemTime(&m_SystemTimeLastFrame); 
	// Initialise System Time Based Time counting	 
    GetSystemTime(&m_SystemTimeStart);

	m_Time=0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD FFGLSineWave2D::InitGL(const FFGLViewportStruct *vp)
{
  //initialize gl extensions and
  //make sure required features are supported
  m_extensions.Initialize();
  if (m_extensions.multitexture==0 || m_extensions.ARB_shader_objects==0)
    return FF_FAIL; 
  //initialize gl shader
  m_shader.SetExtensions(&m_extensions);
  m_shader.Compile(vertexShaderCode,fragmentShaderCode);
 
  //activate our shader
  m_shader.BindShader();
    
  //to assign values to parameters in the shader, we have to lookup
  //the "location" of each value.. then call one of the glUniform* methods
  //to assign a value
  m_inputTextureLocation = m_shader.FindUniform("iChannel0"); 
  m_resolutionLocation = m_shader.FindUniform("iResolution"); 
  m_maxCoordsLocation = m_shader.FindUniform("maxCoords"); 
  m_timeLocation = m_shader.FindUniform("iGlobalTime"); 
  m_aspectLocation= m_shader.FindUniform("aspectRatio");
  
   m_resolutionLocation=m_shader.FindUniform("iResolution");
  //the 0 means that the 'heatTexture' in
  //the shader will use the texture bound to GL texture unit 0 
  m_hsvScaleLocation=m_shader.FindUniform("hsvScale"); 
    m_distanceLocation=m_shader.FindUniform("targetDistance"); 
    m_yawpitchLocation=m_shader.FindUniform("targetYawPitchRoll"); 
    m_camposLocation=m_shader.FindUniform("camPosIn"); 
    
	m_param2Location=m_shader.FindUniform("iParam2"); 
     
    
  m_shader.UnbindShader();

  //create/upload the heat texture on texture unit 0
  //(which should already be the active unit)
  glGenTextures( 1, &m_heatTextureId );
  glBindTexture(GL_TEXTURE_1D, m_heatTextureId);
    
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    


  glTexImage1D(
    GL_TEXTURE_1D,
    0, //0 means the base mipmap level
    3, //# of color components in the texture
    g_heatTextureWidth, //width
    0, //no border,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    g_heatTextureData);  

  return FF_SUCCESS;
}

DWORD FFGLSineWave2D::DeInitGL()
{
  m_shader.FreeGLResources();

  if (m_heatTextureId)
  {
    glDeleteTextures(1, &m_heatTextureId);
    m_heatTextureId = 0;
  } 
  return FF_SUCCESS;
}

DWORD  FFGLSineWave2D::SetTime(double time) {
	m_Time=time;
	return FF_SUCCESS; 
}


double FFGLSineWave2D::systemTimeToDouble(SYSTEMTIME *time){
	return  ((double)time->wMinute*60000+ (double)time->wSecond*1000+(double)time->wMilliseconds)/(double)1000.0f;
}

double FFGLSineWave2D::GetTimeInSecondsSinceStart(){
	 // Update Time Based on Systemtime and initial stored system time...
   SYSTEMTIME st;    
   GetSystemTime(&st); 

   double delta=systemTimeToDouble(&st)-systemTimeToDouble(&m_SystemTimeLastFrame);

   // increase time counter;
   timeCounter+=delta;

   // increase sin values  
   positionAnimX.count+=delta*(positionAnimX.speed*20.0-10.0);
   positionAnimY.count+=delta*(positionAnimY.speed*20.0-10.0);
   
   m_SystemTimeLastFrame=st;

//   return systemTimeToDouble(&st);
   return timeCounter;

}

DWORD FFGLSineWave2D::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{
  if (pGL->numInputTextures<1) return FF_FAIL;

  if (pGL->inputTextures[0]==NULL) return FF_FAIL; 
  //activate our shader
  m_shader.BindShader();
    
  //bind the heat texture to texture unit 0
  glBindTexture(GL_TEXTURE_1D, m_heatTextureId);

	FFGLTextureStruct &Texture = *(pGL->inputTextures[0]);

	//get the max s,t that correspond to the 
	//width,height of the used portion of the allocated texture space
	FFGLTexCoords maxCoords = GetMaxGLTexCoords(Texture);

  //assign the maxCoords value in the shader
  //(glUniform2f assigns to a vec2)
  m_extensions.glUniform2fARB(m_maxCoordsLocation, maxCoords.s, maxCoords.t);
  
  
  
   
   
   
  //assign the heatAmount
 // m_extensions.glUniform1fARB(m_heatAmountLocation, 2.0*m_Heat);
  //assign the time
  m_extensions.glUniform1fARB(m_timeLocation,(float)GetTimeInSecondsSinceStart());  
  m_extensions.glUniform1fARB(m_aspectLocation,((float)pGL->inputTextures[0]->Height/(float)pGL->inputTextures[0]->Width));
  m_extensions.glUniform3fARB(m_resolutionLocation,pGL->inputTextures[0]->HardwareWidth,pGL->inputTextures[0]->HardwareHeight,0 );  
  m_extensions.glUniform1fARB(m_distanceLocation,targetDistance); 
  m_extensions.glUniform4fARB(m_param2Location,param2x,param2y,param2z,param2w); 
  m_extensions.glUniform3fARB(m_camposLocation,positionAnimX.count+(camerapos.x*2.0-1.0)*10.0,positionAnimY.count+(camerapos.y*2.0-1.0)*10.0,(camerapos.z*2.0-1.0)*10.0); 
  m_extensions.glUniform3fARB(m_hsvScaleLocation,hsvScale.x,hsvScale.y,hsvScale.z); 
  m_extensions.glUniform3fARB(m_yawpitchLocation,yawpitchroll.x*PI*2,yawpitchroll.y*PI*2,0); 
  glClearColor(0,0,0,0);

  //activate texture unit 1 and bind the input texture
  m_extensions.glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, Texture.Handle);
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
  //draw the quad that will be painted by the shader/textures
  //note that we are sending texture coordinates to texture unit 1..
  //the vertex shader and fragment shader refer to this when querying for
  //texture coordinates of the inputTexture
	glBegin(GL_QUADS);

	//lower left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0,0);
	glVertex2f(-1,-1);

	//upper left
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, 0, maxCoords.t);
	glVertex2f(-1,1);

	//upper right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, maxCoords.t);
	glVertex2f(1,1);

	//lower right
	m_extensions.glMultiTexCoord2f(GL_TEXTURE1, maxCoords.s, 0);
	glVertex2f(1,-1);
	glEnd();

  //unbind the input texture
  glBindTexture(GL_TEXTURE_2D, 0);
  
  //switch to texture unit 0 and unbind the 1D heat texture
  m_extensions.glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, 0);
  
  //unbind the shader
  m_shader.UnbindShader();
  
  return FF_SUCCESS;
}

DWORD FFGLSineWave2D::GetParameter(DWORD dwIndex)
{
	DWORD dwRet;

	switch (dwIndex) {
		 
  
	default:
		return FF_FAIL;
	}
}

DWORD FFGLSineWave2D::SetParameter(const SetParameterStruct* pParam)
{
	if (pParam != NULL) {
		
		switch (pParam->ParameterNumber) {
   
		case FFPARAM_CAMERA_X:	
			camerapos.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CAMERA_Y:	
			camerapos.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CAMERA_Z:	
			camerapos.z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_SPEED_X:	
			positionAnimX.speed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_SPEED_Y:	
			positionAnimY.speed= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  

			
		case FFPARAM_CAMERA_YAW:	
			yawpitchroll.x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CAMERA_PITCH:	
			yawpitchroll.y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM_CAMERA_TAGRET_DIST:	
			targetDistance= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
		case FFPARAM2_X:	
			param2x= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	  case FFPARAM2_Y:	
			param2y= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	  case FFPARAM2_Z:	
			param2z= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	  case FFPARAM2_W:	
			param2w= *((float *)(unsigned)&(pParam->NewParameterValue)); 
			break;  
	  
	 
	 
	 
		default:
			return FF_FAIL;
		}

		return FF_SUCCESS;
	
	}

	return FF_FAIL;
}
