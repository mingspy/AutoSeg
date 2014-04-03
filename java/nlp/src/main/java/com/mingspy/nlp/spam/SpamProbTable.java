package com.mingspy.nlp.spam;

import java.io.FileInputStream;
import java.io.ObjectInputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.ResourceBundle;

public class SpamProbTable {
	// paul graham's algorithm
	private static Map<String,Double> pgbadProbs = new HashMap<String,Double>(); 
	private static final Double NEW_WORD_PROB = new Double(0.4);
	
	// naive bayes
	private static Map<String,Double> nbbadProbs = new HashMap<String,Double>();
	private static Map<String,Double> nbgoodProbs = new HashMap<String,Double>();
	private static double spamDocsProb = 0.1;
	private static Double NEW_WORD_NBPROB = 0.0000001;
	
	static{
		ResourceBundle bundle = ResourceBundle.getBundle("spam");
		loadPGProbs(bundle.getString("SPAM_PROB_TABLE_PATH"));
		loadNBProbs(bundle.getString("SPAM_NBPROB_TABLE_PATH"));;
	}
	
	public static Double getPGProb(String token) {
		Double prob = pgbadProbs.get(token);
		if(prob == null){
			prob = NEW_WORD_PROB;
		}
		
		return prob;
	}
	
	public static Double getNBSpamProb(String token){
		Double prob = nbbadProbs.get(token);
		if(prob == null){
			prob = NEW_WORD_NBPROB;
		}
		
		return prob;
	}
	
	public static Double getNBHealthProb(String token){
		Double prob = nbgoodProbs.get(token);
		if(prob == null){
			prob = NEW_WORD_NBPROB;
		}
		
		return prob;
	}
	public static Double getNBSpamDocsProb(){
		return spamDocsProb;
	}
	
	public static void loadPGProbs(String path){	
		try {
			FileInputStream fileInputStream = new FileInputStream(path);	
			ObjectInputStream reader = new ObjectInputStream(fileInputStream);
			pgbadProbs  = (Map<String,Double>)reader.readObject();
			reader.close();
		} catch (Exception e) {
			e.printStackTrace();
		} 
	}
	
	/**
	 * load naive bayes algorithm probability table.
	 * @param path
	 * @return
	 */
	public static void loadNBProbs(String path){	
		try {
			FileInputStream fileInputStream = new FileInputStream(path);	
			ObjectInputStream reader = new ObjectInputStream(fileInputStream);
			nbbadProbs = (Map<String,Double>)reader.readObject();
			nbgoodProbs = (Map<String,Double>)reader.readObject();
			spamDocsProb = reader.readDouble();
			reader.close();
		} catch (Exception e) {
			e.printStackTrace();
		} 
	}
}
