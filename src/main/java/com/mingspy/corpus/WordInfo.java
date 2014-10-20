package com.mingspy.corpus;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;


public class WordInfo
{
    public static final String FIELD_TOTAL="FTL";
    public static final String FIELD_TWOFREQ="FBI";
    private String word;
    private Map<String, Integer> natures = new HashMap<String,Integer>();

    public WordInfo(String word)
    {
        this.word = word;
    }

    public void addNature(String nature)
    {
        addNature(nature, 1);
    }

    public void addNature(String nature, int freq)
    {
        Integer times = natures.get(nature);
        if(times == null) {
            times = new Integer(freq);
        } else {
            times += freq;
        }

        natures.put(nature, times);
    }

    @Override
    public String toString()
    {
        String str = word + "\t";
        for(Entry<String,Integer> en : natures.entrySet()) {
            str +=en.getKey().toLowerCase()+":"+en.getValue()+",";
        }
        //str +=FIELD_TOTAL+":"+sumFreq()+",";
        return str;
    }

    public int sumFreq()
    {
        int sum = 0;
        for(Integer i : natures.values()) {
            sum += i;
        }
        return sum;
    }

    public void clear()
    {
        natures.clear();
    }

}
