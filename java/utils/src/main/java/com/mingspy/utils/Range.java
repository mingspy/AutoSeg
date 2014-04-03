package com.mingspy.utils;

public class Range {
	public Range(int min, int max){
		if(min > max){
			throw new IllegalArgumentException("Range max < min");
		}
		this.min = min;
		this.max = max;
	}
	private int min;
	private int max;
	public int getMin() {
		return min;
	}
	public void setMin(int min) {
		if(min > max){
			throw new IllegalArgumentException("min to big");
		}
		this.min = min;
	}
	public int getMax() {
		return max;
	}
	public void setMax(int max) {
		if(min > max){
			throw new IllegalArgumentException("max to small");
		}
		this.max = max;
	}
	
}
