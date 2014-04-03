package com.mingspy.jseg;

public class Token {
	public int off;
	public int len;
	//public int attr;
	public Token(int off, int len){
		this.off = off;
		this.len = len;
	}
	
	@Override
	public String toString() {
		return "("+off+","+len+")";
	}
	
}
