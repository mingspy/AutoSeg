package com.mingspy.nlp.patternMatch.types;

import java.io.Serializable;
import java.util.LinkedList;
import java.util.List;

/**
 * 模板类。<br>
 * 在用户意图识别时，采用模板匹配的方式与用户问题进行匹配。<br>
 * 模板是一种扩展的模式匹配方式，每条模板有多个词或变量组成。<br>
 * 词是一些普通的中文字符，变量是一些字符的集合，代表一类字符<br>
 * 如宝马，奔驰，别克等统统属于汽车，可以用[CAR]来表示<br>
 * 这样在模板匹配的时候可以很容易和用户输入的字符进行匹配。并不要求<br>
 * 每个词都和模板相匹配。
 * @author xiuleili
 *
 */
public class Pattern implements Serializable{
	private static final long serialVersionUID = 3778486122323407669L;

	public static final String SEPARATOR = ";"; // 分隔符
	
	protected long patternId;
	protected int score;
	protected List<Item> items;
	protected String patternstr;
	protected String topic;

	public Pattern(long id, int score, String patternstr, String topic) {
		this.patternId = id;
		this.score = score;
		this.patternstr = patternstr;
		this.topic = topic;
		if (patternstr != null) {
			parse();
		}
	}

	public Pattern(long id, int score, List<Item> items, String topic) {
		this.patternId = id;
		this.score = score;
		this.items = items;
		this.topic = topic;
	}

	public String getTopic() {
		return topic;
	}

	public long getPatternId() {
		return patternId;
	}

	public int getScore() {
		return score;
	}

	public final List<Item> getItems() {
		return items;
	}

	public final Item getItem(int index) {
		return items.get(index);
	}
	
	/**
	 * get the items size.
	 * @return
	 */
	public final int itemsSize(){
		if(items != null){
			return items.size();
		}
		return 0;
	}
	
	public String getPatternstr() {
		return patternstr;
	}


	@Override
	public String toString() {
		if (patternstr == null && items != null) {
			StringBuilder s = new StringBuilder();
			for (int i = 0; i < items.size(); i++) {
				s.append(items.get(i).getKey());
				if (i != items.size() - 1) {
					s.append(SEPARATOR);
				}
			}

			patternstr = s.toString();
		}

		return "pattern { id:" + patternId 
				+ " score:" + score 
				+ " str:[" + patternstr + "] topic:" 
				+ topic + "}";
	}

	private void parse() {
		if (patternstr == null)
			return;

		items = new LinkedList<Item>();
		String[] words = patternstr.split(SEPARATOR);
		for (String w : words) {
			items.add(new Item(w));
		}
	}

	public void addItem(String s) {
		if (s == null)
			return;
		addItem(new Item(s));
	}

	public void addItem(Item s) {
		if (s == null)
			return;
		if (items == null) {
			items = new LinkedList<Item>();
		}
		items.add(new Item(s));
		patternstr = null;
	}

	public class Item implements Serializable{
		private static final long serialVersionUID = -523929108527959121L;
		private ItemType type;
		private String key;
		private int score;
		public static final String STAR = "*"; // 通配符
		public static final String STARTWITH = "^"; // 开始
		public static final String ENDWITH = "$"; // 结束
		public static final int SYMBOL_SCORE = 80;
		public static final int KEYWORD_SCORE = 60;
		public static final int CONSTENT_SCORE = 100;

		public Item(String w) {
			key = w;
			if (w.equals(STAR) || w.equals(STARTWITH) || w.equals(ENDWITH)) {
				type = ItemType.KEYWORD;
				score = KEYWORD_SCORE;
			} else if (w.startsWith("[") && w.endsWith("]")) {
				type = ItemType.SYMBOL;
				score = SYMBOL_SCORE;
			} else {
				type = ItemType.CONSTENT;
				score = CONSTENT_SCORE;
			}
		}

		@Override
		public String toString() {
			StringBuilder sb = new StringBuilder();
			sb.append(key);
			sb.append("[");
			sb.append(type);
			sb.append("]");
			return sb.toString();
		}

		public Item(Item it) {
			key = it.key;
			score = it.score;
			type = it.type;
		}

		public ItemType getType() {
			return type;
		}

		public void setType(ItemType type) {
			this.type = type;
		}

		public String getKey() {
			return key;
		}

		public void setKey(String key) {
			this.key = key;
		}

		public int getScore() {
			if (score <= 0) {
				if (type == ItemType.SYMBOL) {
					score = SYMBOL_SCORE;
				} else if (type == ItemType.KEYWORD) {
					score = KEYWORD_SCORE;
				} else if (type == ItemType.CONSTENT) {
					score = CONSTENT_SCORE;
				}
			}
			return score;
		}

		public void setScore(int score) {
			this.score = score;
		}

		@Override
		public boolean equals(Object obj) {
			if (obj instanceof Item) {
				if (((Item) obj).key.equalsIgnoreCase(this.key)) {
					return true;
				}
			} else {
				if (obj instanceof String) {
					if (((String) obj).equalsIgnoreCase(this.key)) {
						return true;
					}
				}
			}
			return false;
		}

		@Override
		public int hashCode() {
			if (key == null)
				return 0;
			return key.hashCode();
		}

		public boolean isstar() {
			return key.equals(STAR);
		}
	}
}
