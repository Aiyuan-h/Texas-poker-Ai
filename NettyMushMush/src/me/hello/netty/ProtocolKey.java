package me.hello.netty;

public class ProtocolKey {
	public static final String equalSign = "=";
	public static final String returnWrap = "\r\n";
	public static final String Command = "command";
	public static final String Operate = "operate";//操作开关
	public static final String Query = "query";//查询
	public static final String Ping = "ping";//心跳
	public static final String Verify = "verify";//Device自检
	public static final String Restart = "restart";//Device重启
	public static final String Shutdown = "shutdown";//Device关机
}
