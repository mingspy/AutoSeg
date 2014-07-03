package com.mingspy.utils;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;

public class WebUtils {
	/**
	 * 实现web的GET操作
	 * @param uriStr 待访问的web url
	 * @return 读取的web页面内容。
	 * @throws IOException
	 */
	public static String GET(String uriStr) throws IOException{
		
		URL  url=new URL(uriStr);
		url.openConnection();		
		BufferedReader bufferReader = null;
		StringBuffer sb=new StringBuffer();
		try {  
            bufferReader = new BufferedReader(new InputStreamReader(url.openConnection().getInputStream()));         
            String line = null;  
            while ((line = bufferReader.readLine()) != null) {
                sb.append(line);
                sb.append("\n");
            }  
        } finally {  
            bufferReader.close();
        }		
		
		return sb.toString();
	}
	
	public static void main(String args[]) throws IOException{
		System.out.println(WebUtils.GET("http://www.baidu.com"));
	}
}
