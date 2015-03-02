#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#ifndef my_color_convert
#define my_color_convert

#define JCS_NV12 1000

void nv12_yuv_convert(JSAMPLE* src,JSAMPLE *dst,int width,int height);
#endif
