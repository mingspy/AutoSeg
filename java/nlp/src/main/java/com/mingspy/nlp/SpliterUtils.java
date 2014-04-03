package com.mingspy.nlp;

import java.util.List;

import com.mingspy.jseg.AutoTokenizer;
import com.mingspy.jseg.ITokenizer;

public class SpliterUtils {
	public static ITokenizer _tokenizer = new AutoTokenizer();
	public static void setTokenizer(ITokenizer tokenizer){
		_tokenizer = tokenizer;
	}
	public static List<String> split(String str){
		return _tokenizer.oneGramSplit(str);
	}
}
