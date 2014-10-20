package com.mingspy.nlp;

public class CharacterTrie
{
    private static final int MAX_CHILD_NUM = 256; // 用字节保存节点，故最多有256个child.
    private TrieNode Root = new TrieNode();
    private static final int BASE_INDEX = 128; //byte 带有正负号
    private class TrieNode
    {
        public TrieNode[] childs= new TrieNode[MAX_CHILD_NUM];
        public boolean isWordNode = false;
    }

    public boolean addWord(String word)
    {
        byte[] bytes = word.getBytes();
        TrieNode p = Root;
        for(byte c : bytes) {
            int idx = c + BASE_INDEX;
            if(p.childs[idx] == null) {
                p.childs[idx] = new TrieNode();
            }
            p = p.childs[idx];
        }
        p.isWordNode = true;
        return true;
    }

    public boolean contains(String word)
    {
        byte[] bytes = word.getBytes();
        return contains(bytes, 0, bytes.length);
    }

    public boolean contains(byte[] bytes, int start, int end)
    {
        TrieNode p = Root;
        for(int i = start; i< end; i++) {

            int idx = bytes[i] + BASE_INDEX;
            if(p.childs[idx] == null) {
                return false;
            }
            p = p.childs[idx];
        }

        return p.isWordNode;
    }
}
