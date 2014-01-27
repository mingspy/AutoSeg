import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;


public class CoreWordInfo {
	private String word;
	private Map<String, Integer> natures = new HashMap<String,Integer>();
	
	public CoreWordInfo(String word){
		this.word = word;
	}
	
	public void addNature(String nature){
		Integer times = natures.get(nature);
		if(times == null){
			times = new Integer(1);
		}else{
			times ++;
		}
		
		natures.put(nature, times);
	}

	@Override
	public String toString() {
		String str = word + "\t";
		for(Entry<String,Integer> en : natures.entrySet()){
			str +=en.getKey()+":"+en.getValue()+",";
		}
		return str;
	}
	
	
}
