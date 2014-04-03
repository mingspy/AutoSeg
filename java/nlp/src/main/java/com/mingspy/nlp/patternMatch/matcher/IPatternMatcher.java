package com.mingspy.nlp.patternMatch.matcher;

import com.mingspy.nlp.patternMatch.types.Pattern;
import com.mingspy.utils.ScoreList;

public interface IPatternMatcher {
	ScoreList<Long> match(Pattern target);
	Pattern getPattern(long id);
}
