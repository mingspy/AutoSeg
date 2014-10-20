package com.mingspy.utils.dat;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * Double array trie是一种用双数组实现的trie树。<br>
 * 节省空间，又能达到trie树的效率。据测试，空间占用率只有普通trie的20%。<br>
 *
 * @see http://linux.thai.net/~thep/datrie/datrie.html
 * @see DoubleArrayTrie: Java implementation by Christos Gioran
 * @author xiuleili
 *
 * @param <V>
 *            存储的值对象。
 */
public abstract interface IDATrie<V> extends Serializable
{
    /**
     * 添加一个词，如果存在，则会更新value值
     *
     * @param key
     *            关键字
     * @param value
     *            值
     * @return true添加成功, false添加失败
     */
    public boolean add(String key, V value);

    /**
     * 移除词。做标记删除。
     *
     * @param key
     *            关键字
     * @return
     */
    public boolean remove(String key);

    /**
     * 获取存在DAT上的value值
     *
     * @param key
     *            关键字
     * @return
     */
    public V find(String key);

    /**
     * 判断是否包含key或者以key开始的前缀。
     *
     * @param key
     *            关键字
     * @return true：包含，false：不存在。
     */
    public boolean containsPrefix(String key);

    public boolean serialize(ObjectOutputStream outStream) throws IOException;

    public boolean  deserialize(ObjectInputStream inStream) throws IOException, ClassNotFoundException;

}
