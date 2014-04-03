package com.mingspy.nlp.patternMatch.matcher;

import java.io.Serializable;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import com.mingspy.nlp.patternMatch.types.Pattern;
import com.mingspy.nlp.patternMatch.types.QuestionPattern;
import com.mingspy.utils.ScoreList;

public class PatternTrie implements Serializable {
	private static final long serialVersionUID = 2239010188663099238L;
	private TrieNode root = null;
	public class TrieNode implements Serializable
    {
		private static final long serialVersionUID = -4277280900898555052L;
		public long id;
        public String key;
        public List<TrieNode> childs;
        public TrieNode parent;
        public double score;
        public double patternScore;
        public TrieNode(long id, String key, List<TrieNode> childs, TrieNode parent){
        	this.id = id;
        	this.key = key;
        	this.childs = childs;
        	this.parent = parent;
        }
		@Override
		public String toString() {
			return "{"+key+" id:"+id+" score:"+score+" pscore:"+patternScore+"}";
		}
    }
	
	public boolean add(Pattern p)
    {
        if (root == null)
        {
            root = new TrieNode(0,"root", null, null ) ;
        }

        return add(root, p, 0);
    }

    private boolean add(TrieNode item, Pattern p, int i)
    {     
        if (item.childs == null)
        { 
            item.childs = new LinkedList<TrieNode>();
        }

        TrieNode next = null;
        String key = p.getItem(i).getKey();
        for (TrieNode child : item.childs)
        {
            if (child.key.equalsIgnoreCase(key))
            {
                next = child;
                break;
            }
        }

        if (next == null)
        {
            next  = new TrieNode(0, key, null, item) ;
            next.score = p.getItem(i).getScore();
            item.childs.add(next);
        }

        if (i == p.itemsSize() - 1)
        {
            if (next.id != 0)
            {
                return false; // already has.
            }
            next.id = p.getPatternId();
            next.patternScore = p.getScore();
            return true;
        }

        return add(next, p, i+1);
    }
    
    public ScoreList<Long> match(Pattern p){
    	if (root != null)
        {
    		Map<Long, Double> result = new HashMap<Long, Double>();
            for (int i = 0; i < p.itemsSize(); i++)
            {
                match(root, p, i, result);
            }
            
            if(result.size() != 0){
            	return new ScoreList<Long>(result);
            }
                
        }

        return null;
    }
    
    private boolean match(TrieNode item, Pattern p, int i, Map<Long, Double> result)
    {
        // 调用此方法，说明item.val == p.items[i - 1]
        // 只要判断item的孩子是否等于p.items[i];
        
        if (item.childs == null) return true;
        if(i == p.itemsSize()) return false;

        boolean starFound = false;
        boolean keyFound = false;
        boolean keyMatched = false;
        boolean starMatched = false;
        String patternKey = p.getItem(i).getKey();
        for (TrieNode child : item.childs)
        {
            String childKey = child.key;
            if (!childKey.equals("*"))
            {
                // found the key.
                if (!keyFound && isEqual(childKey, patternKey)) {
                	// 特殊处理[CAR] 和 [CAR_*
                	if(!childKey.startsWith("[CAR"))
                		keyFound = true;
                    if (child.id != 0) // met a pattern.
                    {
                    	double score = calcScore(child);
                    	result.put(child.id, score);
                    	keyMatched = true;
                    }
                    

                    // check the longer patterns.
                    // children are longer than parent.
                    if( match(child, p, i + 1, result)){
                    	keyMatched = true;
                    }
                }
            }
            else
            {
                if (child.id != 0)
                {
                	double score = calcScore(child);
                	result.put(child.id, score);
                	starMatched = true;
                }
                starFound = true;
                if(matchStar(child, p, i, result)){
                	starMatched = true;
                }
            }

            if (keyFound && starFound)
            {
                break;
            }
        }

        return starMatched || keyMatched;
    }
    
    private boolean isEqual(String itemkey, String patternkey){
    	if(itemkey.equalsIgnoreCase(patternkey)){
    		return true;
    	}else if(itemkey.equals("[CAR]")&&patternkey.startsWith("[CAR_")){
    		return true;
    	}
    	
    	return false;
    }
    
    private double calcScore(TrieNode child) {
		double score = child.patternScore * Constants.BASE_SCORE;
		while(child.parent != null){
			score += child.score;
			child = child.parent;
		}
		return score;
	}

	private boolean matchStar(TrieNode item, Pattern p, int i, Map<Long, Double> result)
    {
        // item 字符是 *, 那么只有剩下的字符中能匹配item的子节点
        // 即可完成匹配。
       
        if(item.childs == null) return true;
        boolean matched = false;
        for (int j = i; j < p.itemsSize(); j++)
        {
            if (match(item, p, j, result))
            {
                matched =  true;
            }
        }

        return matched;
    }
	
	public static void main(String[] args) {
		String test1 = new String("[CAR]");
		String test2 = new String("[CAR_MODEL]");
		System.out.println(test1.equals("[CAR]"));
		System.out.println(test2.startsWith("[CAR_"));
		QuestionPattern p1 = new QuestionPattern(1, 10, "搜狐;汽车;自然语言处理;小组;", null);
		QuestionPattern p2 = new QuestionPattern(2, 10, "搜狐;汽车;问答;小组;", null);
		QuestionPattern p3 = new QuestionPattern(3, 10, "搜狐;*;小组;", null);
		QuestionPattern p4 = new QuestionPattern(4, 10, "搜狐;测试;小组;", null);
		QuestionPattern p5 = new QuestionPattern(5, 10, "我;在;搜狐;汽车;问答;小组;工作;", null);
		QuestionPattern p6 = new QuestionPattern(6, 10, "我;在;汽车;问答;小组;工作;", null);
		QuestionPattern p7 = new QuestionPattern(7, 9, "技术;中心;", null);
		QuestionPattern p8 = new QuestionPattern(8, 10, "我;在;汽车;技术;中心;小组;工作;", null);
		QuestionPattern p9 = new QuestionPattern(9, 1, "*", null);
		QuestionPattern p10 = new QuestionPattern(10, 10, "搜狐;汽车", null);
		PatternTrie trie = new PatternTrie();
		System.out.println("add p1:"+trie.add(p1));
		System.out.println("add p2:"+trie.add(p2));
		System.out.println("add p3:"+trie.add(p3));
		System.out.println("add p1:"+trie.add(p1));
		System.out.println("add p7:"+trie.add(p7));
		System.out.println("add p9:"+trie.add(p9));
		System.out.println("add p10:"+trie.add(p10));
		System.out.println("query p1=>" + trie.match(p1));
		System.out.println("query p4=>" + trie.match(p4));
		System.out.println("query p5=>" + trie.match(p5));
		System.out.println("query p6=>" + trie.match(p6));
		System.out.println("query p8=>" + trie.match(p8));
	}
}
