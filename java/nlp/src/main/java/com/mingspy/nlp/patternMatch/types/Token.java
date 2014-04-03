package com.mingspy.nlp.patternMatch.types;

import java.io.Serializable;

public class Token implements Serializable{
	/**
	 * 
	 */
	private static final long serialVersionUID = 6462691494404880919L;
	private String word;		// 分词
	private int flag;			// 分词标记
	private String nature;		//词性
	private String category;	// 词类
	public Token(){
		
	}
	
	public Token(int flag, String word, String cat, int webid ,int parentid){
		this.flag = flag;
		this.word = word;
		this.category = cat;
	}
	
	
	public Token(String word){
		this.word = word;
	}
	public String getWord() {
		return word;
	}
	public void setWord(String word) {
		this.word = word;
	}
	public int getFlag() {
		return flag;
	}
	public String getCategory() {
		return category;
	}
	public void setCategory(String category) {
		this.category = category;
	}
	public void setFlag(int flag) {
		this.flag = flag;
	}
	public String getNature() {
		return nature;
	}

	public void setNature(String nature) {
		this.nature = nature;
	}

	@Override
	public String toString() {
		StringBuilder s = new StringBuilder();
		s.append(word);
		s.append("[");
		s.append(flag);
		s.append(",");
		s.append(category);
		s.append(",");
		s.append(nature);
		s.append("]");
		
		return s.toString();
	}
	
	/**
	 * 判断该item是否代表一个实体词
	 * @return
	 */
	public boolean isNotional() {
		return false;
	}
}
