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
	public List<String> uniGramSplit(String str) {
		if(str == null || str.isEmpty()){
			return null;
		}
		
		List<Token> res = JSegJNI.UniGramSplit(str);
		return genWords(str, res);
	}
	
	private List<String> genWords(String str, List<Token> res) {
		ArrayList<String> words = new ArrayList<String>(res.size());
		for(Token t:res){
			words.add(str.substring(t.off, t.off + t.len));
		}
		return words;
	}
	

	@Override
	public List<String> biGramSplit(String str) {
		if(str == null || str.isEmpty()){
			return null;
		}
		
		List<Token> res = JSegJNI.BiGramSplit(str);
		return genWords(str, res);
	}


	@Override
	public List<String> mixSplit(String str) {
		if(str == null || str.isEmpty()){
			return null;
		}
		
		List<Token> res = JSegJNI.MixSplit(str);
		return genWords(str, res);
	}
	
	public static void main(String[] args) {
		AutoTokenizer tokenizer = new AutoTokenizer();
		
		System.out.println(tokenizer.maxSplit("他说的确实在理"));
		System.out.println(tokenizer.fullSplit("他说的确实在理"));
		System.out.println(tokenizer.uniGramSplit("他说的确实在理"));
		System.out.println(tokenizer.biGramSplit("他说的确实在理"));
		System.out.println(tokenizer.mixSplit("他说的确实在理"));
	}


}
