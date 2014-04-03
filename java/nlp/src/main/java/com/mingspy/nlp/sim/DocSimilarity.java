package com.mingspy.nlp.sim;



import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import com.mingspy.utils.MathVector;



public class DocSimilarity {

	/**
	 * 计算两文章向量的相似度
	 * @param doc1Features <文章关键词，权重>
	 * @param doc2Features <文章关键词，权重>
	 * @return
	 */
	public static double calcDocSim(Map<String,Double> doc1Features,
			Map<String,Double> doc2Features,
			boolean usingCosine){
		if(doc1Features == null ||  doc2Features == null){
			return 0;
		}
		
		// Compose keywords, and build keywords vector.
		Set<String> keySet = new HashSet<String>(doc1Features.size());

		keySet.addAll(doc1Features.keySet());
		keySet.addAll(doc2Features.keySet());
		
		
		// Now, allocate the weights vector for map1 and map2.
		int keySetSize = keySet.size();
		double [] x = new double[keySetSize];
		double [] y = new double[keySetSize];
		
		int i = 0;
		for(String key : keySet){
			Double valueMap1 = doc1Features.get(key);
			x[i] = valueMap1==null?0.0:valueMap1;
			Double valueMap2 = doc2Features.get(key);
			y[i] = valueMap2==null?0.0:valueMap2;
			
			i++;
		}
		if(usingCosine){
			return cosineSimilary(x, y);
		}
		
		return pearsonSimilary(x,y);
	}
	
	/**
	 * Pearson similary algorithm.
	 * @author Administrator
	 * sim(x,y) = sigma(x - x')(y - y') / sqrt[sigma((x - x')^2) * sigma((y-y')^2)]
	 *          = (sigmaxy - sigmaxsigmay/n) / sqrt[(sigma(x^2) - ((sigmax)^2)/n)(sigma(y^2) - (sigma(y)^2)/n)]
	 *      x  - vector x
	 *      y  - vector y
	 *      x' - average of x
	 *      y' - average of y
	 *      sigma  -  sum of vector.
	 */
	public static double pearsonSimilary(double[] x, double []y) {

		if(x.length == 0 || y.length == 0) return 0;
		double sigma_x = MathVector.cumulativeSum(x);
		double sigma_y = MathVector.cumulativeSum(y);
		double sigma_xx = MathVector.sumOfSquare(x);
		double sigma_yy = MathVector.sumOfSquare(y);
		double sigma_xy = MathVector.dotProduct(x, y);
		double numerator = sigma_xy - sigma_x*sigma_y / x.length;
		double denominator = (double) Math.sqrt (
				(sigma_xx - Math.pow(sigma_x, 2.0) / x.length) *
				(sigma_yy - Math.pow(sigma_y, 2.0) / y.length)); 

		if(denominator == 0){
			if(sigma_xx == 0 && sigma_yy == 0){
				return 1;
			}
			
			return 0;
		}
		
		return numerator/denominator;
	}
	
	/**
	 * sim(x,y) = cos(x,y)=sigma(xy) / sqrt(sigmax^2 * sigmay^2)
	 * @param x
	 * @param y
	 * @return
	 */
	public static double cosineSimilary(double[] x, double[] y){
		double sigma_xx = MathVector.sumOfSquare(x);
		double sigma_yy = MathVector.sumOfSquare(y);
		double sigma_xy = MathVector.dotProduct(x, y);
		double numerator = sigma_xy;
		double denominator = (float) Math.sqrt (sigma_xx * sigma_yy);
		if(denominator == 0){
			return 0;
		}

		return numerator / denominator;
	}
}
