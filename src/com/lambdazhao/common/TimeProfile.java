package com.lambdazhao.common;

import java.util.ArrayDeque;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import android.R.string;
import android.util.Log;

public class TimeProfile {
	 // Thread local variable containing each thread's ID
    private static final ThreadLocal<Queue<TimeProfile>> queue =
        new ThreadLocal<Queue<TimeProfile>>() {
            @Override 
            protected Queue<TimeProfile> initialValue() {
                return new ArrayDeque<TimeProfile>();
        }
    };
    private String tag;
    private String name;
    
    private long tsLast=0;
    public static TimeProfile Start(String tag,String name)
    {
    	Queue<TimeProfile> profileQueue=queue.get();
    	if(profileQueue.size()==0)
    	{
    		profileQueue.add(new TimeProfile());
    	}
    	TimeProfile profile=profileQueue.poll();
    	profile.tsLast=System.currentTimeMillis();
    	profile.tag=tag;
    	profile.name=name;
    	return profile;
    }
    public void ShowElapse(string subname)
    {
    	Log.i(this.tag, String.format("%s_%s timeElapse=%dms",this.name,subname,System.currentTimeMillis()-this.tsLast));
    	
    }
    public void End()
    {
    	Log.i(this.tag, String.format("%s timeElapse=%dms",this.name,System.currentTimeMillis()-this.tsLast));
    	Queue<TimeProfile> profileQueue=queue.get();
    	profileQueue.add(this);
    }
    
    
}
