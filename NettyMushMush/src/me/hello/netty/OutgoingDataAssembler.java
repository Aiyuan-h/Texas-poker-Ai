package me.hello.netty;

import java.util.ArrayList;


public class OutgoingDataAssembler {
	
    private ArrayList<String> m_protocolText;

    public OutgoingDataAssembler() {
        m_protocolText = new ArrayList<String>();
    }

    public void clear() {
        m_protocolText.clear();
    }

    public String getProtocolText() {
        StringBuilder tmpStr = new StringBuilder();
        for (int i = 0; i < m_protocolText.size(); i++) {
        	if(i != 0) {
        		tmpStr.append(ProtocolKey.returnWrap);
        	}
        	tmpStr.append(m_protocolText.get(i));
        }
        return tmpStr.toString();
    }
    
    public void addValue(String protocolKey, String value) {
        m_protocolText.add(protocolKey + ProtocolKey.equalSign + value);
    }

}
