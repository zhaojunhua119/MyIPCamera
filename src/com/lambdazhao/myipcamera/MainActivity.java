package com.lambdazhao.myipcamera;



import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

import com.lambdazhao.myipcamera.libjpeg.JavaLibjpeg;

import com.lambdazhao.myipcamera.libjpeg.CompressJpegParam;
import com.lambdazhao.myipcamera.libjpeg.JavaLibjpeg.JpegColorSpace;
import com.lambdazhao.myipcamera.libjpeg.JavaMemDest;

import android.app.Activity;
import android.os.Bundle;
import android.os.Environment;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {
	Button btnTrigger=null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		btnTrigger=(Button)this.findViewById(R.id.btnTrigger);
		btnTrigger.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
			
				byte array[]=new byte[640*480*3];
				int k=0;
				for(int j=0;j<480;j++)
				{
					for(int i=0;i<640;i++)
					{
						int clr=i*j;
						
						array[k++]=(byte) (clr|0xFF);
						array[k++]=(byte) ((clr>>8)|0xFF);
						array[k++]=(byte) ((clr>>16)|0xFF);
						
					}
				}
				JavaMemDest javaMemDest=new JavaMemDest(1000);
				CompressJpegParam param=new CompressJpegParam();
				param.width=640;
				param.height=480;
				param.in_color_space=JpegColorSpace.JCS_RGB.value;
				param.inputComponents=3;
				
				JavaLibjpeg.compressJpeg(array, javaMemDest,param);
				File file=new File(Environment.getExternalStorageDirectory().getAbsolutePath()+"/fuck1.jpg");
				
				FileOutputStream fos;
				try {
					fos = new FileOutputStream(file);
					fos.write(javaMemDest.GetBuffer(),0, javaMemDest.GetOutSize());
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			    
				//test1(Environment.getExternalStorageDirectory().getAbsolutePath()+"/fuck1.jpg");
			}
        	
        });
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	public native void test1(String path);
	static {
		JavaLibjpeg.loadLibrary();
	}
	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}
}
