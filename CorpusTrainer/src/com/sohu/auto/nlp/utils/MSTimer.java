package com.sohu.auto.nlp.utils;

/**
 * A timer used to statistic program time elapse in Milli-seconds.
 * @author xiuleili
 *
 */
public class MSTimer {
	private long start_time = System.currentTimeMillis();
	private long end_time = 0;
	public void start(){
		start_time = System.currentTimeMillis();
	}
	
	public long elapsed(){
		end_time = System.currentTimeMillis();
		return end_time - start_time;
	}

	@Override
	public String toString() {
		return "elapsed:"+elapsed()+" ms.";
	}
	
	
}
