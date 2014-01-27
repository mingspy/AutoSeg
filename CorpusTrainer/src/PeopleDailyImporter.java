import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.TreeSet;

import com.sohu.auto.nlp.utils.MapSorter;
import com.sohu.auto.nlp.utils.io.LineFileWriter;


public class PeopleDailyImporter {

	private Map<String, CoreWordInfo> coreWords = new HashMap<String, CoreWordInfo>();
	private Set<String> natures = new TreeSet<String>();
	public void processFiles(String directory, String output) throws UnsupportedEncodingException, FileNotFoundException{
		File dir = new File(directory);
		if(!dir.exists() || !dir.isDirectory()){
			System.out.println(directory+" not a directory!!!");
			return;
		}
		
		File[] files = dir.listFiles();
		for(File file:files){
			processAFile(file);
		}
		MapSorter<String, CoreWordInfo> sorter = new MapSorter<String, CoreWordInfo>();
		List<Entry<String,CoreWordInfo>> res = sorter.sortByKeyASC(coreWords);
		// out put
		LineFileWriter writer = new LineFileWriter(output);
		String nature = "@词性\t";
		for(String na : natures){
			nature+=na+",";
		}
		writer.writeLine(nature);
		
		for(Entry<String, CoreWordInfo> en : res){
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
				processALine(line.trim());
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
	
	/**
	 * 处理一行标注预料，分别提取出： 核心词，二元词， 人名角色词典，人名角色转移词典， 组织机构角色词典，组织机构角色转移词典，
	 * 译名词典，译名角色转移词典
	 * 
	 * @param line
	 *            is same like
	 *            "19980131-04-006-001/m  飞雪/n  迎/v  春/Tg  到/v  心潮/n  逐/Vg  浪/n  高/a  （/w  书法/n  ）/w"
	 * 
	 */
	private void processALine(String line) {
		if (line == null || line.isEmpty())
			return;
		// 提取出原始词串信息
		List<SplitWord> splited_words = extractSplitedWord(line);
		if (splited_words == null) {
			return;
		}

		trianCorpusFromMarkedWords(splited_words);

	}
	
	private void trianCorpusFromMarkedWords(List<SplitWord> words) {
		// 提取核心词典信息
		extractCoreWords(words);

		// 提取二元词典信息
		// extractBigramWords(words);
		
		// 提取词性信息
		//extractLexicalContext(words);

		// 提取人名词典
		// extractPersonNameWords(words);
		// 提取组织机构词典
		//extractOrganizationWords(words);
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
	private List<SplitWord> extractSplitedWord(String line) {
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

			splited_words.add(new SplitWord(w[0],w[1]));

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
		int composeIndex = -1;
		for (int i = 0; i < words.size(); i++) {
			SplitWord sw = words.get(i);
			String word = sw.getWord();

			if (word.equals("[")) {
				composeIndex = i+1;
				continue;
			}

			String nature = sw.getFlag();
			if(nature.equals("%")){
				System.out.println(words.toString());
			}
			// 如果是人名，则把人名合并
			if (nature.equals("nr")) {
				int t = 0;
				while (i + 1 < words.size()) {
					sw = words.get(i + 1);
					if (!sw.getFlag().equals("nr")) {
						break;
					}
					word += sw.getWord();
					i++;
					t ++;
				}
				if(t > 3){
					continue;
				}
			}

			// 如果词非空,不是标点
			// 则加入核心词库.
			if (!word.isEmpty() && !word.equals("]")) {
				addCoreWord(word, nature);
			}
			
			// 添加组合词
			if(word.equals("]")){
				word = "";
				if(composeIndex >= 0){
					for(int j = composeIndex; j < i; j++){
						if(!words.get(j).getFlag().equals("w"))
							word += words.get(j).getWord();
					}
					addCoreWord(word, nature);
				}
				composeIndex = -1;
			}
		}
			
	}
	
	private void addCoreWord(String word, String nature){
		natures.add(nature);
		CoreWordInfo info = coreWords.get(word);
		if(info == null){
			info = new CoreWordInfo(word);
		}
		info.addNature(nature);
		coreWords.put(word, info);
	}
	
	
	public static void main(String[] args) throws UnsupportedEncodingException, FileNotFoundException {
		PeopleDailyImporter peopleDaily = new PeopleDailyImporter();
		long start_time = System.currentTimeMillis();
		peopleDaily.processFiles("E:/tmp/alphabet/corpus_data", "e:/tmp/alphabet/coreWordInfo.txt");
		long end_time = System.currentTimeMillis();
		System.out.println("work done! used "+(end_time - start_time));
	
	}
}
