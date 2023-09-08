
#ifndef AE_ENABLE_FREETYPE
# error AE_ENABLE_FREETYPE required
#endif

#define _INC_STDDEF
#define _INC_STRING
#define _INC_STDLIB
#define _INC_ERRNO
#define _INC_STDIO

#include <ft2build.h>
#include <freetype/freetype.h>

#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H
#include FT_TRUETYPE_TABLES_H


int main ()
{
    return 0;
}
