package com.mingspy.nlp.sim;

import java.util.ArrayList;
import java.util.List;

/**
 * Jaccard相似度<br>
 * J(A,B) = C / (A + B - C)<br>
 * 其中，C = {x| x belongs A && x belongs B}<br>
 * 
 * @author xiuleili
 * 
 */
public class JaccardSimilarity {

	public static double calcSegmentedSimilarity(List<String> strs1, List<String> strs2) {
		List<String> commons = new ArrayList<String>();
		// TODO  经过测试，Jaccard相似度在短文本中几乎全部为0。
		// 原因是：1.这里没有进行同义词判别，没有公共词出现。
		//       2.问题很短，而答案很长，公共词占比重太低。
		// 基于上述原因，目前暂不做改进。
		for(String str : strs1){
			if(strs2.contains(str)){
				commons.add(str);
			}
		}
		return commons.size() / (strs1.size() + strs2.size() - commons.size());
	}

}
