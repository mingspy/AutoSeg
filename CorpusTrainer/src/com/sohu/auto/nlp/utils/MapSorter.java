package com.sohu.auto.nlp.utils;

import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

public class MapSorter<K, V> {
	/**
	 * 根据map的value升序排序
	 * @param map 待排序的map
	 * @return 已经排好序的Entry列表
	 */
	public List<Map.Entry<K, V>> sortByValueASC(Map<K, V> map) {
		List<Map.Entry<K, V>> list = new LinkedList<Map.Entry<K, V>>(map.entrySet());
		Collections.sort(list, new Comparator<Map.Entry<K, V>>() {
			@SuppressWarnings({ "rawtypes", "unchecked" })
			public int compare(Map.Entry<K, V> o1, Map.Entry<K, V> o2) {
				return ((Comparable) o1.getValue()).compareTo((Comparable) o2.getValue());
			}

		});
		return list;
	}
	
	/**
	 * 根据map的value降序排序
	 * @param map 待排序的map
	 * @return 已经排好序的Entry列表
	 */
	public List<Map.Entry<K, V>> sortByValueDSC(Map<K, V> map) {
		List<Map.Entry<K, V>> list = new LinkedList<Map.Entry<K, V>>(map.entrySet());
		Collections.sort(list, new Comparator<Map.Entry<K, V>>() {
			@SuppressWarnings({ "rawtypes", "unchecked" })
			public int compare(Map.Entry<K, V> o1, Map.Entry<K, V> o2) {
				return ((Comparable) o2.getValue()).compareTo((Comparable) o1.getValue());
			}

		});
		return list;
	}

	/**
	 * 根据map的value降序排序
	 * @param map 待排序的map
	 * @return 已经排好序的Entry列表
	 */
	public List<Map.Entry<K, V>> sortByKeyDSC(Map<K, V> map) {
		List<Map.Entry<K, V>> list = new LinkedList<Map.Entry<K, V>>(map.entrySet());
		Collections.sort(list, new Comparator<Map.Entry<K, V>>() {
			@SuppressWarnings({ "rawtypes", "unchecked" })
			public int compare(Map.Entry<K, V> o1, Map.Entry<K, V> o2) {
				return ((Comparable) o2.getKey()).compareTo((Comparable) o1.getKey());
			}

		});
		return list;
	}
	
	/**
	 * 根据map的value升序排序
	 * @param map 待排序的map
	 * @return 已经排好序的Entry列表
	 */
	public List<Map.Entry<K, V>> sortByKeyASC(Map<K, V> map) {
		List<Map.Entry<K, V>> list = new LinkedList<Map.Entry<K, V>>(map.entrySet());
		Collections.sort(list, new Comparator<Map.Entry<K, V>>() {
			@SuppressWarnings({ "rawtypes", "unchecked" })
			public int compare(Map.Entry<K, V> o1, Map.Entry<K, V> o2) {
				return ((Comparable) o1.getKey()).compareTo((Comparable) o2.getKey());
			}

		});
		return list;
	}
	
}
