package com.mingspy.nlp.spam;

public interface ISpamFilter
{
    String getFilterName();
    boolean isSpam(String doc);
    double filter(String doc);
    boolean isSpamProb(double prob);
    double getSpamThreshold();
}
