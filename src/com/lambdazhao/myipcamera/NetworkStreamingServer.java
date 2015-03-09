package com.lambdazhao.myipcamera;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import com.lambdazhao.common.NetworkProfiler;
import com.lambdazhao.common.TimeProfile;
import com.lambdazhao.common.media.DropFrameNetworkBufferManager;
import com.lambdazhao.myipcamera.libjpeg.JavaMemDest;

;

public class NetworkStreamingServer {
	DropFrameNetworkBufferManager bufferManager = null;
	Thread th;

	public NetworkStreamingServer(DropFrameNetworkBufferManager manager) {
		this.bufferManager = manager;

	}

	NetworkProfiler networkProfile = new NetworkProfiler("MyIpCamera",
			"Video Transafer");

	ServerSocket socket = null;
	Socket client = null;
	volatile boolean isStopped = true;
	public JavaMemDest GetWriteBuffer()
	{
		return bufferManager.GetWriteBuffer();
	}
	public  boolean ScheduleWriteBuffer() throws Exception
	{
		return bufferManager.ScheduleWriteBuffer();
	}
	public void start() {
		isStopped = false;
		Runnable runable = new Runnable() {

			@Override
			public void run() {
				// TODO Auto-generated method stub
				try {
					socket = new ServerSocket(8081);

					client = socket.accept();

					PrintWriter pw = new PrintWriter(client.getOutputStream());
					pw.print("HTTP/1.1 " + 200 + " \r\n");
					pw.print("Connection: close\r\n");
					pw.write("Content-Type: multipart/x-mixed-replace;boundary=Ba4oTvQMY8ew04N8dcnM\r\n");
					pw.flush();

					while (!isStopped) {
						TimeProfile profileNetwork = TimeProfile.Start(
								"MyIpCamera", "jpegCompress");
						JavaMemDest memDest = bufferManager
								.pollTransferBuffer();
						byte[] tmp = memDest.GetBuffer();
						pw.write("\r\n--Ba4oTvQMY8ew04N8dcnM\r\nContent-Type: image/jpeg\r\n\r\n");
						pw.flush();
						client.getOutputStream().write(tmp, 0,
								memDest.GetOutSize());
						networkProfile.AddVolume(memDest.GetOutSize());
						profileNetwork.End();
					}
				} catch (IOException e) {
					e.printStackTrace();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}

		};
		th = new Thread(runable);
		th.start();
	}
}
