package me.hello.netty;

import java.util.ArrayList;

public class IncomingDataParser {//要考虑线程同步

    public String m_command;
    private ArrayList<String> m_names;
    private ArrayList<String> m_values;
    
    public IncomingDataParser() {
        m_names = new ArrayList<String>();
        m_values = new ArrayList<String>();
    }
    
    public void decodeProtocolText(String protocolText) {
    	m_names.clear();
    	m_values.clear();
    	int speIndex = protocolText.indexOf(ProtocolKey.returnWrap);
    	if (speIndex < 0) {
    		return ;
    	} else {
    		String[] tmpNameValues = protocolText.split(ProtocolKey.returnWrap);
            for (int i = 0; i < tmpNameValues.length; i++) {
                String[] tmpStr = tmpNameValues[i].split(ProtocolKey.equalSign );
                if (tmpStr.length == 2) {
                    m_names.add(tmpStr[0]);
                    m_values.add(tmpStr[1]);	               
                }
            }
    	} 	
    }
 
    public ArrayList<String> getKeyList() {
    	return m_names;
    }
    
    public ArrayList<String> getValuesList() {
    	return m_values;
    }
}
