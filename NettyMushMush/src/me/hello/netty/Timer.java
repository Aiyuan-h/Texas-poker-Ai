package me.hello.netty;

public class Timer implements Runnable{
	private boolean flag = true;//计时开始。
	private int delay = 10000;//10秒
	private int count = 0; //每十秒钟发一次，发10次后，没1秒钟发一次。
	private TimeArrive t = null;
	
	public Timer(TimeArrive _t) {
		this.t = _t;
		new Thread(this).start();
	}
		
	public void stop() {
		flag = false;
	}
	
	public void verify() {//自检时，校正
		delay = 10000;
		count = 0;
	}
	
	@Override
	public void run() {
		while(flag) {//重启和关机时，要调用。
			System.out.println("Device：心跳包：" + count);
			t.callBack();
			try {
				Thread.sleep(delay);
				if(count > 10) {
					delay = 1;//每毫秒发一次
				} else {
					count++;
				}
				//TODO 发送心跳包
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
	}
	
	public interface TimeArrive {
		public void callBack();
	}
}
