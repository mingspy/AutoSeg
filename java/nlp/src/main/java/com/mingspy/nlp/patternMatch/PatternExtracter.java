package com.mingspy.nlp.patternMatch;

import java.util.ArrayList;
import java.util.List;

import com.mingspy.nlp.patternMatch.types.Pattern;
import com.mingspy.utils.Range;
import com.mingspy.utils.RangeKeeper;
import com.mingspy.utils.SuffixTree;

public class PatternExtracter {
	public enum ExtractMode {
		Original, // 用原始字符串组成pattern
		NamedEntry, // 用命名实体识别字符串
	}


	/**
	 * 提取兩字符串的所有公共子串。<br>
	 * 字符串str1对str2的公共子串与str2对str1的公共子串可能不同，<br>
	 * 所以这里全部提取出来。
	 * @param str1 
	 * @param str2
	 * @return 提取出来的公共子串列表。如果没有公共子串，则列表为空。<br>
	 * 		公共子串的格式为pattern的格式: patternStr1;patternstr2;...
	 */
	public static List<String> extractBothPatterns(String str1, String str2) {
		SuffixTree tree1 = new SuffixTree(str2);
		tree1.addString(str1);

		SuffixTree tree2 = new SuffixTree(str1);
		tree2.addString(str2);

		RangeKeeper keeper1 = tree1.getLastCommonRanges();
		List<String> commonTokens1 = splitTokens(str1, keeper1);
		RangeKeeper keeper2 = tree2.getLastCommonRanges();
		List<String> commonTokens2 = splitTokens(str2, keeper2);
		//System.out.println(keeper1);
		//System.out.println(commonTokens1.toString());
		//System.out.println(keeper2);
		//System.out.println(commonTokens2.toString());
		List<String> patterns = new ArrayList<String>();
		List<String> result1 = simplePattern(keeper1, commonTokens1);
		if(result1 != null){
			patterns.addAll(result1);
		}
		
		List<String> result2 = simplePattern(keeper2, commonTokens2);
		if(result2 != null){
			patterns.addAll(result2);
		}
		
		//patterns.addAll(extractPatterns(keeper1, commonTokens1, commonTokens2));
		//patterns.addAll(extractPatterns(keeper2, commonTokens2, commonTokens1));
		return patterns;
	}


	/**
	 * 提取字符串source相对于refer的公共子串，refer作为基准串<br>
	 * 应用于需要单边提取的情况。
	 * @param source 待提取的字符串
	 * @param refer  参考字符串
	 * @return 提取出来的公共子串列表。如果没有公共子串，则列表为空。<br>
	 * 		公共子串的格式为pattern的格式: patternStr1;patternstr2;...
	 */
	public static List<String> extractPartialPattern(String source, String refer) {
		SuffixTree tree1 = new SuffixTree(refer);
		tree1.addString(source);

		RangeKeeper keeper1 = tree1.getLastCommonRanges();
		List<String> commonTokens1 = splitTokens(source, keeper1);

		return simplePattern(keeper1, commonTokens1);
	}

	private static List<String> splitTokens(String str, RangeKeeper keeper) {
		List<String> tokens = new ArrayList<String>();
		for (Range r : keeper.getRanges()) {
			tokens.add(str.substring(r.getMin(), r.getMax() + 1));
		}
		return tokens;
	}

	/**
	 * 直接把公共子串当作模板
	 * @param keeper1
	 * @param tokens1
	 * @return
	 */
	private static List<String> simplePattern(RangeKeeper keeper1,List<String> tokens1){
		List<String> results = new ArrayList<String>();
		StringBuilder sb = new StringBuilder();
		Range last = null;
		int i = 0;
		for(Range r : keeper1.getRanges()){
			String token = tokens1.get(i);
			token = token.trim();
			// 去掉无用的组合
			if("《<>()（）        	.,?\'\"。，？$#-+》[]".contains(token)||"0123456789".contains(token)
					|| "abcdefghijklmnopqrstuvwxyz".contains(token)||
					"ACDEFGHIJKLMNOPQRSTUVWXYZ".contains(token)
			) {
				continue;
			}
			
			if (last != null) {
				if(r.getMin() < last.getMax()){
					return null;
				}else if (r.getMin() != last.getMax() + 1) {
					sb.append(Pattern.SEPARATOR);
					sb.append(Pattern.Item.STAR);
					sb.append(Pattern.SEPARATOR);
				}
			}
			if(token.startsWith("[")&&!token.endsWith("]") || token.endsWith("]") && !token.startsWith("["))
			{
				return null;
			}
			sb.append(token);
			results.add(token);
			last = r;
			i++;
		}
		
		if(sb.length() > 1){
			results.add(sb.toString());
		}
		return results;
	}
	
	/*
	 * 使用规则提取所有最长公共子串
	 **/
	private static List<String> extractPatterns(RangeKeeper keeper1,
			List<String> tokens1, List<String> tokensRefer) {
		List<String> patterns = new ArrayList<String>();
		List<Range> ranges1 = keeper1.getRanges();
		int lastindex = -1;
		List<Integer> commons = new ArrayList<Integer>();
		boolean notfound = true; // 如果发现，那么就退出，以减少pattern的数量。
		                      
		for (int i = 0; i < tokens1.size() && notfound; i++) {
			commons.clear();
			lastindex = tokensRefer.indexOf(tokens1.get(i));
			commons.add(i);
			for (int j = i + 1; j < tokens1.size(); j++) {
				String current = tokens1.get(j);
				int index = existObjectAfter(tokensRefer, current, lastindex);
				if (index != -1) {
					commons.add(j);
				}
			}
			if(commons.size() > 1){
				String pattern = buildPatternStr(commons, ranges1, tokens1);
				if(pattern != null&& !pattern.isEmpty()){
					patterns.add(pattern);
					notfound = false;
				}
			}
		}
		return patterns;
	}

	private static String buildPatternStr(List<Integer> indexs, List<Range> ranges,
			List<String> tokens) {
		StringBuilder sb = new StringBuilder();
		Range last = null;
		for (Integer i : indexs) {
			Range current = ranges.get(i);
			String pattern = tokens.get(i);
			pattern = pattern.trim();
			if(pattern.isEmpty()){
				last = current;
				continue;
			}
			
			if (last != null) {
				if(current.getMin() < last.getMax()){
					return null;
				}else
				if (current.getMin() != last.getMax() + 1) {
					sb.append(Pattern.SEPARATOR);
					sb.append(Pattern.Item.STAR);
					sb.append(Pattern.SEPARATOR);
				}
			}

			sb.append(pattern);
			last = current;
		}
		return sb.toString();
	}

	private static int existObjectAfter(List<String> list, String str,
			int fromIndex) {
		
		for (int i = fromIndex + 1; i < list.size(); i++) {
			if (list.get(i).equalsIgnoreCase(str)) {
				return i;
			}
		}
		return -1;
	}

	public static void main(String args[]) {
		List<String> lst = new PatternExtracter().extractBothPatterns("二手车异地过户怎样办理？", "买二手车怎么过户");
		System.out.println(lst.toString());
	}
}
