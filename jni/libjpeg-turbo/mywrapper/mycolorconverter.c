#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "mycolorconverter.h"

void nv12_yuv_convert(JSAMPLE* src,JSAMPLE *dst,int width,int height)
{
	//http://stackoverflow.com/questions/5272388/extract-black-and-white-image-from-android-cameras-nv21-format
	int pixelSum=width*height;
	int bufferSize=width*height*3;
	int Stride1=width;
	int Stride3=width*3;

	JSAMPLE* y=src;
	
	JSAMPLE* u=src+pixelSum;
	JSAMPLE* uend=src+pixelSum+pixelSum/2;
	JSAMPLE* d=dst;
	int t,l;
	for(t=0;t<height;t+=2)
	{
		for(l=0;l<width;l+=2)
		{
			//y
			*d=*y;
			*(d+Stride3)=*(y+Stride1);
			*(d+3)=*(y+1);
			*(d+Stride3+3)=*(y+Stride1+3);
			//u
			*(d+1)=*(u+1);
			*(d+Stride3+1)=*(u+1);
			*(d+4)=*(u+1);
			*(d+Stride3+4)=*(u+1);

			//v
			*(d+2)=*u;
			*(d+Stride3+2)=*u;
			*(d+5)=*u;
			*(d+Stride3+5)=*u;


			d+=6;
			y+=2;
			u+=2;
		}
		d+=Stride3;
		y+=Stride1;
	}
}
