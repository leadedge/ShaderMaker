 
# Repository 

this repository is a collection of adjusted [https://github.com/leadedge/ShaderMaker][ShaderMaker] templates with a very high redundancy,
the build process is - as of now - only manual 64Bit build in [https://visualstudio.microsoft.com/](VisualStudio) and copying of .dll files to resolume plugin directory.

- in VisualStudio set 'Output Directory' to your Resolume Plugin Folder\
- in VisualStudio add Post-Build-Event start Resolume


## Quickstart (only dll copy)

the precompiled 64 bit windows .dll files can be found in the release folder of this repository, just copy them to your resolume plugin folder to check them out

in the release folder is a .resolume file with an example deck containing examples for most of the .dlls



## Concepts

since this is a public repository and - as described above - only serves the demonstration of chaos theory let me include the ideas that will help to understand why the particular resolume plugins are designed the way they are

### 3-Arity
[![Generic badge](https://img.shields.io/badge/3ARY-YES-green.svg)](https://shields.io/)
the [https://www.wikiwand.com/en/Three-body_problem_(disambiguation)](three body problem) leads to a simple conclusion: 'you just need 3'. Christian conclusing is in popular speaking that 3 makes an interesting pattern, which is we may not encounter the exactly underlying rules at first glance, which makes it interesting. 
 
 		hint:
 		
 		prime number repetitions make for very long loops, 
 		let's say we have 3 angles that rotate, each at the same speed
 		then the whole system wil repeat after 1 cycle
 		 
 		now, when using different speeds one can caluclate the repeat cycle 
 		like: speed1*speed2*speed3
 		
 		e.g. when the speeds are 1 and twice and triple the speed of the first
 		the result would be 1*2*3= 6 cycles needed to repeat the whole process
 		so it takes 6 times longer
 		
 		and here comes the tip, using prime numbers here for creating non visible loops
 		
 		
### Repeatable parameters
![Generic badge](https://img.shields.io/badge/LOOPABLE-YES-green.svg)
 some parameters in the plugins are repeatable by nature, for example the angle parameter of any effect can be easily
 animated becaus when it jumps from end to beginning (360degree to 0degree) its exactly the same repeating the whole process
 
 Resolume6 introduced the parameter hull curve functionality which allows you to make evey parameter loopable 
 		 
 		 
 		NOTE ON RESOLUME 6 Hull Curve improvements
 		as of the current version of resolume it does not contain:
 		- please introduce the ping-pong loop mode for all of your timelines
 		- make hull curves loopable and allow the first and last keyframes not sticked to the borders



### Simple / Complex

when a effect is available in SIMPLE or COMPLEX mode, basically means the complex has more parameters :) but beside of that, the complexity
usually introduces the same 2 parameters for a selected bunch of effect parameters, which are:

- radius
- speed

which gives a - usually a 2 or 3 dimensional - parameter a radius which it rotates around with defined speed, see remarks about time for more information

### Time  
[http://shadertoy.com](Shadertoy.com) which defined the main syntax for the shader parameters introduces a 'time' concept, this time conecpt
is so utterly important for working with the provided effects by this repository that i need to loose a word about it

- the time can usually NOT be reseted or rolled back, its a continuos value that increases up to defined limit ( 32/64 bit) by the speeds  

- there may be more than 1 time variable : some effects introduce own time variables with their own, either by extended mode or by definition (example: sinus) 

## Hybridisation -> Alternation

[![Generic badge](https://img.shields.io/badge/HYBRID-YES-green.svg)](https://shields.io/) the method used in all of the plugins that deal with chaotic image creation (fractals) use a simple hybridisation approach, it is always using 3-alternate-hybrids, meaning that when alternation starts, three sets of parameters are cycled through ind subsequent iterations

## Exponentional Parameters

[![Generic badge](https://img.shields.io/badge/EXPONENTIAL-YES-green.svg)](https://shields.io/) some parameters use exponential scaling by nature, and are marked with the  badge - the input value x in {0..1} is exponentiated (x*x)


