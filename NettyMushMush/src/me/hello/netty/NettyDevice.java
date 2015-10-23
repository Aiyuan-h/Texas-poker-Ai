package me.hello.netty;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Executors;

import me.hello.netty.Timer.TimeArrive;

import org.jboss.netty.bootstrap.ClientBootstrap;
import org.jboss.netty.channel.Channel;
import org.jboss.netty.channel.ChannelFuture;
import org.jboss.netty.channel.ChannelHandlerContext;
import org.jboss.netty.channel.ChannelPipeline;
import org.jboss.netty.channel.ChannelPipelineFactory;
import org.jboss.netty.channel.ChannelStateEvent;
import org.jboss.netty.channel.Channels;
import org.jboss.netty.channel.MessageEvent;
import org.jboss.netty.channel.SimpleChannelHandler;
import org.jboss.netty.channel.socket.nio.NioClientSocketChannelFactory;
import org.jboss.netty.handler.codec.string.StringDecoder;
import org.jboss.netty.handler.codec.string.StringEncoder;


public class NettyDevice {
	static Channel mChannel = null;
	static Timer mTimer = null;
	
	static OutgoingDataAssembler encode = null;//打包
	static IncomingDataParser decode = null;//解包
	static Map<String,String> map = null;
	
    public static void main(String[] args) {
    	init();
        // Configure the client.
        ClientBootstrap bootstrap = new ClientBootstrap(new NioClientSocketChannelFactory(Executors.newCachedThreadPool(), Executors.newCachedThreadPool()));

        // Set up the default event pipeline.
        bootstrap.setPipelineFactory(new ChannelPipelineFactory() {
            @Override
            public ChannelPipeline getPipeline() throws Exception {
                return Channels.pipeline(new StringDecoder(), new StringEncoder(), new DeviceHandler());
            }
        });

        // Start the connection attempt.
        ChannelFuture future = bootstrap.connect(new InetSocketAddress("localhost", 8000));

        // Wait until the connection is closed or the connection attempt fails.
        future.getChannel().getCloseFuture().awaitUninterruptibly();

        // Shut down thread pools to exit.
        bootstrap.releaseExternalResources();
    }

    private static void init() {
    	encode = new OutgoingDataAssembler();
    	decode = new IncomingDataParser();
    	map = new HashMap<String, String>();
    	for(int i = 1; i <= 5; i++) {
    		map.put(Integer.toString(i), State.off.toString());
    	}
    }
    
    private static class DeviceHandler extends SimpleChannelHandler {

        @Override
        public void messageReceived(ChannelHandlerContext ctx, MessageEvent e) throws Exception {
            if (e.getMessage() instanceof String) {
                String message = (String) e.getMessage();
                System.out.println(message);
            }

            super.messageReceived(ctx, e);
        }

        @Override
        public void channelConnected(ChannelHandlerContext ctx, ChannelStateEvent e) throws Exception {
            System.out.println("已经与Server建立连接。。。。");   
            super.channelConnected(ctx, e);
            
            mChannel = e.getChannel();
            startTimer();
        }
    }
    
    private static void startTimer() {//启动定时器
    	mTimer = new Timer(new TimeArrive() {
			
			@Override
			public void callBack() {//发送心跳包。
				encode.clear();
				encode.addValue(ProtocolKey.Command, ProtocolKey.Ping);
				Set<String> keySet = map.keySet();
				Iterator<String> it = keySet.iterator();
				while(it.hasNext()) {
					String key = it.next();
					String value = map.get(key);
					encode.addValue(key, value);
				}		
				String s = encode.getProtocolText();
				mChannel.write(s);		
				System.out.println(s);
			}
		});
    }
}
