/*
 * javalibjpeg.c
 *
 *  Created on: Feb 27, 2015
 *      Author: zhahua
 */

#include <setjmp.h>
#include <jni.h>
#include <string.h>
#include <android/log.h>
#include <stdio.h>

#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "javamemdst.h"
#include "mycolorconverter.h"
int GetJavaIntValue(JNIEnv *env,jobject obj,char* fieldName)
{
	struct jpeg_compress_struct cinfo;
	jclass class = (*env)->GetObjectClass(env,obj);
	jfieldID f = (*env)->GetFieldID(env,class, fieldName, "I");
	int result= (*env)->GetIntField(env,obj, f);
	(*env)->DeleteLocalRef(env,class);
	return result;
}

jint Java_com_lambdazhao_myipcamera_libjpeg_JavaLibjpeg_newBuffer(JNIEnv * env, jclass cls,jint size)
{
	return malloc(size);
}
void Java_com_lambdazhao_myipcamera_libjpeg_JavaLibjpeg_deleteBuffer(JNIEnv * env, jclass cls,jint p)
{
	free(p);
}


void Java_com_lambdazhao_myipcamera_libjpeg_JavaLibjpeg_compressJpeg(JNIEnv * env, jclass cls,jarray srcImage,jobject javaMemDestRef,jobject param)
{
	//converted/origin buffer
	JSAMPLE * image_buffer;	/* Points to large array of R,G,B-order data */
	//origin buffer
	JSAMPLE * srcImage_buffer;	/* Points to large array of R,G,B-order data */
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int height=GetJavaIntValue(env,param,"height");
	int width=GetJavaIntValue(env,param,"width");
	int inputComponents=GetJavaIntValue(env,param,"inputComponents");
	int quality=GetJavaIntValue(env,param,"quality");
	int in_color_space=GetJavaIntValue(env,param,"in_color_space");

	int row_stride=GetJavaIntValue(env,param,"row_stride");
	JSAMPLE *image_convert_buffer=GetJavaIntValue(env,param,"image_convert_buffer");
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jboolean isCopy=0;
	srcImage_buffer=(*env)->GetByteArrayElements(env,srcImage,&isCopy);
	image_buffer=srcImage_buffer;
	if(in_color_space==JCS_NV12)
	{
		nv12_yuv_convert(image_buffer,image_convert_buffer,width,height);
		in_color_space=JCS_YCbCr;
		row_stride=width*3;
		image_buffer=image_convert_buffer;
	}

	java_mem_dest(&cinfo,env,javaMemDestRef);

//	java_mem_dest_ptr dest=(java_mem_dest_ptr)cinfo.dest;
//
//	dest->pub.init_destination(&cinfo);
//	JOCTET* p=dest->pub.next_output_byte;
//	int i;
//	unsigned char c=0;
//	for(i=0;i<dest->pub.free_in_buffer;i++)
//		*(p++)=c++;
//	dest->pub.empty_output_buffer(&cinfo);
//	p=dest->pub.next_output_byte;
//	for(i=0;i<dest->pub.free_in_buffer;i++)
//		*(p++)=c++;
//	dest->pub.empty_output_buffer(&cinfo);
//	p=dest->pub.next_output_byte;
//	for(i=0;i<dest->pub.free_in_buffer;i++)
//		*(p++)=c++;
//	dest->pub.free_in_buffer=2;
//	dest->pub.term_destination(&cinfo);

	cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = inputComponents;		/* # of color components per pixel */
	cinfo.in_color_space = in_color_space; 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & image_convert_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	(*env)->ReleaseByteArrayElements(env,srcImage,srcImage_buffer,JNI_ABORT);

}
