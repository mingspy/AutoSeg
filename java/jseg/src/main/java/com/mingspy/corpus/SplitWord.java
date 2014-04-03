package com.mingspy.corpus;

public class SplitWord {
	public SplitWord(String word, String flag) {
		this.word = word;
		this.flag = flag;
	}

	private String word;
	private String flag;

	public String getWord() {
		return word;
	}

	public void setWord(String word) {
		this.word = word;
	}

	public String getFlag() {
		return flag;
	}

	public void setFlag(String flag) {
		this.flag = flag;
	}

	@Override
	public String toString() {
		return word + "/" + flag;
	}
	
	
}
