package com.mingspy.utils.dat.alph;

import java.util.Map;

public class StatisticsAlphabet extends Alphabet
{
    public static final int DEFAULT_CHILDSIZE = 65536;
    public static final float REDUDANCY_RATE = 1.005f;
    private static final long serialVersionUID = 4928760335276657551L;
    private int maxCachedChars = 0;
    private Map<Integer, CharInfo> cachedInfo = null;

    public StatisticsAlphabet(int maxCachedChars, Map<Integer, CharInfo> info)
    {
        this.maxCachedChars = maxCachedChars;
        this.cachedInfo = info;
    }

    public StatisticsAlphabet()
    {

    }

    @Override
    public int childSize(int ch)
    {
        CharInfo info = cachedInfo.get(ch);
        if (info != null && info.childSize != 0) {
            return (int) (info.childSize + 10);
        }
        return DEFAULT_CHILDSIZE;
    }

    @Override
    public int getInnerCode(int ch)
    {
        CharInfo info = cachedInfo.get(ch);
        if (info != null) {
            return info.code;
        }
        if (ch < 256) {
            return ch;
        }
        return ch + maxCachedChars;
    }

}
