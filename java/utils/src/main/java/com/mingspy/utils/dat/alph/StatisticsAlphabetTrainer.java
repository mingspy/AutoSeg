package com.mingspy.utils.dat.alph;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.UnsupportedEncodingException;
import java.security.InvalidParameterException;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

public class StatisticsAlphabetTrainer {
	private Set<String> wordsSet = new HashSet<String>();
	private Map<Integer, Set<Integer>> map = new HashMap<Integer, Set<Integer>>();
	List<CharInfo> info = new LinkedList<CharInfo>();
	private int maxCode = 0;

	public void addWord(String word) {
		wordsSet.add(word);
	}

	public void addWords(Set<String> words) {
		wordsSet.addAll(words);
	}

	public StatisticsAlphabet trian() {
		map.clear();

		Set<Integer> nochildren = new HashSet<Integer>();
		// prepare char relations.
		for (String word : wordsSet) {
			for (int i = 0; i < word.length() - 1; i++) {
				addChildren(word.charAt(i), word.charAt(i + 1));
			}

			if (word.length() > 0)
				nochildren.add((int) word.charAt(word.length() - 1));
		}

		// get Children info.
		info.clear();
		for (Entry<Integer, Set<Integer>> entry : map.entrySet()) {
			CharInfo ci = new CharInfo();
			ci.ch = entry.getKey();
			ci.childSize = entry.getValue().size();
			info.add(ci);
			nochildren.remove(ci.ch);
		}

		// append no children info.
		for (Integer i : nochildren) {
			CharInfo ci = new CharInfo();
			ci.ch = i;
			ci.childSize = 0;
			info.add(ci);
		}

		// sort:
		// 1. the one has more children has a smaller index.
		// 2. the one whose Unicode is litter has a smaller index.
		Collections.sort(info, new Comparator<CharInfo>() {
			@Override
			public int compare(CharInfo o1, CharInfo o2) {
				if (o1.childSize < o2.childSize) {
					return 1;
				} else if (o1.childSize > o2.childSize) {
					return -1;
				} else {
					return o1.ch - o1.ch > 0 ? 1 : -1;
				}
			}
		});

		int code = 256;
		for (CharInfo ci : info) {
			if (ci.ch < 256) {
				ci.code = ci.ch;
			} else {
				ci.code = code++;
			}
		}

		maxCode = code - 1;
		Map<Integer, CharInfo> tmp = new HashMap<Integer, CharInfo>();
		for (CharInfo ci : info) {
			tmp.put(ci.ch, ci);
		}

		// update childsize: the max children code.
		for (CharInfo ci : info) {
			int max = ci.childSize;
			if (max == 0) {
				continue;
			}

			Set<Integer> children = map.get(ci.ch);
			for (Integer child : children) {
				CharInfo cci = tmp.get(child);
				if (cci.code > max) {
					max = cci.code;
				}
			}
			ci.childSize = max;
		}

		//
		Map<Integer, CharInfo> alph_info = new HashMap<Integer, CharInfo>();
		for (CharInfo ci : info) {
			alph_info.put(ci.ch, ci);
		}
		StatisticsAlphabet alph = new StatisticsAlphabet(maxCode, alph_info);
		return alph;

	}

	private void addChildren(int parent, int child) {
		Set<Integer> childs = map.get(parent);
		if (childs == null) {
			childs = new HashSet<Integer>();
			map.put(parent, childs);
		}
		childs.add(child);
	}

	public void outputWords(String path) throws UnsupportedEncodingException, FileNotFoundException {
		LineFileWriter writer = new LineFileWriter(path);
		writer.writeLine(wordsSet);
		writer.close();
	}

	public void outputCharInfo(String path) throws UnsupportedEncodingException, FileNotFoundException {
		LineFileWriter writer = new LineFileWriter(path);
		writer.writeLine("maxcode=" + maxCode);
		writer.writeLine(info);
		writer.close();
	}

	public static Set<String> chineseDailyWordsLoader(String dictionary) {
		Set<String> set = new HashSet<String>();
		File rootDict = new File(dictionary);
		if (!rootDict.isDirectory()) {
			throw new InvalidParameterException("input not a dictionary");
		}

		File[] files = rootDict.listFiles();
		for (File file : files) {
			if (!file.getName().endsWith("txt"))
				continue;

			LineFileReader reader = new LineFileReader(file.getAbsolutePath());
			String line = null;
			while ((line = reader.nextLine()) != null) {
				String[] fields = line.split("  ");
				for (int i = 1; i < fields.length; i++) {
					String[] word = fields[i].trim().split("/");
					set.add(word[0]);
				}
			}

		}

		return set;
	}

	public static void outputUnicode(String path) throws UnsupportedEncodingException, FileNotFoundException {
		LineFileWriter writer = new LineFileWriter(path);
		String header = "     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F";
		StringBuilder line = new StringBuilder();
		line.append(header);
		for (int i = 0; i <= 65536; i++) {
			if ((i & 0xFF) == 0xFF) {
				line.append(' ');
				line.append((char) i);
				line.append('\n');
				// print block
				writer.writeLine(line.toString());
				line = new StringBuilder();
				line.append(header);
				continue;
			}

			if ((i & 0x0F) == 0) {
				// print line header
				line.append('\n');
				line.append(Integer.toHexString(i & 0xFFF0));
			}

			line.append(' ');
			line.append((char) i);
		}

		writer.close();
	}

	public static void main(String args[]) throws IOException {
		// StatisticsAlphabetTrainer.outputUnicode("E:/tmp/alphabet/unicode.txt");

		StatisticsAlphabetTrainer trainer = new StatisticsAlphabetTrainer();
		Set<String> words = StatisticsAlphabetTrainer.chineseDailyWordsLoader("E:/tmp/alphabet/corpus_data");
		trainer.addWords(words);
		words = null;
		StatisticsAlphabet alph = trainer.trian();
		trainer.outputWords("E:/tmp/alphabet/words.txt");
		trainer.outputCharInfo("E:/tmp/alphabet/charinfo.txt");

		ObjectOutputStream stream = new ObjectOutputStream(new FileOutputStream("e:/tmp/alphabet/statistics.alph"));
		stream.writeObject(alph);
		stream.close();
		System.out.println("works done!!!!");
	}
}
