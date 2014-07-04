package com.mingspy.toolkits;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.io.FileUtils;
import org.codehaus.jackson.JsonParseException;
import org.codehaus.jackson.map.JsonMappingException;
import org.codehaus.jackson.map.ObjectMapper;
import org.htmlcleaner.HtmlCleaner;
import org.htmlcleaner.TagNode;
import org.htmlcleaner.XPatherException;

import com.mingspy.utils.MSTimer;
import com.mingspy.utils.MapSorter;
import com.mingspy.utils.html.HttpDownloader;
import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

/**
 * 汽车之家精华帖列表下载器
 * 
 * @author xiuleili
 * 
 */
public class AutoHomeClubJXDownloader  {
	public static final String CLUB_URL = "http://club.autohome.com.cn";
	public static final String URL_PREFIX = "/bbs/jx-a-";
	public static final String URL_POSTFIX = ".html";

	public static class Post implements Comparable<Post>{
		private String topicid;
		private String title;
		private String author;
		private String url;
		private int replies;
		private int hits;
		private String time; // 发布时间

		@Override
		public String toString() {
			return "{title=" + title + ",author=" + author + ",url=" + url
					+ ",replies=" + replies + ",hits=" + hits +",time="+time+ "}";
		}

		public String getTitle() {
			return title;
		}

		public void setTitle(String title) {
			this.title = title;
		}

		public String getAuthor() {
			return author;
		}

		public void setAuthor(String author) {
			this.author = author;
		}

		public String getUrl() {
			return url;
		}

		public void setUrl(String url) {
			this.url = url;
		}

		public int getReplies() {
			return replies;
		}

		public void setReplies(int replies) {
			this.replies = replies;
		}

		public int getHits() {
			return hits;
		}

		public void setHits(int hits) {
			this.hits = hits;
		}

		public String getTime() {
			return time;
		}

		public void setTime(String time) {
			this.time = time;
		}
		
		public String getTopicid() {
			return topicid;
		}

		public void setTopicid(String topicid) {
			this.topicid = topicid;
		}

		public int compareTo(Post o) {
			int cmphits = hits - o.hits;
			if(cmphits == 0){
				return replies - o.replies;
			}
			return cmphits;
		}
	}

	private Map<String, Post> posts = new HashMap<String,Post>();

	/**
	 * 下载某个地区精华帖列表。
	 * 
	 * @param areaCode
	 * @return
	 * @throws XPatherException
	 * @throws IOException 
	 * @throws InterruptedException 
	 */
	public boolean downloadAreaLists(int areaCode) throws XPatherException, IOException, InterruptedException {
		HttpDownloader downloader = new HttpDownloader();
		HtmlCleaner cleaner = new HtmlCleaner();
		boolean hasSuccessDown = false;

		int total = -1;
		// 最多100页
		for (int i = 1; i < 200; i++) {
			String pageName = URL_PREFIX + areaCode +"-"+ i + URL_POSTFIX;
			String content = downloader.downloadAsString(CLUB_URL + pageName+"#pvareaid=101061","gb2312");
			if (content == null) {
				System.out.println("download fail");
				return hasSuccessDown;
				
			}

			FileUtils.writeStringToFile(new File("e:/tmp/autohome/"+pageName), content,"gb2312");
			TagNode root = cleaner.clean(content);

			if (total == -1) {
				Object[] totalpageNode = root
						.evaluateXPath("//div[@class='pagearea']//span[@class='fr']");
				if (totalpageNode == null)
					return hasSuccessDown;
				String text = ((TagNode) totalpageNode[0]).getText().toString().trim();
				// / 31 页
				text = text.substring(1, text.indexOf('页'));
				total = Integer.parseInt(text.trim());
				if (total <= 0)
					return hasSuccessDown;
			}
			hasSuccessDown = true;
			Object[] dls = root
					.evaluateXPath("//div[@class='carea']//dl[@class='list_dl']");
			String ids = genPosts(dls);
			
			//get replys and views
			getReplyAndViews(ids ,downloader );
			if (i == total) {
				System.out.println("total download "+i+" pages in area "+areaCode);
				return true;
			}
			
			Thread.currentThread().sleep(200);
		}
		return hasSuccessDown;
	}

	public static class RV {
		private int topicid;
		private int replys;
		private int views;

		public RV() {
		}

		public int getTopicid() {
			return topicid;
		}

		public void setTopicid(int topicid) {
			this.topicid = topicid;
		}

		public int getReplys() {
			return replys;
		}

		public void setReplys(int replys) {
			this.replys = replys;
		}

		public int getViews() {
			return views;
		}

		public void setViews(int views) {
			this.views = views;
		}

	}

	public static class RVList {
		public RVList() {
		}

		private List<RV> jsonprv = null;

		public List<RV> getJsonprv() {
			return jsonprv;
		}

		public void setJsonprv(List<RV> jsonprv) {
			this.jsonprv = jsonprv;
		}

	}
	
	private void getReplyAndViews(String ids, HttpDownloader downloader) throws JsonParseException, JsonMappingException, IOException {
		String request = "http://club.ajax.autohome.com.cn/topic/rv?fun=jsonprv&callback=jsonprv&ids="+ids;
		String res = downloader.downloadAsString(request);
		if(res == null) return;
		String jstr = "{" + res.replace("jsonprv(", "\"jsonprv\":").replace(")", "") + "}";
		RVList rvlist = new ObjectMapper().readValue(jstr, RVList.class);
		if(rvlist == null || rvlist.getJsonprv() == null){
			return ;
		}
		List<RV> rvs = rvlist.getJsonprv();
		for(RV rv :rvs){
			Post post = posts.get(rv.getTopicid()+"");
			if(post != null){
				post.setHits(rv.getViews());
				post.setReplies(rv.getReplys());
			}
		}
		
	}

	private String genPosts(Object[] dls) throws XPatherException {
		if (dls == null)
			return null;
		StringBuilder sbuilder = new StringBuilder();
		for (Object dl : dls) {
			TagNode node = (TagNode)dl;
			Post post = new Post();
			String lang = node.getAttributeByName("lang");
			String[] lang_pieces = lang.split("\\|");
			post.setTime(lang_pieces[4]);
			post.setAuthor(lang_pieces[10]);
			String url = "/bbs/thread-" + lang_pieces[0] + "-" + lang_pieces[1]
					+ "-" + lang_pieces[2] + "-1.html";
			post.setUrl(url);

			Object[] obs = node.evaluateXPath("//dt//a[@href='" + url + "']");
			if (obs == null)
				continue;
			post.setTitle(((TagNode) obs[0]).getText().toString().trim());

			posts.put(lang_pieces[2],post);
			if(sbuilder.length() > 0){
				sbuilder.append(",");
			}
			
			sbuilder.append(lang_pieces[2]);
		}
		return sbuilder.toString();
	}


	public void output(String path) throws UnsupportedEncodingException, FileNotFoundException{
		MapSorter<String, Post> sorter = new MapSorter<String, AutoHomeClubJXDownloader.Post>();
		List<Entry<String,Post>> lst = sorter.sortByValueDSC(posts);
		LineFileWriter writer = new LineFileWriter(path);
		writer.writeLine(lst);
		writer.close();
	}
	
	public void tidy(String path) throws UnsupportedEncodingException, FileNotFoundException{
		LineFileReader reader = new LineFileReader(path);
		reader.clearAnnotations();
		List<String> lst = new ArrayList<String>(90000);
		String line = null;
		while((line = reader.nextLine())!= null){
			if(!lst.contains(";&#"))
				lst.add(line);
		}
		reader.close();
		
		LineFileWriter writer = new LineFileWriter(path);
		writer.writeLine(lst);
		writer.close();
	}
	
	public static void main(String[] args) throws XPatherException, IOException, InterruptedException {
		AutoHomeClubJXDownloader downloader = new AutoHomeClubJXDownloader();
		for(int i = 1; i < 35; i++){
			MSTimer timer = new MSTimer();
			if(!downloader.downloadAreaLists(100000+i)){
				break;
			}
			System.out.println("succeed download area:"+(100000+i)+" used:"+timer);
		}
		
		downloader.output("E:/tmp/autohome/autohome_club_jx.txt");
		downloader.tidy("E:/tmp/autohome/autohome_club_jx.txt");
		
	}

}
