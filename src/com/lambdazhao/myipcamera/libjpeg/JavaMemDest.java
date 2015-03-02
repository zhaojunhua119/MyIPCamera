package com.lambdazhao.myipcamera.libjpeg;

import android.util.Log;

public class JavaMemDest {

	  private byte []buffer=null;

	  private int outSize=0;
	  public JavaMemDest(int size)
	  {
		  buffer=new byte[size];
	  }
	  public byte[] GetBuffer()
	  {
		  return buffer;
	  }
	  public int ResizeBuffer(int newBufSize)
	  {
		  Log.d("JavaMemDest","ResizeBuffer oldSize="+buffer.length+" newSize="+newBufSize);
		  byte[] newBuffer=new byte[newBufSize];
		  if(buffer!=null)
			  System.arraycopy(buffer, 0, newBuffer, 0, buffer.length);
		  buffer=newBuffer;
		  return 0;
	  }
	 
	  public int GetOutSize()
	  {
		  return outSize;
	  }
	  public void SetOutSize(int outSize)
	  {
		  	this.outSize=outSize;
	  }
	  
}
