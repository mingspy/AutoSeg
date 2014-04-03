package com.mingspy.utils.dat.alph;

import java.io.FileInputStream;
import java.io.ObjectInputStream;

public class AlphabetFactory {
	private static Alphabet alphabet = new RoughAlphabet();

	// private static Alphabet alphabet = loadAlphabet();

	protected static Alphabet loadAlphabet() {
		ObjectInputStream instream;
		try {
			long starttime = System.currentTimeMillis();
			instream = new ObjectInputStream(new FileInputStream("resources/statistics.alph"));
			Alphabet alphabet = (Alphabet) instream.readObject();
			long endtime = System.currentTimeMillis();
			System.out.println("Load alphabet used ms:" + (endtime - starttime));
			instream.close();
			return alphabet;
		} catch (Exception e) {
			e.printStackTrace();
		}

		return null;

	}

	public static Alphabet getAlphabet() {
		return alphabet;
	}
}
