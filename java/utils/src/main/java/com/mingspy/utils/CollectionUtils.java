package com.mingspy.utils;

import java.util.List;

public class CollectionUtils {
	/**
	 * 查找从fromIdex+1开始的第一个目标字符串出现的位置
	 * @param list
	 * @param target
	 * @param fromIndex
	 * @return
	 */
	public static int indexOf(List<String> list, String target, int fromIndex){
		for(int i = fromIndex + 1; i < list.size(); i++){
			if(list.get(i).equalsIgnoreCase(target)){
				return i;
			}
		}
		return -1;
	}
}
