package com.mingspy.jseg;

import java.util.List;

public interface ITokenizer {
	List<String> maxSplit(String str);
	List<String> fullSplit(String str);
	List<String> uniGramSplit(String str);
	List<String> biGramSplit(String str);
	List<String> mixSplit(String str);
	List<Token> POSTagging(String str);
}
