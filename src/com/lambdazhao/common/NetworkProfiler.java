package com.lambdazhao.common;

import java.util.HashMap;
import java.util.Map;

import android.util.Log;

public class NetworkProfiler {
	long dataVolume=0;
	long lastTS=0;
	long dumpTimeBetween=1000;
	String name;
	String tag;
	public NetworkProfiler(String name,String tag)
	{
		this.name=name;
		this.tag=tag;
	}
	public void AddVolume(long _dataVolume)
	{
		if(lastTS==0)
		{
			lastTS=System.currentTimeMillis();
			return;
		}
		dataVolume+=_dataVolume;
		
		long current=System.currentTimeMillis();
		if(current-lastTS>dumpTimeBetween)
		{
			Log.i(tag, name + "Network Transfer Speed="+(dataVolume/(current-lastTS))+"kB/s");
			lastTS=current;
			dataVolume=0;
		}
	}
}
