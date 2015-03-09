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

#define DEBUG_TAG "MyIpCamera"
#define DCT_SIZE 16
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
	return (jint) malloc(size);
}
void Java_com_lambdazhao_myipcamera_libjpeg_JavaLibjpeg_deleteBuffer(JNIEnv * env, jclass cls,jint p)
{
	free((void*)p);
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;
METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}


void Java_com_lambdazhao_myipcamera_libjpeg_JavaLibjpeg_compressJpeg(JNIEnv * env, jclass cls,jarray srcImage,jobject javaMemDestRef,jobject param)
{
	//converted/origin buffer
	JSAMPLE * yimage_convert_buffer;	/* Points to large array of R,G,B-order data */
	JSAMPLE * uimage_convert_buffer;	/* Points to large array of R,G,B-order data */
	JSAMPLE * vimage_convert_buffer;	/* Points to large array of R,G,B-order data */

	//origin buffer
	JSAMPLE * srcImage_buffer;	/* Points to large array of R,G,B-order data */
	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;
	int height=GetJavaIntValue(env,param,"height");
	int width=GetJavaIntValue(env,param,"width");
	int inputComponents=GetJavaIntValue(env,param,"inputComponents");
	int quality=GetJavaIntValue(env,param,"quality");
	int in_color_space=GetJavaIntValue(env,param,"in_color_space");

	//int row_stride=GetJavaIntValue(env,param,"row_stride");
	JSAMPLE *image_convert_buffer=(JSAMPLE *)GetJavaIntValue(env,param,"image_convert_buffer");
	JSAMPROW row_pointer[3];	/* pointer to JSAMPLE row[s] */

	cinfo.err = jpeg_std_error(&jerr);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
	/* If we get here, the JPEG code has signaled an error.
	 * We need to clean up the JPEG object, close the input file, and return.
	 */
		char buffer[JMSG_LENGTH_MAX];

		/* Create the message */
		(cinfo.err->format_message) (&cinfo, buffer);
		__android_log_print(DEBUG_TAG, "JavaLibjpeg_compressJpeg", "ERROR=%s",buffer );

		jpeg_destroy_decompress(&cinfo);
		return ;
	}
	jpeg_create_compress(&cinfo);
	jboolean isCopy=0;
	srcImage_buffer=(*env)->GetByteArrayElements(env,srcImage,&isCopy);
	//image_buffer=srcImage_buffer;
	if(in_color_space==JCS_NV12)
	{
		nv12_yu12_convert(srcImage_buffer,image_convert_buffer,&yimage_convert_buffer,&uimage_convert_buffer,&vimage_convert_buffer, width,height);
		in_color_space=JCS_YCbCr;
		//row_stride=width;
		//image_buffer=image_convert_buffer;
	}

	java_mem_dest(&cinfo,env,javaMemDestRef);

	cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = 1;//inputComponents;		/* # of color components per pixel */
	cinfo.in_color_space = in_color_space; 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	cinfo.raw_data_in = TRUE;//nv12 raw data
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);

	int line =0,i;
    JSAMPROW y[16],u[16],v[16];
    row_pointer[0]=y;
    row_pointer[1]=u;
    row_pointer[2]=v;

	for(line=0;line< cinfo.image_height;line+=DCT_SIZE) {
		for (i=0; i<DCT_SIZE; i++)
		{
			y[i] = yimage_convert_buffer +cinfo.image_width*(line+i);
		    if (i%2 == 0) {
		    	 u[i/2] =uimage_convert_buffer +(cinfo.image_width/2)*((line+i)/2);
		    	 v[i/2] =vimage_convert_buffer +(cinfo.image_width/2)*((line+i)/2);
		    }
		}
		jpeg_write_raw_data(&cinfo,row_pointer,DCT_SIZE);
		//(void) jpeg_write_scanlines(&cinfo, row_pointer, 2);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	(*env)->ReleaseByteArrayElements(env,srcImage,srcImage_buffer,JNI_ABORT);

}
