package com.mingspy.nlp;

import java.util.HashSet;
import java.util.Set;

import com.mingspy.utils.io.LineFileReader;

public class StopWords {
	private static Set<String> _stopwords = new HashSet<String>();
	static {
		load();
	}

	public static boolean contains(String word) {
		return _stopwords.contains(word);
	}

	private static void load() {
		LineFileReader reader = new LineFileReader(Thread.currentThread()
				.getContextClassLoader().getResource("stopwords.txt").getPath());
		String line = null;
		try {
			while ((line = reader.nextLine()) != null) {
				_stopwords.add(line);
			}
		} finally {
			reader.close();
		}
	}

}
