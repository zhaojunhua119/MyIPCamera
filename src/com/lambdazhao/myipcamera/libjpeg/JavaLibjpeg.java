package com.lambdazhao.myipcamera.libjpeg;

public class JavaLibjpeg {
	public static void loadLibrary()
	{
		System.load("libjpeg3.so");	
	}

	public enum JpegColorSpace {
		JCS_UNKNOWN,		/* error/unspecified */
		JCS_GRAYSCALE(1),		/* monochrome */
		JCS_RGB(2),		/* red/green/blue as specified by the RGB_RED, RGB_GREEN,
					   RGB_BLUE, and RGB_PIXELSIZE macros */
		JCS_YCbCr(3),		/* Y/Cb/Cr (also known as YUV) */
		JCS_CMYK,		/* C/M/Y/K */
		JCS_YCCK,		/* Y/Cb/Cr/K */
		JCS_EXT_RGB,		/* red/green/blue */
		JCS_EXT_RGBX,		/* red/green/blue/x */
		JCS_EXT_BGR(8),		/* blue/green/red */
		JCS_EXT_BGRX,		/* blue/green/red/x */
		JCS_EXT_XBGR,		/* x/blue/green/red */
		JCS_EXT_XRGB,		/* x/red/green/blue */
		/* When out_color_space it set to JCS_EXT_RGBX, JCS_EXT_BGRX,
		   JCS_EXT_XBGR, or JCS_EXT_XRGB during decompression, the X byte is
		   undefined, and in order to ensure the best performance,
		   libjpeg-turbo can set that byte to whatever value it wishes.  Use
		   the following colorspace constants to ensure that the X byte is set
		   to 0xFF, so that it can be interpreted as an opaque alpha
		   channel. */
		JCS_EXT_RGBA,		/* red/green/blue/alpha */
		JCS_EXT_BGRA,		/* blue/green/red/alpha */
		JCS_EXT_ABGR,		/* alpha/blue/green/red */
		JCS_EXT_ARGB,		/* alpha/red/green/blue */
		JCS_RGBA_8888,  /* red/green/blue/alpha */
		JCS_RGB_565(17),     /* red/green/blue in 565 format */
		JCS_NV12(1000);
		public int value;
		JpegColorSpace(int value)
		{
			this.value=value;
		}
		JpegColorSpace()
		{

		}
		
	}
	public static native void compressJpeg(byte[] imageBuffer,JavaMemDest javaMemDest,CompressJpegParam param);
	public static native int newBuffer(int size);
	public static native int deleteBuffer(int bufferPointer);
	
}
