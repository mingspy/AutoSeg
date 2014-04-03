package com.mingspy.jseg;

import java.util.ArrayList;
import java.util.List;

public class AutoTokenizer implements ITokenizer {

	@Override
	public List<String> maxSplit(String str) {
		if(str == null || str.isEmpty()){
			return null;
		}
		
		List<Token> res = JSegJNI.MaxSplit(str);
		return genWords(str, res);
	}


	@Override
	public List<String> fullSplit(String str) {
		if(str == null || str.isEmpty()){
			return null;
		}
		
		List<Token> res = JSegJNI.FullSplit(str);
		return genWords(str, res);
	}

	@Override
	public List<String> oneGramSplit(String str) {
		if(str == null || str.isEmpty()){
			return null;
		}
		
		List<Token> res = JSegJNI.OneGramSplit(str);
		return genWords(str, res);
	}
	
	private List<String> genWords(String str, List<Token> res) {
		ArrayList<String> words = new ArrayList<String>(res.size());
		for(Token t:res){
			words.add(str.substring(t.off, t.off + t.len));
		}
		return words;
	}
	
	public static void main(String[] args) {
		AutoTokenizer tokenizer = new AutoTokenizer();
		System.out.println(tokenizer.maxSplit("他说的确实在理"));
		System.out.println(tokenizer.fullSplit("他说的确实在理"));
		System.out.println(tokenizer.oneGramSplit("他说的确实在理"));
	}
}
