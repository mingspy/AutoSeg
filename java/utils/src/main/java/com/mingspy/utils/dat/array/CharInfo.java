package com.mingspy.utils.dat.array;

import java.io.Serializable;

public class CharInfo implements Serializable {
	private static final long serialVersionUID = -2767803201507141433L;
	public int ch;
	public int code;
	public int childSize;

	@Override
	public String toString() {
		StringBuilder builder = new StringBuilder();
		builder.append((char) (ch));
		builder.append(" ");
		builder.append(code);
		builder.append(" ");
		builder.append(childSize);
		return builder.toString();
	}

}
