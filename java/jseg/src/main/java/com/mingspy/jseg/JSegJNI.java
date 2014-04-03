package com.mingspy.jseg;

import java.util.List;
import java.util.ResourceBundle;

public abstract class JSegJNI {
	static{
		ResourceBundle bundle = ResourceBundle.getBundle("jseg");
		System.out.println(System.getProperty("java.library.path"));
		System.loadLibrary("JSegJNI");
		SetDictFolder(bundle.getString("DICT_FOLDER"));
	}
	/**
	 * Set the dictionary folder
	 * @param path the folder's path.
	 * @return 
	 */
	public static native boolean SetDictFolder(String path);
	
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
	public static native List<Token> OneGramSplit(String str);
	
	public static native void Test(String str);

}
