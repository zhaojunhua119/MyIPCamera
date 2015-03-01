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
int GetJavaIntValue(JNIEnv *env,jobject obj,char* fieldName)
{
	jclass class = (*env)->GetObjectClass(env,obj);
	jfieldID f = (*env)->GetFieldID(env,class, fieldName, "I");
	return (*env)->GetIntField(env,obj, f);
}

void Java_com_lambdazhao_myipcamera_JavaLibjpeg_libjpeg(JNIEnv * env, jobject this,jbyteArray srcImage,jobject javaMemDestRef,jobject param)
{

	JSAMPLE * image_buffer=NULL;	/* Points to large array of R,G,B-order data */
	int height=GetJavaIntValue(env,param,"height");
	int width=GetJavaIntValue(env,param,"width");
	int inputComponents=GetJavaIntValue(env,param,"inputComponents");
	int quality=GetJavaIntValue(env,param,"quality");
	int in_color_space=GetJavaIntValue(env,param,"in_color_space");
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jboolean isCopy=0;
	image_buffer=(*env)->GetByteArrayElements(env,srcImage,&isCopy);
	java_mem_dest(&cinfo,env,javaMemDestRef);

	java_mem_dest_ptr dest=(java_mem_dest_ptr)cinfo.dest;

	dest->pub.init_destination(&cinfo);
	dest->pub.empty_output_buffer(&cinfo);
	dest->pub.empty_output_buffer(&cinfo);
	dest->pub.free_in_buffer=450;
	dest->pub.term_destination(&cinfo);

	cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = inputComponents;		/* # of color components per pixel */
	cinfo.in_color_space = in_color_space; 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = width * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);


}
