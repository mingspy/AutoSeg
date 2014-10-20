package com.mingspy.utils;

import java.io.Serializable;
import java.security.InvalidParameterException;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

public class ScoreList<K> implements Serializable
{
    /**
     *
     */
    private static final long serialVersionUID = -3146950441707838385L;
    Map<K, Double> results;

    public ScoreList()
    {

    }

    public int size()
    {
        return results == null ? 0 : results.size();
    }

    public void put(K key, Double value)
    {
        if (results == null) {
            results = new HashMap<K, Double>();
        }

        results.put(key, value);
    }

    public ScoreList(Map<K, Double> results)
    {
        this.results = results;
        if (results == null || results.isEmpty()) {
            throw new InvalidParameterException("Parameter can't be null");
        }
    }

    public Set<K> getKeys()
    {
        return results.keySet();
    }

    public Double getScore(K key)
    {
        return results.get(key);
    }

    public boolean contains(K pid)
    {
        return results.containsKey(pid);
    }

    public boolean constainsAll(Collection<K> c)
    {
        return results.keySet().containsAll(c);
    }

    /**
     * 获取排序的结果
     * @param isAsc true是按升序，否则按降序
     * @return
     */
    public List<Entry<K, Double>> getSortedEntries(boolean isAsc)
    {
        if(isAsc) {
            return new MapSorter<K, Double>().sortByValueASC(results);
        } else {
            return new MapSorter<K, Double>().sortByValueDSC(results);
        }
    }

    public List<Entry<K, Double>> getNormalizeEntries()
    {
        List<Map.Entry<K, Double>> list = new MapSorter<K, Double>().sortByValueASC(results);

        // 归一化得分
        float sum = 0;
        for (Map.Entry<K, Double> entry : list) {
            sum += entry.getValue();
        }

        for (Map.Entry<K, Double> entry : list) {
            entry.setValue(entry.getValue() / sum);
        }

        return list;
    }

    public Set<Entry<K, Double>> getEntries()
    {
        // TODO Auto-generated method stub
        return results == null ? null : results.entrySet();
    }

    public boolean containsSameKey(ScoreList<K> score)
    {
        for (K key : score.results.keySet()) {
            if (results.containsKey(key)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public String toString()
    {
        String str = "ScoreList:{";
        if (results != null) {
            str += "elements:{" + results.toString() + "}";
        }
        str += "}";
        return str;
    }

}
