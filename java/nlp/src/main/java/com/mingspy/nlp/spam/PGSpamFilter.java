package com.mingspy.nlp.spam;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Pattern;

import com.mingspy.nlp.SpliterUtils;

/**
 * Paul Graham bayes spam filter<br>
 * @see http://www.paulgraham.com/spam.html
 */
public class PGSpamFilter implements ISpamFilter{

	private int _maxSpamWords = 15;
	public static final Pattern CHINESE_PATTERN= Pattern.compile("[\u4e00-\u9fa5]+");
	public static final double SPAM_PROB_THRESHOLD = 0.9;
	public void setMaxSpamWords(int num) {
		_maxSpamWords = num;
	}

	public boolean isSpam(String doc){
		return filter(doc) > SPAM_PROB_THRESHOLD;
	}
	
	/**
	 * Paul Graham bayes spam filter<br>
	 * @see http://www.paulgraham.com/spam.html
	 * @param doc - the doc to check whether a spam.
	 * @return [0,1.0] the spam probability of doc
	 */
	public double filter(String doc) {
		
		List<String> tokens = SpliterUtils.split(doc);
		if (tokens == null) {
			return 0;
		}

		// Get the max N spam words.
		List<Double> badProbs = getMaxSpamWords(tokens);

		if(badProbs == null){
			return 0;
		}
		
		// Calculate the probability of spam.
		double pbad = 0.5;
		double pgood = 1.0 - pbad;
		for (Double p : badProbs) {
			pbad *= p;
			pgood *= (1 - p);
		}

		return pbad / (pbad + pgood);
	}

	private List<Double> getMaxSpamWords(List<String> tokens) {

		String stopDelimiters = "~!@#$%^&*()_+=-[]{}|:;\\\"\',.<>/?，。、‘’：；【】·\t 的地得吗";
		Map<String, Double> probMap = new HashMap<String, Double>();
		for (String token : tokens) {
			// remove delimiters.
			if(!stopDelimiters.contains(token))
			probMap.put(token, SpamProbTable.getPGProb(token));
		}

		if(probMap.size() == 0){
			return null;
		}
		List<Map.Entry<String, Double>> sortedList = new LinkedList<Map.Entry<String, Double>>(probMap.entrySet());
		Collections.sort(sortedList, new Comparator<Map.Entry<String, Double>>() {
			public int compare(Map.Entry<String, Double> o1, Map.Entry<String, Double> o2) {
				return (o2.getValue()).compareTo(o1.getValue());
			}

		});

		// for debug
		//System.out.println("\n------------------");
		List<Double> result = new ArrayList<Double>(_maxSpamWords);
		for (int i = 0; i < _maxSpamWords && i < sortedList.size(); i++) {
			result.add(sortedList.get(i).getValue());
			// for debug
			//System.out.print(sortedList.get(i)+" ");
		}
		//System.out.println();
		
		return result;
	}
	
	@Override
	public boolean isSpamProb(double prob) {
		return prob > SPAM_PROB_THRESHOLD;
	}
	
	public static void main(String[] args) {
		PGSpamFilter spam = new PGSpamFilter();
		System.out.println(spam.filter("张华平写"));
		System.out.println(spam.filter("张华平写中文分词"));
		System.out.println(spam.filter("中文分词"));
	}

	@Override
	public String getFilterName() {
		return "Paul Graham Spam Filter";
	}

	@Override
	public double getSpamThreshold() {
		return SPAM_PROB_THRESHOLD;
	}
}
