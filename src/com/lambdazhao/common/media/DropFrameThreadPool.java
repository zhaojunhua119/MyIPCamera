package com.lambdazhao.common.media;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;


public class DropFrameThreadPool {

	  ArrayList<Thread> threads=null;
	  ArrayList<DropFrameThread> runnables=null;
	  public int maxThreadSize;
	  public int currentThreadSize;
	  public String name;
	
	public DropFrameThreadPool(String name,int currentThreadSize, int maxThreadSize)
	{
		this.maxThreadSize=maxThreadSize;
		this.currentThreadSize=currentThreadSize;
		this.name=name;
		threads=new ArrayList<Thread>();
		runnables=new ArrayList<DropFrameThread>();
		for (int i=0;i<maxThreadSize;i++)
		{
			DropFrameThread runnable=new DropFrameThread();
			Thread th=new Thread(runnable);
			threads.add(th);
			runnables.add(runnable);
			th.setName(name+"_"+i);
			th.start();
		}
		
	}
	public void Dispose()
	{
		for(int i=0;i<currentThreadSize;i++)
		{
			DropFrameThread dropFrameThread=runnables.get(i);
			dropFrameThread.isNeedToBeStopped=true;
			synchronized(dropFrameThread.signal){
				dropFrameThread.isSignaled=true;
				dropFrameThread.signal.notify();
			}
		}
	}
	//return false if it is dropped
	public boolean Schedule(Runnable runnable)
	{
		for(int i=0;i<currentThreadSize;i++)
		{
			DropFrameThread dropFrameThread=runnables.get(i);
			if(!dropFrameThread.isRunning)
			{
				synchronized(dropFrameThread)
				{
					if(!dropFrameThread.isRunning)
					{
						dropFrameThread.task=runnable;
						synchronized(dropFrameThread.signal)
						{
							dropFrameThread.isSignaled=true;
							dropFrameThread.signal.notify();
						}
						return true;
					}
				}
			}
		}
		return false;
	}
	/**
	 * @param args
	 */
	public static void maind(String[] args) {
		// TODO Auto-generated method stub
		DropFrameThreadPool pool=new DropFrameThreadPool("Test",2,2);
		int i=0;
		int s=0;
		int d=0;
		for(;i<1000;i++)
		{
			boolean result=pool.Schedule(new Runnable(){
	
				@Override
				public void run() {
					// TODO Auto-generated method stub
					try {
						Thread.sleep(4);
						} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						//e.printlnStackTrace();
					}
				}
				
			});
			if(result==true)
				s++;
			else 
				d++;
			try {
				Thread.sleep(1);
			} catch (InterruptedException e){ 
				// TODO Auto-generated catch block
			}
		}
		pool.Dispose();
	}
	
}

class DropFrameThread implements Runnable{
	public boolean isSignaled=false;
	public Object signal=new Object();
	public volatile Runnable task;
	public volatile boolean isNeedToBeStopped=false;
	public volatile boolean isRunning=false;
	@Override
	public void run() {
		while(true)
		{
		//	System.out.println(Thread.currentThread().getName()+"is waiting");
			try {
				synchronized(signal)
				{
					while(!isSignaled)
						signal.wait();
					isSignaled=false;
				}
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				return;
			}
			if(isNeedToBeStopped)
			{
				//System.out.println(Thread.currentThread().getName()+"exited");
				return;
			}
			isRunning=true;
		//	System.out.println(Thread.currentThread().getName()+"start to run");
			
			task.run();
			isRunning=false;
		}
	}
	
}
