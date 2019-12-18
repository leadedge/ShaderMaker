/* 
    stbi-write-api - based on public domain JPEG/PNG reader - http://nothings.org/stb_image.c
*/

#ifndef STBI_INCLUDE_STB_IMAGE_WRITE_H
#define STBI_INCLUDE_STB_IMAGE_WRITE_H

#ifndef STBI_NO_STDIO
    #if defined(_MSC_VER) && _MSC_VER >= 0x1400
        #define _CRT_SECURE_NO_WARNINGS // suppress bogus warnings about fopen()
    #endif

    #include <stdio.h>
#endif

#define STBI_VERSION 1


#ifdef __cplusplus
extern "C" {
#endif

// WRITING API

#if !defined(STBI_NO_WRITE) && !defined(STBI_NO_STDIO)
// write a BMP/TGA file given tightly packed 'comp' channels (no padding, nor bmp-stride-padding)
// (you must include the appropriate extension in the filename).
// returns TRUE on success, FALSE if couldn't open file, error writing file
extern int      stbi_write_bmp       (char const *filename,     int x, int y, int comp, void *data);
extern int      stbi_write_tga       (char const *filename,     int x, int y, int comp, void *data);
#endif


#ifdef __cplusplus
}
#endif

//
//
////   end header file   /////////////////////////////////////////////////////
#endif // STBI_INCLUDE_STB_IMAGE_WRITE_H
