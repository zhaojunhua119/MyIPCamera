


#ifndef JAVAMEMDST_H
#define JAVAMEMDST_H

#include "jpeglib.h"

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  //unsigned long outsize;

  JOCTET * buffer;		/* start of buffer,must be set to null if the java array is unlocked */
  int bufsize;
  boolean alloc;
  JNIEnv * jnienv;
  jobject jobject_ref;
} java_mem_destination_mgr;

typedef java_mem_destination_mgr * java_mem_dest_ptr;



GLOBAL(void)
java_mem_dest(j_compress_ptr cinfo,JNIEnv * jnienv,jobject jobject_ref);

#endif
