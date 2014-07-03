package com.mingspy.nlp.spam;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import com.mingspy.nlp.CharacterTrie;
import com.mingspy.utils.MSTimer;
import com.mingspy.utils.Range;
import com.mingspy.utils.RangeKeeper;
import com.mingspy.utils.io.LineFileReader;

public class WordSpamChecker {
	private static CharacterTrie trie = new CharacterTrie();
	static{
		load();
	}
	private static void load(){
		LineFileReader reader = new LineFileReader(Thread.currentThread()
				.getContextClassLoader().getResource("badword.txt").getPath());
		String line = null;
		try {
			while ((line = reader.nextLine()) != null) {
				trie.addWord(line);
			}
		} finally {
			reader.close();
		}
	}
	
	public boolean hasSpamWord(String text){
		if(text == null || text.isEmpty()){
			return false;
		}
		
		byte [] bytes = text.getBytes();
		for( int i = 0; i < bytes.length; i++){
			for (int j = i + 1; j <= bytes.length; j++){
				if(trie.contains(bytes, i, j)){
					return true;
				}
			}
		}
		return false;
	}
	
	/**
	 * 返回敏感词的起始位置列表. bytes[r.getStart~r.getEnd)是一个敏感词。
	 * @param text
	 * @return 敏感词位置信息。
	 */
	public RangeKeeper findSpamWords(byte [] bytes){
		if(bytes == null || bytes.length == 0){
			return null;
		}

		RangeKeeper keeper = new RangeKeeper();

		for( int i = 0; i < bytes.length; ){
			Range r = null;
			for (int j = i + 1; j <= bytes.length; j++){
				if(trie.contains(bytes, i, j)){
					if(r == null){
						r = new Range(i, j);
					}else{
						r.setEnd(j);
					}
				}
			}
			if(r != null){
				keeper.add(r);
				i = r.getEnd();
			}
			else{
				i++;
			}
		}
		return keeper.size() > 0?keeper:null;
	}
	
	/**
	 * 查找句子中的敏感词，以及敏感词的位置信息(text.getBytes()中的偏移)
	 * @param text
	 * @return
	 */
	public Map<Integer, String> findSpamWords(String text){
		byte [] bytes = text.getBytes();
		RangeKeeper keeper = findSpamWords(bytes);
		if(keeper == null) return null;
		Map<Integer,String> map = new LinkedHashMap<Integer,String>();
		for(Range r : keeper.getRanges()){
			String word = new String(bytes, r.getStart(), r.length());
			map.put(r.getStart(), word);
		}
		return map;
	}
	
	public static void main(String[] args) {
		WordSpamChecker checker = new WordSpamChecker();
		MSTimer timer = new MSTimer();
		System.out.println(checker.findSpamWords("我的2010最新股民资料以及2009第一号人肉搜索令20年专访吕秀莲"));
		System.out.println(timer);
	}
}
