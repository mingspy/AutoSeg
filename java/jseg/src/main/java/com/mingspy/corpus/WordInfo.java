package com.mingspy.corpus;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;


public class WordInfo {
	public static final String FIELD_TOTAL="FREQTOL";
	public static final String FIELD_TWOFREQ="FREQTWO";
	private String word;
	private Map<String, Integer> natures = new HashMap<String,Integer>();
	
	public WordInfo(String word){
		this.word = word;
	}
	
	public void addNature(String nature){
		Integer times = natures.get(nature);
		if(times == null){
			times = new Integer(1);
		}else{
			times ++;
		}
		
		natures.put(nature, times);
	}

	@Override
	public String toString() {
		String str = word + "\t";
		for(Entry<String,Integer> en : natures.entrySet()){
			str +=en.getKey()+":"+en.getValue()+",";
		}
		//str +=FIELD_TOTAL+":"+sumFreq()+",";
		return str;
	}
	
	public int sumFreq(){
		int sum = 0;
		for(Integer i : natures.values()){
			sum += i;
		}
		return sum;
	}
	
}
