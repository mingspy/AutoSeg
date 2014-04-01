package com.mingspy.utils.io;

import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.Collection;

public class LineFileWriter {
	private BufferedWriter writer = null;

	public LineFileWriter(String path) throws UnsupportedEncodingException, FileNotFoundException {
		this(path,"utf-8");
	}

	public LineFileWriter(String path, String coding) throws UnsupportedEncodingException, FileNotFoundException {
			writer = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(path), coding));
	}
	
	public void writeLine(String line){
		try {
			writer.write(line);
			writer.write("\n");
		} catch (IOException e) {
			if(writer == null){
				System.out.println("File has been closed.");
			}
			e.printStackTrace();
		}	
	}

	public void writeLine(Collection<?> coll){
		int count = 0;
		for(Object o : coll){
		
			writeLine(o.toString());
			if((++count % 100) == 0){
				try {
					writer.flush();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
	public void close(){
		if(writer != null){
			try {
				writer.flush();
				writer.close();
				writer = null;
			}catch(Exception e){
				
			}
		}
	}
	@Override
	protected void finalize() throws Throwable {
		close();
		super.finalize();
	}
	

}
