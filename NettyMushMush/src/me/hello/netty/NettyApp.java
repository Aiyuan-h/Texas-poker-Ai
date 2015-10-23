package me.hello.netty;

import java.net.InetSocketAddress;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.Executors;

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


public class NettyApp {
	static Channel mChannel = null;
	static OutgoingDataAssembler encode = null;//打包
	static IncomingDataParser decode = null;//解包
	static Map<String,String> map = null;//key:开关编号(1-5)   value:开关状态，0：关     1：开
	
    public static void main(String[] args) {
    	init();
    	startConnectServer();
    }  
    
    private static void init() {
    	encode = new OutgoingDataAssembler();
    	decode = new IncomingDataParser();
    	map = new HashMap<String, String>();	
    }
    
    private static void startConnectServer() {
        ClientBootstrap bootstrap = new ClientBootstrap(new NioClientSocketChannelFactory(Executors.newCachedThreadPool(), Executors.newCachedThreadPool()));
        bootstrap.setPipelineFactory(new ChannelPipelineFactory() {
            @Override
            public ChannelPipeline getPipeline() throws Exception {
                return Channels.pipeline(new StringDecoder(), new StringEncoder(), new AppHandler());
            }
        });

        ChannelFuture future = bootstrap.connect(new InetSocketAddress("localhost", 8888));
        future.getChannel().getCloseFuture().awaitUninterruptibly();
        bootstrap.releaseExternalResources();
    }
    
	public void query() {
		encode.clear();
		encode.addValue(ProtocolKey.Command, ProtocolKey.Query);
		String s = encode.getProtocolText();
		mChannel.write(s);
	}
	
	public void operateSwitch(String switchKey, State s) {
		encode.clear();
		encode.addValue(ProtocolKey.Command, ProtocolKey.Operate);
		encode.addValue(switchKey, s.toString());
		String text = encode.getProtocolText();
		mChannel.write(text);
	}
    
    private static class AppHandler extends SimpleChannelHandler {

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
        }
    }
    
}
