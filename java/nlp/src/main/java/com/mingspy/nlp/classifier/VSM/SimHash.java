package com.mingspy.nlp.classifier.VSM;



import java.math.BigInteger;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * SimHash 算法的实现
 * @author mingspy
 */
public abstract class SimHash {
	
	private static final int BITS_PER_UNIT = 16; // 多少位划分到一个单元。
	
	public static int[] genFingerPrint(List<String> terms, int printBits){
		if(terms == null){
			throw new IllegalArgumentException("Null features!");
		}
		//assert((printBits % BITS_PER_UNIT) == 0);
		
		int[] weightVector = new int[printBits];
		for(String term : terms){
			BigInteger hashcode = Hash(term, printBits);
			addWeight(weightVector, hashcode,1.0);
		}

		int[] print = new int[printBits/BITS_PER_UNIT+((printBits%BITS_PER_UNIT == 0)?0:1)];
		genPrint(weightVector, print);
		return print;
	}
	
	/**
	 * 计算两个int数组的汉明距。两数组长度要一致。
	 * @param a
	 * @param b
	 * @return 汉明距数值。
	 */
	public static int hammingDist(int[] a, int[] b){
		int dist = 0;
		for(int i = 0; i < a.length; i++){
			dist += Integer.bitCount(a[i] ^ b[i]);
		}
		return dist;
	}
	
	private static int[] genPrint(int[] weightVector, int[] print) {
		int len = weightVector.length;
		
		for(int i = len - 1, j = 0; i >=0; i--){
			print[j] <<=1;
			if(weightVector[i] > 0){
				print[j] +=1;
			}
			
			if(i%BITS_PER_UNIT == 0){
				j +=1;
			}
		}
		
		return print;
	}


	private static void addWeight(int[] weightVector, BigInteger hashcode, Double weight) {
		int len = weightVector.length;
		
		weight = Math.log(weight) + 1;
		
		for(int i = 0; i < len; i ++){
			if(hashcode.testBit(i)){
				weightVector[i] += weight;
			}else{
				weightVector[i] -=weight;
			}
		}
	}


	//-------------------------------------------------------------------------------------
	// old methods.
	//-------------------------------------------------------------------------------------
	public static BigInteger genFingerPrintForWords(List<String> terms, int printBits){
		if(terms == null){
			throw new IllegalArgumentException("Null features!");
		}
		assert((printBits % 8) == 0);
		
		double[] weightVector = new double[printBits];
		for(String term : terms){
			BigInteger hashcode = Hash(term, printBits);
			makeWeightVector(weightVector, hashcode, 1.0);
		}

		BigInteger print = new BigInteger(new byte[printBits/8]);
		print = setPrintFromVector(weightVector, print);
		return print;
	}
	
	/**
	 * Generate finger print for given terms using SimHash algorithm. About SimHash, please 
	 * @see http://www.cnblogs.com/linecong/archive/2010/08/28/simhash.html
	 * @param terms : means documents feature vector. <word, weight> pairs.
	 * @param printBits : how long the print in bits. It should multiple of 8.
	 * @return
	 */
	public static BigInteger genFingerPrint(Map<String, Double> terms, int printBits){
		if(terms == null){
			throw new IllegalArgumentException("Null features!");
		}
		assert((printBits % 8) == 0);
		
		double[] weightVector = new double[printBits];
		for(String term : terms.keySet()){
			BigInteger hashcode = Hash(term, printBits);
			makeWeightVector(weightVector, hashcode, terms.get(term));
		}

		BigInteger print = new BigInteger(new byte[printBits/8]);
		print = setPrintFromVector(weightVector, print);
		return print;
	}
	
	/**
	 * Set print from weightVector according SimHash.
	 * The input weightVecotor should have same bits with print.
	 * @param weightVector
	 * @param print
	 */
	private static BigInteger setPrintFromVector(double[] weightVector, BigInteger print) {
		assert(weightVector.length == print.bitCount());
		
		int len = weightVector.length;
		for(int i = 0; i < len; i ++){
			if(weightVector[i] > 0){
				print = print.setBit(i);
			}else{
				print = print.clearBit(i);
			}
		}
		
		return print;
	}

	/**
	 * Make weight vector from the given hash code.
	 * In SimHash algorithm, if the bit of hashcode[i] is set, the value of weightVector[i] 
	 * set to weight, else -weight. Here we use one weightVector to contains sum for 
	 * each dimension.
	 * @param weightVector
	 * @param hashcode
	 * @param weight
	 */
	private static void makeWeightVector(double[] weightVector, BigInteger hashcode, Double weight) {
		assert(weightVector.length == hashcode.bitCount());
		
		int len = weightVector.length;
		for(int i = 0; i < len; i ++){
			if(hashcode.testBit(i)){
				weightVector[i] += weight;
			}else{
				weightVector[i] -=weight;
			}
		}
	}
	
	public static int DJBHash(String str)  
    {  
        int hash = 5381;  
          
        for(int i = 0; i < str.length(); i++)  
        {  
            hash = ((hash << 5) + hash) + str.charAt(i);  
        }  
          
        return hash;  
    } 
	
	/**
	 *计算hamming距离 
	 */
	public static int hammingDistance(BigInteger hash,BigInteger otherHash, int bits) {  
		BigInteger x = hash.xor(otherHash);  
        int tot = 0;  
        /*while (x.signum() != 0) {  
            tot += 1;  
            x = x.and(x.subtract(new BigInteger("1")));  
        }*/
        for(int i = 0; i < bits; i++){
        	if(x.testBit(i)){
        		tot ++;
        	}
        }
        return tot;  
	}
	
	/**
	 * Hash the term to given bits.
	 * @param term means the term to be hashed.
	 * @param printBits means how many bits the hash code in bits.
	 * @return the hash code.
	 */
	private static BigInteger Hash(String term, int printBits){
		BigInteger baseHash = new BigInteger("14695981039346656037");
		Long seed = 1099511628211L;
		long hash_mod = (long)java.lang.Math.pow(2, printBits);
		for(int i = 0; i < term.length(); i++){
			baseHash = baseHash.multiply(new BigInteger(seed.toString())).remainder(new BigInteger(hash_mod+""));
			baseHash = baseHash.xor(BigInteger.valueOf((long)term.charAt(i)));
		}
		return baseHash;
	}
	
	public static void main(String[] args) {
		BigInteger a = new BigInteger("1100001001011110111010111000000011111110001111101000111001110111",2);
		int i = 0;
		while((i=a.signum())!=0){
			//System.out.println(i);
			a = a.and(a.subtract(new BigInteger("1"))); 
		}
		
		//System.out.println(SimHash.hammingDistance(new BigInteger("11000011010111101110101111111111",2), new BigInteger("11000010010111101110101111111111",2),32));
		
		
		Map<String, Double> m = new HashMap<String, Double>();
		m.put("我的", 1.0);
		m.put("中国", 1.5);
		m.put("胜利", 3.0);
		m.put("成果", 0.5);
		BigInteger bi = SimHash.genFingerPrint(m, 64);
		System.out.println(bi.toString(2));
	}

}
