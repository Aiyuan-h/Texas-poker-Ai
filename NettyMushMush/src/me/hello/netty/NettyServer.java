package me.hello.netty;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Executors;

import org.jboss.netty.bootstrap.ServerBootstrap;
import org.jboss.netty.channel.Channel;
import org.jboss.netty.channel.ChannelHandlerContext;
import org.jboss.netty.channel.ChannelPipeline;
import org.jboss.netty.channel.ChannelPipelineFactory;
import org.jboss.netty.channel.ChannelStateEvent;
import org.jboss.netty.channel.Channels;
import org.jboss.netty.channel.ExceptionEvent;
import org.jboss.netty.channel.MessageEvent;
import org.jboss.netty.channel.SimpleChannelHandler;
import org.jboss.netty.channel.socket.nio.NioServerSocketChannelFactory;
import org.jboss.netty.handler.codec.string.StringDecoder;
import org.jboss.netty.handler.codec.string.StringEncoder;


public class NettyServer {
	static Channel deviceChannel = null;//如果客户端多，可以放在容器里。
	static Channel appChannel = null;
	static OutgoingDataAssembler encode = null;//打包
	static IncomingDataParser decode = null;//解包
	static Map<String,String> map = null;//key:开关编号(1-5)   value:开关状态，0：关     1：开
	
	
    public static void main(String[] args) {
    	init();
    	startListenerDevice();
    	startListenerApp();
    }

    private static void init() {
    	encode = new OutgoingDataAssembler();
    	decode = new IncomingDataParser();
    	map = new HashMap<String, String>();	
    }
    
    private static void startListenerDevice() {
        ServerBootstrap bootstrap = new ServerBootstrap(new NioServerSocketChannelFactory(Executors.newCachedThreadPool(), Executors.newCachedThreadPool()));

        // Set up the default event pipeline.
        bootstrap.setPipelineFactory(new ChannelPipelineFactory() {
            @Override
            public ChannelPipeline getPipeline() throws Exception {
                return Channels.pipeline(new StringDecoder(), new StringEncoder(), new DeviceHandler());
            }
        });

        // Bind and start to accept incoming connections.
        Channel bind = bootstrap.bind(new InetSocketAddress(8000));
        System.out.println("Server已经启动，监听端口: " + bind.getLocalAddress() + "， 等待Device连接。。。");
    }
    
    private static void startListenerApp() {
        ServerBootstrap bootstrap = new ServerBootstrap(new NioServerSocketChannelFactory(Executors.newCachedThreadPool(), Executors.newCachedThreadPool()));

        bootstrap.setPipelineFactory(new ChannelPipelineFactory() {
            @Override
            public ChannelPipeline getPipeline() throws Exception {
                return Channels.pipeline(new StringDecoder(), new StringEncoder(), new AppHandler());
            }
        });

        Channel bind = bootstrap.bind(new InetSocketAddress(8888));
        System.out.println("Server已经启动，监听端口: " + bind.getLocalAddress() + "， 等待APP连接。。。");
    }
    
    private static void printMap() {
    	Set<String> set = map.keySet();
    	Iterator<String> it = set.iterator();
    	while(it.hasNext()) {
    		String key = it.next();
    		String value = map.get(key);
    		System.out.println(key + "=" + value);
    	}
    	System.out.println();//换行
    }
    
    private static class DeviceHandler extends SimpleChannelHandler {
        @Override
        public void messageReceived(ChannelHandlerContext ctx, MessageEvent e) throws Exception {
            if (e.getMessage() instanceof String) {
                String message = (String) e.getMessage();
                decode.decodeProtocolText(message);
                List<String> keys = decode.getKeyList();
                List<String> values = decode.getValuesList();
                map.clear();
                for(int i = 1; i < keys.size(); i++) {
                	map.put(keys.get(i), values.get(i));
                }
                System.out.println("Service 收到 来自Device的心跳包");
                printMap();
            }

            super.messageReceived(ctx, e);
        }

        @Override
        public void exceptionCaught(ChannelHandlerContext ctx, ExceptionEvent e) throws Exception {
            super.exceptionCaught(ctx, e);
        }

        @Override
        public void channelConnected(ChannelHandlerContext ctx, ChannelStateEvent e) throws Exception {          
            super.channelConnected(ctx, e);           
            deviceChannel = e.getChannel();    
        }
    }
    
    private static class AppHandler extends SimpleChannelHandler {
        @Override
        public void messageReceived(ChannelHandlerContext ctx, MessageEvent e) throws Exception {
            if (e.getMessage() instanceof String) {
                String message = (String) e.getMessage();
                decode.decodeProtocolText(message);
                List<String> keys = decode.getKeyList();
                List<String> values = decode.getValuesList();
                for(int i = 0; i < keys.size(); i++) {
                	map.put(keys.get(i), values.get(i));
                }				
            }

            super.messageReceived(ctx, e);
        }

        @Override
        public void exceptionCaught(ChannelHandlerContext ctx, ExceptionEvent e) throws Exception {
            super.exceptionCaught(ctx, e);
        }

        @Override
        public void channelConnected(ChannelHandlerContext ctx, ChannelStateEvent e) throws Exception {          
            super.channelConnected(ctx, e);           
            appChannel = e.getChannel();    
        }
    }
}
