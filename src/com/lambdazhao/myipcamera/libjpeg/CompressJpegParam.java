package com.lambdazhao.myipcamera.libjpeg;

import com.lambdazhao.myipcamera.libjpeg.JavaLibjpeg.JpegColorSpace;

public class CompressJpegParam
{
	public int width;
	public int height;
	public int inputComponents;
	public int quality=25;
	public int in_color_space=JpegColorSpace.JCS_RGB.value;
	public CompressJpegParam(){}
}