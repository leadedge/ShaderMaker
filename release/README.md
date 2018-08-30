# Spack O Mat Shader Collection

![one of i-love-chaos logos print your own more than endless possibilities](media/i-love-chaos-white-000000000000003.png )


## Source Plugins

### 001 Mandelbrot   ![Generic badge](https://img.shields.io/badge/HYBRID-ALTERNATE-green.svg) [![Generic badge](https://img.shields.io/badge/3ARY-YES-green.svg)](https://shields.io/)
       
|  |  |
| ------------- | ------------- |
| ![Mandelbrot S01 Exampole](media/001-mandelbrot.png )  | ![Mandelbrot Params](media/001-mandelbrot-params.png ) |
 id [Mandelbrot](https://www.wikiwand.com/en/Mandelbrot_set) with 3 seeds.
           
#### Parameters 
- Julia - switches julia/mandelbrot mode
- CenterX/Y - the zoom center and center of image
-  Zoom  [![Generic badge](https://img.shields.io/badge/EXPONENTIAL-YES-green.svg)](https://shields.io/) - the zoom of the whole image
- Rotation  [![Generic badge](https://img.shields.io/badge/LOOPABLE-YES-green.svg)](https://shields.io/)  - the rotation angle of the image     
- Start of Alternation - iteration when alternation kicks in after this iteration the seeds are alternated                                
- Max Iterations - the maximum number of iterations to use
- Seed 1..3 Real/Imaginary	- the seeds to be used in alternation, the first is pertubation
- Power 1..3 The power used in the mandelbrot equation 0.3125=1, 0.625=2.0, ... stepp is 0.0625 -8..8 
- Color1/2 - the inner and outer colors as RGB
     

   
#### Status 

ID: SM01
Version: 2.0
 

### 002 Mandelbox ![Generic badge](https://img.shields.io/badge/HYBRID-ALTERNATE-green.svg) [![Generic badge](https://img.shields.io/badge/3ARY-YES-green.svg)](https://shields.io/)
|  |  |
| ------------- | ------------- |
| ![Mandelbox Example](media/002-mandelbox.png )  |  ![Mandelbox Params](media/002-mandelbox-params.png )  |
 
 2d alternating Hybrid [Mandelbox](https://www.wikiwand.com/en/Mandelbox) with 3 alternating seeds.
         
#### Parameters 
          
- CenterX/Y - the zoom center and center of image
- [![Generic badge](https://img.shields.io/badge/EXPONENTIAL-YES-green.svg)](https://shields.io/)Zoom - the zoom of the whole image                                                                      
- [![Generic badge](https://img.shields.io/badge/LOOPABLE-YES-green.svg)](https://shields.io/) Rotation - the rotation angle of the image     
- Start of Alternation - iteration when alternation kicks in after this iteration the seeds are alternated 
- Max Iterations - the maximum number of iterations to use
- Seed 1..3 Real/Imaginary	- the seeds to be used in alternation, the first is pertubation
- Color1/2 - the inner and outer colors as RGB
- Mandelbox Scale - determines the mandelbox scale parameter
                
#### Status 

ID: SM02
Version: 2.0

core functionality, perhaps make scale parameter a alternation option

### 003 Sinewave 1D  [![Generic badge](https://img.shields.io/badge/3ARY-YES-green.svg)](https://shields.io/)
       
|  |  |
| ------------- | ------------- |                
| ![Sinewave Example](media/003-sinewave.png )   | ![Sinewave Params](media/003-sinewave-params.png )       |

Renders 3 [Sinewaves](https://www.wikiwand.com/en/Sine_wave) defined by frequency, wavelength and amplitude added together

#### Parameters

- Frequency 1..3 - the speed of the particular sine wave

- Wavelength 1..3 ![Generic badge](https://img.shields.io/badge/EXPONENTIAL-YES-green.svg) - for each wave defines the number of repetitions (exponential grow)

- Amplitude 1..3 - for each sine wave, the amplitude or height of the sine wave

           
#### Status 
         
ID: SM03
Version: 2.0

core functionality, no changes planned, inversions and line mode can be achieved using other visual plugins/options


### 004 Circle Sun      [![Generic badge](https://img.shields.io/badge/3ARY-YES-green.svg)](https://shields.io/)
         
|  |  |
| ------------- | ------------- |       
|![one of i-love-chaos logos print your own more than endless possibilities](media/004-circlesun.png ) |![one of i-love-chaos logos print your own more than endless possibilities](media/004-circlesun-params.png )           |
                               
 - shape - controls the 'roundness', its the exponent for length calculation estimation ([modelling with distance functions ](https://iquilezles.org/www/articles/distfunctions/distfunctions.htm))
 - rotation - the image rotation
 - pos X/Y - image center x/y 
 - Divisions - number of divisions breaking the shape
 - Color 1..3 - the individual color of each part
 - Radius 1..3 - each shapes starting radius from center of image 
 - Width 1..3 - each shapes cover of a full circle rotation
 - Height 1..3 - each shapes height starting from shapes radius
 - Offset 1..3 - each shapes angle offset 
                                
#### Status 

               
ID: SM04
Version: 2.0

core fun functionality, finished no further additions/changes planned 

## Effect Plugins

### 001 God Rays        
|  |  |
| ------------- | ------------- |                      
|![GodRays Example](media/sf-001-godrays.png )  | ![GodRays Params](media/sf-001-godrays-params.png)   |


the godray filter works as any othe filter producing a light beam on areas that are below a grayscale limit


#### Parameters

- Exposure - initial light exposure
- Decay - decaying of light per step
- Density - (confusing param) this is the distance over what area the light beams are calulated from start
- Weight - weight of the light beams through addition
- Bias - the image is converted to a gray scale image and any pixel below that value is considered translucent
- Light X/Y - the 2 dimensional position on the image plane of the image center
- Light Radius - the radius of the light bulb
 
#### Status 

ID: SF01
Version 1.0

nice core functionality, not using a pre render step, might receive a gaussian blur/prefilter step in future, and adjustment for transluency options like alpha or luminance
 
## Shadertoy Conversions

### 001 Kali Sea Creature
                                    
|  |  |
| ------------- | ------------- |   
|![one of i-love-chaos logos print your own more than endless possibilities](media/st-001-kaliseacreature.png )   |
           

### 001 Kali Sea Creature Extended
                                    
|  |  |
| ------------- | ------------- |   
|![one of i-love-chaos logos print your own more than endless possibilities](media/st-001-kaliseacreature.png )   |
           
#### Status 

ID: ST01
Version 1.0

phew, this is a resolume variant of one of the greatest shader writers in the world - and a pretty good friend of mine - kali. He constructs
the most fantastic structures out of the code, the shader used here is famous for its lifelike own animations controlled by the shadertoy time
parameter

additionally christian packed in a bunch of alternating parameters, namely the rotationAngle this is heavy uncertain if it will stay this way
 so no further parameter explanation for now here
