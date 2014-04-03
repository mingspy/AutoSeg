package com.mingspy.jseg;

import java.util.List;

public interface ITokenizer {
	List<String> maxSplit(String str);
	List<String> fullSplit(String str);
	List<String> oneGramSplit(String str);
}
