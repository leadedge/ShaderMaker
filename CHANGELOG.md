 # Changelog 
 
 # 2018 +
 
2.1.0 - [2019-04-25]

- fine tune kali xt parameters:
   - scale is only negative
   - julia is only positive
   - lighting and distance estimation adjustments
 
2.0.1 - [2019-04-25] 

- NVIDIA Fix - most recent nvidia cards do not allow dynamic array access, falling back to
   expanded style, shouldnt be too much of an calculation impact, due to most global initialisers
   work, just an if/else branching inside loop
  [001 Kali Sea Creature Extended](#001-kali-sea-creature-extended)  

2.0.0 - [2018-09-8] 

this 2.0 release is a rework and update for 64bit resolume, all plugins are available as 32 and 64 bit versions. Only Windows
binaries included.

Included in the 2.0 release:
- [Source Plugins](#source-plugins)
   - [001 Mandelbrot](#001-mandelbrot)
   - [002 Mandelbox](#002-mandelbox)
   - [003 Sinewave 1D](#003-sinewave-1d)
   - [004 Circle Sun](#004-circle-sun)
   - [005 3 Balls](#005-3-balls)
   - [006 Starfield](#006-starfield)
   - [007 Tori](#007-tori)
-  [Effect Plugins](#effect-plugins)
   - [001 God Rays](#001-god-rays)
   - [003 Tilt Shift](#003-tilt-shift)
- [Shadertoy Conversions](#shadertoy-conversions)
   - [001 Kali Sea Creature](#001-kali-sea-creature) 
   - [001 Kali Sea Creature Extended](#001-kali-sea-creature-extended) 
 
 # Pre 2018 
 
 this shader collection started as loose collection for 32bit shaders, with the advent and the necesity to convert
 existing effects to their 64 counterparts a rework of the repository setup, and the full usage of git repo is governed
 in henve there exist no changelog for the 32 bit versions - all of the upcomming effects will be 64/32 bit but they are
 not direct successors of the pre-2018 era
