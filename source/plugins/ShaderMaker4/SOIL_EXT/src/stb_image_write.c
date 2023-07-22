/* 
    stbi-write-api - based on public domain JPEG/PNG reader - http://nothings.org/stb_image.c
*/

// LJ
#define _CRT_SECURE_NO_WARNINGS

#include "stb_image_write.h"


#ifndef STBI_NO_STDIO
    #include <stdio.h>
#endif

#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <stdarg.h>


// implementation:
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef   signed short  int16;
typedef unsigned int   uint32;
typedef   signed int    int32;
typedef unsigned int   uint;

// should produce compiler error if size is wrong
typedef unsigned char validate_uint32[sizeof(uint32)==4 ? 1 : -1];

#if defined(STBI_NO_STDIO) && !defined(STBI_NO_WRITE)
#define STBI_NO_WRITE
#endif

/////////////////////// write image ///////////////////////

#ifndef STBI_NO_WRITE

static void write8(FILE *f, int x) { uint8 z = (uint8) x; fwrite(&z,1,1,f); }

static void writefv(FILE *f, char *fmt, va_list v)
{
   while (*fmt) {
      switch (*fmt++) {
         case ' ': break;
         case '1': { uint8 x = va_arg(v, int); write8(f,x); break; }
         case '2': { int16 x = va_arg(v, int); write8(f,x); write8(f,x>>8); break; }
         case '4': { int32 x = va_arg(v, int); write8(f,x); write8(f,x>>8); write8(f,x>>16); write8(f,x>>24); break; }
         default:
            assert(0);
            va_end(v);
            return;
      }
   }
}

static void writef(FILE *f, char *fmt, ...)
{
   va_list v;
   va_start(v, fmt);
   writefv(f,fmt,v);
   va_end(v);
}

static void write_pixels(FILE *f, int rgb_dir, int vdir, int x, int y, int comp, void *data, int write_alpha, int scanline_pad)
{
   uint8 bg[3] = { 255, 0, 255}, px[3];
   uint32 zero = 0;
   int i,j,k, j_end;

   if (vdir < 0)
      j_end = -1, j = y-1;
   else
      j_end =  y, j = 0;

   for (; j != j_end; j += vdir) {
      for (i=0; i < x; ++i) {
         uint8 *d = (uint8 *) data + (j*x+i)*comp;
         if (write_alpha < 0)
            fwrite(&d[comp-1], 1, 1, f);
         switch (comp) {
            case 1:
            case 2: writef(f, "111", d[0],d[0],d[0]);
                    break;
            case 4:
               if (!write_alpha) {
                  for (k=0; k < 3; ++k)
                     px[k] = bg[k] + ((d[k] - bg[k]) * d[3])/255;
                  writef(f, "111", px[1-rgb_dir],px[1],px[1+rgb_dir]);
                  break;
               }
               /* FALLTHROUGH */
            case 3:
               writef(f, "111", d[1-rgb_dir],d[1],d[1+rgb_dir]);
               break;
         }
         if (write_alpha > 0)
            fwrite(&d[comp-1], 1, 1, f);
      }
      fwrite(&zero,scanline_pad,1,f);
   }
}

static int outfile(char const *filename, int rgb_dir, int vdir, int x, int y, int comp, void *data, int alpha, int pad, char *fmt, ...)
{
   FILE *f = fopen(filename, "wb");
   if (f) {
      va_list v;
      va_start(v, fmt);
      writefv(f, fmt, v);
      va_end(v);
      write_pixels(f,rgb_dir,vdir,x,y,comp,data,alpha,pad);
      fclose(f);
   }
   return f != NULL;
}

int stbi_write_bmp(char const *filename, int x, int y, int comp, void *data)
{
   int pad = (-x*3) & 3;
   return outfile(filename,-1,-1,x,y,comp,data,0,pad,
           "11 4 22 4" "4 44 22 444444",
           'B', 'M', 14+40+(x*3+pad)*y, 0,0, 14+40,  // file header
            40, x,y, 1,24, 0,0,0,0,0,0);             // bitmap header
}

int stbi_write_tga(char const *filename, int x, int y, int comp, void *data)
{
   int has_alpha = !(comp & 1);
   return outfile(filename, -1,-1, x, y, comp, data, has_alpha, 0,
                  "111 221 2222 11", 0,0,2, 0,0,0, 0,0,x,y, 24+8*has_alpha, 8*has_alpha);
}

// any other image formats that do interleaved rgb data?
//    PNG: requires adler32,crc32 -- significant amount of code
//    PSD: no, channels output separately
//    TIFF: no, stripwise-interleaved... i think

#endif // STBI_NO_WRITE

