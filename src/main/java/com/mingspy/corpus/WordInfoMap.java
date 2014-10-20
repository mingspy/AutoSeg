package com.mingspy.corpus;

import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.mingspy.utils.MapSorter;
import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

public class WordInfoMap
{
    private Map<String, WordInfo> wordInfos = new HashMap<String, WordInfo>();
    private int totalFreq = 0;
    public Map<String, WordInfo> getWordInfos()
    {
        return wordInfos;
    }

    public void setWordInfos(Map<String, WordInfo> wordInfos)
    {
        this.wordInfos = wordInfos;
    }

    public int getTotalFreq()
    {
        return totalFreq;
    }

    public void setTotalFreq(int totalFreq)
    {
        this.totalFreq = totalFreq;
    }

    public void addWordInfo(String word,
                            String nature, int frq)
    {
        WordInfo info = wordInfos.get(word);
        if (info == null) {
            info = new WordInfo(word);
        }
        info.addNature(nature, frq);
        wordInfos.put(word, info);
    }

    public void clear()
    {
        for(WordInfo info : wordInfos.values()) {
            info.clear();
        }
        wordInfos.clear();
        totalFreq = 0;
    }

    public void prune( int limit)
    {
        List<String> keys = new ArrayList<String>(1000);
        for (Entry<String, WordInfo> en : wordInfos.entrySet()) {
            if (en.getValue().sumFreq() <= limit) {
                keys.add(en.getKey());
            }
        }

        for (String key : keys) {
            wordInfos.remove(key);
        }
        keys.clear();
    }

    public void writeWordInfo(LineFileWriter writer)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        MapSorter<String, WordInfo> sorter = new MapSorter<String, WordInfo>();
        List<Entry<String, WordInfo>> res = sorter.sortByKeyASC(wordInfos);

        for (Entry<String, WordInfo> en : res) {
            writer.writeLine(en.getValue().toString());
        }
        writer.close();
    }

    public void readWordInfo(LineFileReader reader)
    {
        String line = null;
        while ((line = reader.nextLine()) != null) {
            try {
                int wordIdx = line.indexOf('\t');
                String word = line.substring(0, wordIdx);
                line = line.substring(wordIdx + 1);
                String[] infos = line.split(",");
                for (String inf : infos) {
                    int inIdx = inf.lastIndexOf(":");
                    String nt = inf.substring(0, inIdx);
                    String freqField = inf.substring(inIdx + 1);
                    int freq = Integer.parseInt(freqField);
                    totalFreq += freq;
                    addWordInfo(word, nt, freq);
                    nt = null;
                    freqField = null;
                }

                infos = null;
                word = null;
            } catch(Exception e) {
                System.err.println(line);
                throw new RuntimeException(e);
            }
        }
    }

    public int size()
    {
        return wordInfos.size();
    }
}
