package com.sohu.auto.nlp.utils.io;



import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

/**
 * 行文本文件读取器。<br>
 * 按行读取文件，跳过注释行，空行，并对每行进行trim()操作 <br>
 * 默认的注释为#，//，块注释(/*, * /)。<br>
 * 可以通过clearAnnotations()清空注释标记。<br>
 * 通过addAnnotation()添加注释标记。
 * 
 * 
 * @author xiuleili
 * 
 */
public class LineFileReader {
	private List<String> annotations = new ArrayList<String>();
	private BufferedReader reader = null;

	public LineFileReader(String path) {
		this(path,"utf-8");
	}

	public LineFileReader(String path, String coding) {
		try {
			reader = new BufferedReader(new InputStreamReader(new FileInputStream(path), coding));
			addDefaultAnotation();
		} catch (Exception e) {
			reader = null;
		}
		
	}

	private void addDefaultAnotation() {
		annotations.add("#");
		annotations.add("//");
		annotations.add("/*");
		annotations.add("*/");
	}

	/**
	 * 添加注释字符串。在开头出现该字符串，则认为是注释行。
	 * 
	 * @param ch
	 */
	public void addAnnotation(String ch) {
		if (!annotations.contains(ch)) {
			annotations.add(ch);
		}
	}

	/**
	 * 清空注释字符集。
	 */
	public void clearAnnotations() {
		annotations.clear();
	}

	/**
	 * 从文件中读取一行内容。注释和空行将会被跳过。
	 * @return
	 */
	public String nextLine() {
		String line = null;
		if (reader != null) {
			try {
				boolean isblocking = false;
				while (true) {
					line = reader.readLine();
					if (line == null) {
						break;
					}

					line = line.trim();
					if (line.isEmpty()) {
						continue;
					}

					if(isblocking){
						if(line.endsWith("*/")){
							isblocking = false;
						}
						continue;
					}
					
					boolean isAnnotation = false;
					for (String ch : annotations) {
						if (line.startsWith(ch)) {
							isAnnotation = true;
							if(ch.equals("/*")){
								isblocking = true;
							}
							break;
						}
					}

					if (!isAnnotation && !isblocking) {
						break;
					}
				}
			} catch (IOException e) {
				e.printStackTrace();
				line = null;
			}
		}
		return line;
	}
	
	public void close(){
		if(reader != null){
			try{
				reader.close();
				reader = null;
			}catch(Exception e){
				
			}
		}
	}

}
