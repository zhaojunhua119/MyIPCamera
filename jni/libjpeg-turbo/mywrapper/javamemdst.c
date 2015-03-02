#include <setjmp.h>
#include <jni.h>
#include <string.h>
#include <android/log.h>
#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"
#include "javamemdst.h"

#define DEBUG_TAG "javamemdst"
LOCAL(void) setJavaRefOutSize(JNIEnv * jnienv,jobject classref,jint size);

LOCAL(void)
attachJavaRefBuffer(j_compress_ptr cinfo,java_mem_dest_ptr dest);

LOCAL(void)
detachJavaRefBuffer(j_compress_ptr cinfo,java_mem_dest_ptr dest);

LOCAL(void)
resizeAndReattachJavaRefBuffer(j_compress_ptr cinfo,java_mem_dest_ptr dest,int newSize);

METHODDEF(void)
init_mem_destination (j_compress_ptr cinfo)
{
  /* no work necessary here */
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "%s", "init_mem_destination");

}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */

METHODDEF(boolean)
empty_mem_output_buffer (j_compress_ptr cinfo)
{


	int newBufSize;
	//JOCTET * nextbuffer;
	java_mem_dest_ptr dest = (java_mem_dest_ptr) cinfo->dest;

	int oldBufSize=dest->bufsize;
	newBufSize = oldBufSize * 2;
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "empty_mem_output_buffer oldSize=%d,newSize=%d", oldBufSize,newBufSize);
	//resize the java buffer and reset the dest->buffer and dest->buffSize
	resizeAndReattachJavaRefBuffer(cinfo,dest,newBufSize);

	dest->pub.next_output_byte = dest->buffer + oldBufSize;
	dest->pub.free_in_buffer = newBufSize-oldBufSize;
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "empty_mem_output_buffer next_output_byte=%08x, free_in_buffer=%01x",(uint)dest->pub.next_output_byte,(uint)dest->pub.free_in_buffer);


	return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_mem_destination (j_compress_ptr cinfo)
{
  java_mem_dest_ptr dest = (java_mem_dest_ptr) cinfo->dest;

  setJavaRefOutSize(dest->jnienv,dest->jobject_ref,dest->bufsize-dest->pub.free_in_buffer);
  detachJavaRefBuffer(cinfo,dest);
  __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "term_mem_destination free_in_buffer=%d",(uint)dest->pub.free_in_buffer);


}



GLOBAL(void)
java_mem_dest(j_compress_ptr cinfo,JNIEnv * jnienv,jobject jobject_ref)
{
  java_mem_dest_ptr dest;

  /* The destination object is made permanent so that multiple JPEG images
   * can be written to the same buffer without re-executing jpeg_mem_dest.
   */
  if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(java_mem_destination_mgr));
    dest = (java_mem_dest_ptr) cinfo->dest;
  }
  dest = (java_mem_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_mem_destination;
  dest->pub.empty_output_buffer = empty_mem_output_buffer;
  dest->pub.term_destination = term_mem_destination;
  dest->buffer=0;
  dest->byteArray=0;
  dest->bufsize=0;
  dest->jnienv=jnienv;
  dest->jobject_ref=jobject_ref;
  attachJavaRefBuffer(cinfo,dest);
  dest->pub.free_in_buffer = dest->bufsize;
  dest->pub.next_output_byte = dest->buffer;
  __android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "java_mem_dest next_output_byte=%08x",(uint)dest->pub.next_output_byte);

}

LOCAL(void) setJavaRefOutSize(JNIEnv * jnienv,jobject classref,jint size)
{
	jclass jc = (*jnienv)->GetObjectClass(jnienv,classref);
	jmethodID mid = (*jnienv)->GetMethodID(jnienv,jc, "SetOutSize","(I)V");
	(*jnienv)->CallVoidMethod(jnienv,classref, mid,size);
	(*jnienv)->DeleteLocalRef(jnienv,jc);


}
//jni wrapper

LOCAL(jbyte*)
lockJavaRefBuffer(JNIEnv * jnienv,jobject classref,jboolean *isCopy,jsize *bufSize,jbyteArray *byteArray)
{
	jclass jc = (*jnienv)->GetObjectClass(jnienv,classref);
	jmethodID mid = (*jnienv)->GetMethodID(jnienv,jc, "GetBuffer","()[B");
	*byteArray = (jbyteArray)(*jnienv)->CallObjectMethod(jnienv,classref, mid);
	*bufSize=(*jnienv)->GetArrayLength(jnienv, *byteArray);
	jbyte* buffer = (*jnienv)->GetByteArrayElements(jnienv,*byteArray,isCopy);
	(*jnienv)->DeleteLocalRef(jnienv,jc);
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "lockJavaRefBuffer byteArray=%08x,buffer=%08x",(uint)*byteArray,(uint)buffer);
	return buffer;
}
//jni wrapper
LOCAL(void)
unlockJavaRefBuffer(JNIEnv * jnienv,jbyteArray byteArray,jbyte *buffer)
{
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "unlockJavaRefBuffer byteArray=%08x,buffer=%08x",(uint)byteArray,(uint)buffer);

	(*jnienv)->ReleaseByteArrayElements(jnienv,byteArray,buffer,0);


}
LOCAL(void)
attachJavaRefBuffer(j_compress_ptr cinfo,java_mem_dest_ptr dest)
{
	if(dest->buffer!=NULL)
		ERREXIT(cinfo, JERR_BUFFER_SIZE);
	jboolean isCopy=0;
	jbyteArray byteArray=0;

	int i=0;
	jbyte *buffer=lockJavaRefBuffer(dest->jnienv,dest->jobject_ref,&isCopy,&(dest->bufsize),&byteArray);

	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "attachJavaRefBuffer bufSize=%d",dest->bufsize);

	if(isCopy!=0)
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "attachJavaRefBuffer isCopy=true");
		//ERREXIT(cinfo, JERR_BUFFER_SIZE);
	dest->buffer=buffer;
	dest->byteArray=byteArray;



}

LOCAL(void)
detachJavaRefBuffer(j_compress_ptr cinfo,java_mem_dest_ptr dest)
{
	if(dest->buffer!=NULL)
	{
		__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "detachJavaRefBuffer");

		unlockJavaRefBuffer(dest->jnienv,dest->byteArray,dest->buffer);
		dest->buffer=NULL;
		dest->byteArray=NULL;
	}
}

LOCAL(void)
resizeAndReattachJavaRefBuffer(j_compress_ptr cinfo,java_mem_dest_ptr dest,jint newSize)
{
	int oldSize=dest->bufsize;
	detachJavaRefBuffer(cinfo,dest);
	JNIEnv * jnienv=dest->jnienv;
	jclass jc = (*jnienv)->GetObjectClass(jnienv,dest->jobject_ref);
	jmethodID mid = (*jnienv)->GetMethodID(jnienv,jc, "ResizeBuffer","(I)I");
	__android_log_print(ANDROID_LOG_DEBUG, DEBUG_TAG, "resizeAndReattachJavaRefBuffer oldSize=%d, newSize=%d",oldSize,newSize);
	jint result = (jint)(*jnienv)->CallIntMethod(jnienv,dest->jobject_ref, mid,newSize);
	(*jnienv)->DeleteLocalRef(jnienv,jc);

	if(result!=0)
		ERREXIT(cinfo, JERR_BUFFER_SIZE);

	attachJavaRefBuffer(cinfo,dest);
}
