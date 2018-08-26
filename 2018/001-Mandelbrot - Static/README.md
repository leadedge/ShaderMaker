# Spack-O-Mat Mandelbrot Static

A hybrid mandelbrot renderer, providing alternation hybridisation, using different seeds for each step.

# Parameters

# Start of alternation

this parameter controls at which iteration the alternation should start, left is at iteration 0. This comes in very handy,
because for the part the image stays the same, use this to control the 'frame' of your setup

## Transformation

Affine transformations are controlled usiong the properties

### Center X/Y

the center of the image

### Zoom

the scale of the image

### Rotation

the rotation of the image

## Seeds

The seeds is what makes the most interesting part, each seed value varies from -2..2. When all seeds are at value 0 the result is the 
classic mandelbrot set. By moving these seeds one should easily encounter the control these seeds provide

### Seed 1..n real/imag

these are the seeds that become added in the iteration loop at the iteration modulus 4 indexes