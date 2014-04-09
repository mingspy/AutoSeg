package com.mingspy.corpus;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import java.util.Set;
import java.util.TreeSet;

import com.mingspy.utils.MSTimer;
import com.mingspy.utils.MapSorter;
import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

public class PeopleDailyDataProcesser {

	private Map<String, WordInfo> coreWords = new HashMap<String, WordInfo>();
	private Set<String> natures = new TreeSet<String>();
	private boolean compose = false;
	
	private Map<String, WordInfo> bigramWords = new HashMap<String, WordInfo>();
	private Map<String, WordInfo> lexicalInfo = new HashMap<String, WordInfo>();
	
	public void processFiles(String directory, String output)
			throws IOException {
		MSTimer timer = new MSTimer();
		File dir = new File(directory);
		if (!dir.exists()) {
			System.out.println(directory + " not exist!!!");
			return;
		}

		File[] files = null;
		if(dir.isDirectory()){
			files = dir.listFiles();
		}else{
			files = new File[1];
			files[0] = dir;
		}
		
		for (File file : files) {
			processAFile(file);
		}
		
		
		natures.add(WordInfo.FIELD_TOTAL);
		
		prune(coreWords,1);
		writeWordInfo(coreWords, output+"/core/coreWord.txt");
		prune(bigramWords,1);
		writeWordInfo(bigramWords, output+"/core/bigramWords.txt");
		
		
		prune(lexicalInfo,1);
		writeWordInfo(lexicalInfo, output+"lexicalInfo.txt");
		
		System.out.println("total used: "+timer);
	}

	private void prune(Map<String, WordInfo> coreWords, int limit) {
		List<String> keys = new ArrayList<String>(1000);
		for(Entry<String,WordInfo> en : coreWords.entrySet()){
			if(en.getValue().sumFreq() <= limit){
				keys.add(en.getKey());
			}
		}
		
		for(String key : keys){
			coreWords.remove(key);
		}	
	}

	private void writeWordInfo(Map<String,WordInfo> coreWords,String output)
			throws UnsupportedEncodingException, FileNotFoundException {
		MapSorter<String, WordInfo> sorter = new MapSorter<String, WordInfo>();
		List<Entry<String, WordInfo>> res = sorter.sortByKeyASC(coreWords);
		// out put
		LineFileWriter writer = new LineFileWriter(output);
		String nature = "@WordPOS\t";
		for (String na : natures) {
			nature += na + ",";
		}
		writer.writeLine(nature);

		for (Entry<String, WordInfo> en : res) {
			writer.writeLine(en.getValue().toString());
		}
		writer.close();
	}

	private void processAFile(File file) {
		BufferedReader reader = null;
		try {
			// String coding = Utilities.getFileCoding(file);
			reader = new BufferedReader(new InputStreamReader(
					new FileInputStream(file), "utf-8"));
			String line = null;
			while ((line = reader.readLine()) != null) {
				List<SplitWord> splited_words = changeToSplitedWord(line.trim());
				if (splited_words == null) {
					continue;
				}

				trianCorpusFromMarkedWords(splited_words);
			}

		} catch (IOException ee) {
			ee.printStackTrace();
		} finally {
			try {
				if (reader != null)
					reader.close();
			} catch (IOException e) {
				throw new RuntimeException(e);
			}
		}
	}


	private void trianCorpusFromMarkedWords(List<SplitWord> words) {
		// 提取核心词典信息
		extractCoreWords(words);

		// 提取二元词典信息
		extractBigramWords(words);

		// 提取词性信息
		extractLexicalContext(words);

		// 提取人名词典
		// extractPersonNameWords(words);
		// 提取组织机构词典
		// extractOrganizationWords(words);
		// 提取译名词典
	}

	/**
	 * 提取标注的字串,按顺序加入词链
	 * 
	 * @param line
	 *            is same like
	 *            "19980131-04-006-001/m  飞雪/n  迎/v  春/Tg  到/v  心潮/n  逐/Vg  浪/n  高/a  （/w  书法/n  ）/w"
	 * @return
	 */
	private List<SplitWord> changeToSplitedWord(String line) {
		String[] marked_words = line.split("  ");
		List<SplitWord> splited_words = new LinkedList<SplitWord>();
		for (int i = 1; i < marked_words.length; i++) {
			if (marked_words[i].isEmpty()) {
				continue;
			}
			String[] w = marked_words[i].split("/");

			// handle [中央/n 人民/n 广播/vn 电台/n]nt
			if (w[0].startsWith("[")) {
				splited_words.add(new SplitWord("[", "w"));
				w[0] = w[0].substring(1);
			}

			if (w[w.length - 1].contains("]")) {
				String[] ww = w[1].split("]");
				splited_words.add(new SplitWord(w[0], ww[0]));
				w[0] = "]";
				w[1] = ww[1];
			}

			splited_words.add(new SplitWord(w[0], w[1]));

		}
		return splited_words.isEmpty() ? null : splited_words;
	}

	/**
	 * 提取核心词典信息 先提取出非特殊词 加入词库由于人名的姓和名是分开的，所以对于人名进行合并，加入词库，并增加PER的出现次数。
	 * 对于地名和组织机构名，不合并，加入核心词典, 然后增加对应ORG和LOC的频率。 时间和数词不加入核心词典，只增加NUM,TIME的频率。
	 * 
	 * @param splited_words
	 */
	private void extractCoreWords(final List<SplitWord> words) {
		// TODO 训练实际的分词时，决定是否组合专有名词和人名。设置compose = true;则组合。
		int composeIndex = -1;
		for (int i = 0; i < words.size(); i++) {
			SplitWord sw = words.get(i);
			String word = sw.getWord();

			if (word.equals("[")) {
				composeIndex = i + 1;
				continue;
			}

			String nature = sw.getFlag();
			if (nature.equals("%")) {
				System.out.println(words.toString());
			}

			if (compose) {
				// 如果是人名，则把人名合并
				if (nature.equals("nr") && (i + 1) < words.size()) {
					sw = words.get(i + 1);
					if (!sw.getFlag().equals("nr")) {
						word += sw.getWord();
						i++;
					}
				}
			}
			// 如果词非空,不是标点
			// 则加入核心词库.
			if (!word.isEmpty() && !word.equals("]")) {
				addWordInfo(coreWords,word, nature);
			}

			// 添加组合词
			if (compose) {
				if (word.equals("]")) {
					word = "";
					if (composeIndex >= 0) {
						for (int j = composeIndex; j < i; j++) {
							if (!words.get(j).getFlag().equals("w"))
								word += words.get(j).getWord();
						}
						addWordInfo(coreWords,word, nature);
					}
					composeIndex = -1;
				}
			}
		}

	}

	/**
	 * 提取二元共献词典 把属于特殊词类的所有词都替换成特殊词的标记，进行统计。 其他按正常词类计算。标点不统计，作为切分点。
	 * 停用词也不统计，作为切分点。 不加开始结束标志，亦可正确分词。
	 * 
	 * @param words
	 */
	private void extractBigramWords(final List<SplitWord> words) {
		String currWord = null;
		String nextWord = null;
		for (currentIndex = 0; currentIndex < words.size() - 1;) {
			// 获取当前词
			if (currWord == null) {
				currWord = nextBigramWord(words);
			}

			// 获取下一词
			nextWord = nextBigramWord(words);
			if (currWord != null && nextWord != null) {
				String biWord = currWord + "@"+nextWord;
				addWordInfo(bigramWords, biWord, WordInfo.FIELD_TWOFREQ);
			}
			// 把下一词赋值给当前词
			currWord = nextWord;
			nextWord = null;
		}
	}


	private int currentIndex = 0;

	private String nextBigramWord(final List<SplitWord> words) {
		if (currentIndex >= words.size())
			return null;
		SplitWord sw = words.get(currentIndex++);
		String currWord = sw.getWord();

		// 处理组合词 [ ]
		if (compose && currWord.equals("[")) {
			currWord = "";
			while (currentIndex < words.size()) {
				sw = words.get(currentIndex++);
				if (sw.getWord().equals("]")) {
					break;
				}

				currWord += sw.getWord();
				
				String nxtWord = words.get(currentIndex).getWord();
				if(!nxtWord.equals("]")){
					addWordInfo(bigramWords,sw.getWord() + "@" + nxtWord,WordInfo.FIELD_TWOFREQ);
				}
			}
		} 
		
		if(!compose && currWord.equals("]")){
			return nextBigramWord(words);
		}
		
		String flag = sw.getFlag();
		if(flag.equals("w")){
			if(currWord.equals("["))
				return nextBigramWord(words);
			// 标点符号去掉
			return null;
		}

		// 如果是人名，则把人名合并
		
		if (compose && flag.equalsIgnoreCase("nr") && currentIndex < words.size()) {
			sw = words.get(currentIndex);
			if (!sw.getFlag().equalsIgnoreCase("nr")) {
				currWord += sw.getWord();
				currentIndex++;
			}
				
		}

		return currWord;
	}
	
	private void addWordInfo(Map<String, WordInfo> coreWords,String word, String nature) {
		natures.add(nature);
		WordInfo info = coreWords.get(word);
		if (info == null) {
			info = new WordInfo(word);
		}
		info.addNature(nature);
		coreWords.put(word, info);
	}

	public void prepairTrainTestData(String orign_data_dir, String out_dir)
			throws IOException {
		MSTimer timer = new MSTimer();

		File dir = new File(orign_data_dir);
		if (!dir.exists() || !dir.isDirectory()) {
			System.out.println(orign_data_dir + " not a directory!!!");
			return;
		}

		LineFileWriter trainData = new LineFileWriter(out_dir
				+ "train_data.txt");
		LineFileWriter testData = new LineFileWriter(out_dir + "test_data.txt");
		LineFileWriter testRefer = new LineFileWriter(out_dir
				+ "test_refer.txt");
		Random r = new Random();
		File[] files = dir.listFiles();

		for (File file : files) {
			LineFileReader reader = new LineFileReader(file.getAbsolutePath());
			String line = null;
			while ((line = reader.nextLine()) != null) {
				if (r.nextInt() % 10 == 1) {
					line = line.substring(line.indexOf("  ") + 2);
					testRefer.writeLine(line);

					String[] tokens = line.split("  ");
					StringBuilder strBuilder = new StringBuilder();
					for (String token : tokens) {
						String[] parts = token.split("/");
						if (parts[0].startsWith("[")) {
							parts[0] = parts[0].substring(1);
						}
						strBuilder.append(parts[0]);
					}
					testData.writeLine(strBuilder.toString());
				} else {
					trainData.writeLine(line);
				}
			}

			reader.close();
		}

		trainData.close();
		testData.close();
		testRefer.close();

		System.out.println(timer);
	}

	private void extractLexicalContext(final List<SplitWord> words) {
		String flag = null;
		for (Iterator<SplitWord> it = words.iterator(); it.hasNext();) {
			SplitWord sw = it.next();
			if (sw.getWord().equals("[")) {
				continue;
			}
			else if(sw.getWord().equals("]")){
				flag = sw.getFlag();
				continue;
			}

			if (flag != null) {
				addWordInfo(lexicalInfo, flag, sw.getFlag());
			}
			
			flag = sw.getFlag();
		}
	}
	
	public static void main(String[] args) throws IOException {
		PeopleDailyDataProcesser peopleDaily = new PeopleDailyDataProcesser();

		// peopleDaily.processFiles("E:/tmp/alphabet/corpus_data",
		// "e:/tmp/alphabet/coreWordInfo.txt");

		//peopleDaily.prepairTrainTestData("E:/tmp/alphabet/corpus_data",
		//		"E:/tmp/alphabet/estimate");
		
		peopleDaily.processFiles("D:/GitHub/AutoSeg/Cpp/data/estimate/train_data.txt",
				 "D:/GitHub/AutoSeg/Cpp/data/words/");
		
		System.out.println("done!!");

	}
}
