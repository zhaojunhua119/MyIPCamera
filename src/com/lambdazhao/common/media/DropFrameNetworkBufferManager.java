package com.lambdazhao.common.media;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;

import com.lambdazhao.myipcamera.libjpeg.JavaMemDest;


public class DropFrameNetworkBufferManager {
	//lock is not required
	ConcurrentLinkedQueue<JavaMemDest> freeBufferPool=null;
	//lock is required
	LinkedList<JavaMemDest> transferQueue=null;
	
	JavaMemDest transferBuffer=null;
	//lock not required
	ThreadLocal<JavaMemDest> writeBuffer=null;
	
	boolean isSignaled=false;
	Object signal=new Object();
	
	int transferQueueSize=0;
	public static void  printf(String format, Object ... args)
	{
		System.out.printf(Thread.currentThread().getName() +": "+format, args);
	}
	public static void println(String format)
	{
		System.out.println(Thread.currentThread().getName() +": "+format);
	}
	
	public void dump()
	{
		synchronized(transferQueue)
		{
			System.out.print("Dump: transferQueue=");
			for(int i=0;i<transferQueue.size();i++)
			{
	//			System.out.printf("n="+transferQueue.get(i).name+"v="+transferQueue.get(i).data);
			}
			System.out.println("");
		}
		synchronized(freeBufferPool)
		{

			Object[] array=freeBufferPool.toArray();
			printf("Dump: freeBufferPool=");
			for(int i=0;i<array.length;i++)
			{
			//	System.out.printf("n="+((JavaMemDest)array[i]).name+"v="+((JavaMemDest)array[i]).data);
			}
			System.out.println("");
		}
	}
	public DropFrameNetworkBufferManager(int transferQueueSize)
	{
		
		this.transferQueueSize=transferQueueSize;
		freeBufferPool=new ConcurrentLinkedQueue<JavaMemDest>();
		transferQueue=new LinkedList<JavaMemDest>();
		writeBuffer =new ThreadLocal<JavaMemDest>(){
			 @Override protected JavaMemDest initialValue() {
				 JavaMemDest ret = new JavaMemDest();
			//	 println("Create "+ret.name);
	             return ret;
			 }
			 
		};
		//transferQueue& tranBuffer is empty
		for(int i=0;i<transferQueueSize+1;i++)
			freeBufferPool.add(new JavaMemDest());
		transferBuffer=null;
		
	}
	public JavaMemDest GetWriteBuffer()
	{
		return writeBuffer.get();
	}
	public  boolean ScheduleWriteBuffer() throws Exception
	{
		synchronized(transferQueue)
		{
			if(transferQueue.size()<transferQueueSize)
			{
				JavaMemDest temp=writeBuffer.get();
				transferQueue.add(temp);
				if(temp==null)
					throw new Exception();
				temp=freeBufferPool.poll();
				writeBuffer.set(temp);
				if(temp==null)
					throw new Exception();
				synchronized(signal)
				{
		//			println("signaling"+transferQueue.peek().name+"  ");
					if(transferQueue.peek()==null)
						throw new Exception();
					isSignaled=true;
					signal.notify();
				}
				return true;
			}
		}
		return false;
	}
	public JavaMemDest pollTransferBuffer() throws Exception
	{
		synchronized(transferQueue)
		{
			if(transferQueue.size()!=0)
			{
				if(transferBuffer!=null)
					freeBufferPool.add(transferBuffer);
				transferBuffer=transferQueue.poll();
				if( transferBuffer==null)
					throw new Exception("wt");
				isSignaled=false;
				return transferBuffer;
			}
		}
		synchronized(signal)
		{
			while(!isSignaled)
			{
				signal.wait();
			}
			isSignaled=false;
			if(transferBuffer!=null)
				freeBufferPool.add(transferBuffer);
			synchronized (transferQueue)
			{
				transferBuffer=transferQueue.poll();
				if( transferBuffer==null)
					throw new Exception("wt");
			}
		}
		return transferBuffer;
	
	}
	public JavaMemDest getTransferBuffer()
	{
		return transferBuffer;
	}
	volatile static int dataCount=0;
	/**
	 * @param args
	 * @throws Throwable 
	 */
	public static void main(String[] args) throws Throwable {
		// TODO Auto-generated method stub
		
		ArrayList<Thread> ths=new ArrayList<Thread>();
		final DropFrameNetworkBufferManager tran=new DropFrameNetworkBufferManager(1);
		
		for(int i=0;i<10;i++)
		{
			Runnable runable=new Runnable(){

				@Override
				public void run() {
					// TODO Auto-generated method stub
					while(true){
					JavaMemDest writeBuf=tran.GetWriteBuffer();
				//	writeBuf.data=dataCount++;

			//		println("writing= "+writeBuf.data+"buffer="+writeBuf.name);
					try {
						Thread.sleep(500);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					Boolean isScheduled = null;
					try
					{
						isScheduled=tran.ScheduleWriteBuffer();
					}
					catch(Throwable wt)
					{
						wt.printStackTrace();
					}
					tran.dump();
			//		println("Scheduled buffer= "+writeBuf.name+" scheduled="+isScheduled.toString());
					}
				}
			};
			Thread th=new Thread(runable);
			ths.add(new Thread(runable));
			th.start();
			Thread.sleep(50);
		}
		int transfercount=0;
		while(true)
		{
			JavaMemDest dest=null;
			
			dest=tran.pollTransferBuffer();
			
		//	println("transfering " +dest.data+"buffer="+dest.name+"transfercount=" +transfercount++);
			Thread.sleep(50);
		}
	}

}
