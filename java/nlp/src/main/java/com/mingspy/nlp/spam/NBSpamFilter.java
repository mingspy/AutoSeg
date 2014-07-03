package com.mingspy.nlp.spam;

import java.util.ArrayList;
import java.util.List;

import com.mingspy.nlp.SpliterUtils;
import com.mingspy.nlp.StopWords;

public class NBSpamFilter implements ISpamFilter{

	private int _maxSpamWords = 15;
	public static final double SPAM_PROB_THRESHOLD = 0.9;
	private double spamDocsProb = SpamProbTable.getNBSpamDocsProb();
	public void setMaxSpamWords(int num) {
		_maxSpamWords = num;
	}

	public boolean isSpam(String doc){
		try {
			return filter(doc) > SPAM_PROB_THRESHOLD;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return false;
	}
	
	/**
	 * Paul Graham bayes spam filter<br>
	 * @see http://www.paulgraham.com/spam.html
	 * @param doc - the doc to check whether a spam.
	 * @return [0,1.0] the spam probability of doc
	 * @throws Exception 
	 */
	public double filter(String doc) {
		
		List<String> tokens = SpliterUtils.split(SpliterUtils.stem(doc));
		if (tokens == null) {
			return 0;
		}

		// Get the max N spam words.
		List<String> features = filterStopWords(tokens);
		
		if(features == null || features.isEmpty()){
			return 0;
		}
		
		// Calculate the probability of spam.
		double pbad = 0.5;
		double pgood = 1.0 - pbad;
		for (String word : features) {
			pbad *= SpamProbTable.getNBSpamProb(word) * 100.0;
			pgood *= SpamProbTable.getNBHealthProb(word) * 100.0;
		}

		return pbad / (pbad + pgood);
	}

	/**
	 * 去除停用词。
	 * @param tokens
	 * @return
	 */
	private List<String> filterStopWords(List<String> tokens) {
		// 去除标点符号和常见词
		
		List<String> features = new ArrayList<String>();
		for(String token : tokens){
			if(!StopWords.contains(token)){
				features.add(token);
			}
		}
		return features;
	}

	@Override
	public boolean isSpamProb(double prob) {
		return prob > SPAM_PROB_THRESHOLD;
	}
	
	
	public static void main(String[] args) throws Exception {
		NBSpamFilter spam = new NBSpamFilter();
		System.out.println(spam.filter("张华平写"));
		System.out.println(spam.filter("张华平写中文分词"));
		System.out.println(spam.filter("中文分词"));
	}

	@Override
	public String getFilterName() {
		return "Naive Bayes Spam Filter";
	}

	@Override
	public double getSpamThreshold() {
		return SPAM_PROB_THRESHOLD;
	}
}
