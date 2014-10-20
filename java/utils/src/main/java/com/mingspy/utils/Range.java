package com.mingspy.utils;

public class Range
{
    public Range(int start, int end)
    {
        if(start > end) {
            throw new IllegalArgumentException("Range max < min");
        }
        this.start = start;
        this.end = end;
    }
    private int start;
    private int end;
    public int getStart()
    {
        return start;
    }
    public void setStart(int start)
    {
        this.start = start;
    }
    public int getEnd()
    {
        return end;
    }
    public void setEnd(int end)
    {
        this.end = end;
    }

    public int length()
    {
        return  end - start;
    }

}
