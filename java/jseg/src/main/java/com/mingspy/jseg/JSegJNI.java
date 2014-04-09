package com.mingspy.jseg;

import java.util.List;
import java.util.ResourceBundle;

public abstract class JSegJNI {
	static{
		ResourceBundle bundle = ResourceBundle.getBundle("jseg");
		System.out.println(System.getProperty("java.library.path"));
		System.loadLibrary("JSegJNI");
	}
	
	/**
	 * Forward max match split.
	 * @param str
	 * @return
	 */
	public static native List<Token> MaxSplit(String str);
	
	/**
	 * ForWard full match split.
	 * @param str
	 * @return
	 */
	public static native List<Token> FullSplit(String str);
	
	/**
	 * Using one gram split.
	 * @param str
	 * @return
	 */
	public static native List<Token> UniGramSplit(String str);
	
	public static native List<Token> BiGramSplit(String str);
	
	public static native List<Token> MixSplit(String str);
	
	public static native void Test(String str);

}
