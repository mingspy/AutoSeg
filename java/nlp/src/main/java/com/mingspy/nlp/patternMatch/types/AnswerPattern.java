package com.mingspy.nlp.patternMatch.types;



public class AnswerPattern extends Pattern
{

    public static final String Topic = "[$A]";
    public AnswerPattern(long id, int score, String patternstr, String topic)
    {
        super(id, score, patternstr, topic);
    }

}
