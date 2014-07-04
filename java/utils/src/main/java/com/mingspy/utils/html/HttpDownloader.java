package com.mingspy.utils.html;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.NoSuchElementException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.GZIPInputStream;

import org.apache.commons.httpclient.Header;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.SimpleHttpConnectionManager;
import org.apache.commons.httpclient.URI;
import org.apache.commons.httpclient.URIException;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.methods.PostMethod;
import org.apache.commons.httpclient.params.HttpMethodParams;
import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.apache.log4j.Logger;

public class HttpDownloader {

	private static final Logger logger = Logger.getLogger(HttpDownloader.class);
	private static final String USER_AGENT = " Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; CIBA; .NET CLR 2.0.50727; MAXTHON 2.0)";
	private static final String ACCEPT = "image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, application/x-shockwave-flash, */*";

	private Map<String, String> requestHeaders = new HashMap<String, String>();

	public void add(String headName, String headValue) {
		this.requestHeaders.put(headName, headValue);
	}

	public final HttpClient client = new HttpClient(
			new SimpleHttpConnectionManager(true));

	public void download(String urlStr, StringBuffer returnString)
			throws IOException {
		this.download(urlStr, "utf-8", returnString, null);

	}

	private GetMethod excuteGet(String urlStr, String urlCharset)
			throws IOException {
		GetMethod get = new GetMethod();
		URI uri = new URI(urlStr, true, urlCharset);
		get.setURI(uri);

		get.addRequestHeader("Connection", "close");
		get.addRequestHeader("User-Agent", USER_AGENT);
		get.setRequestHeader("Accept", ACCEPT);
		get.getParams().setParameter(HttpMethodParams.SO_TIMEOUT, 30 * 1000);

		for (Entry<String, String> entry : this.requestHeaders.entrySet()) {
			get.setRequestHeader(entry.getKey(), entry.getValue());
		}

		// client.getHostConfiguration().setProxy("10.16.0.186", 8000);
		// UsernamePasswordCredentials creds = new
		// UsernamePasswordCredentials("auto","auto");
		// client.getState().setProxyCredentials(AuthScope.ANY, creds);
		//

		if (client.executeMethod(get) != HttpStatus.SC_OK) {
			System.err.println("Method failed: " + get.getStatusLine());
			return null;
		}
		return get;
	}

	private InputStream getStream(GetMethod get) throws IOException {
		if (get == null)
			return null;
		InputStream resultStream = null;
		Header content_encoding_header = get
				.getResponseHeader("content-encoding");
		if (content_encoding_header != null
				&& content_encoding_header.getValue().equals("gzip")) {
			resultStream = new GZIPInputStream(get.getResponseBodyAsStream());
		} else {
			resultStream = get.getResponseBodyAsStream();
		}
		return resultStream;
	}

	public InputStream downloadAsStream(String urlStr, String urlCharset) {
		InputStream resultStream = null;
		try {
			GetMethod get = excuteGet(urlStr, urlCharset);
			resultStream = getStream(get);
		} catch (Throwable e) {
			logger.error(e.getMessage(), e);
		}

		return resultStream;
	}

	public void download(String urlStr, String urlCharset,
			StringBuffer returnString, String charset) {
		InputStream stream = null;
		String result = null;
		GetMethod get = null;
		try {
			get = excuteGet(urlStr, urlCharset);
			if (get == null) {
				return;
			}
			stream = getStream(get);
			byte[] responseBody = IOUtils.toByteArray(stream);
			if ((charset == null)
					&& get.getResponseHeader("Content-Type") != null) {
				charset = getProp("charset=(.*)",
						get.getResponseHeader("Content-Type").getValue(), 1);
			}
			if (charset == null || charset.equals("off")) {
				if (isUTF8(responseBody)) {
					charset = "utf-8";
				}
			}

			if (charset == null) {
				result = new String(responseBody);
			} else {
				result = new String(responseBody, charset.trim());
			}
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		} finally {
			// Release the connection.
			if (get != null)
				get.releaseConnection();
			if (returnString != null) {
				returnString.append(result);
			}
		}
	}

	public InputStream downloadPost(String urlStr, StringBuffer returnString,
			String charset, String urlCharset) throws IOException {

		PostMethod get = new PostMethod();
		// get.setRequestHeader("Referer", urlStr);
		// get.addParameter("ctl00$btn$txtSearch", "中国");
		// get.addParameter("ctl00$btn$btnSearch", "Search");
		// ctl00$btn$txtSearch=中国&ctl00$btn$btnSearch=Search
		InputStream resultStream = null;
		URI uri = new URI(urlStr, false, urlCharset);
		get.setURI(uri);

		get.addRequestHeader("Connection", "close");
		get.addRequestHeader("User-Agent", USER_AGENT);

		get.setRequestHeader("Accept", ACCEPT);

		for (Entry<String, String> entry : this.requestHeaders.entrySet()) {
			get.setRequestHeader(entry.getKey(), entry.getValue());
		}

		String results = null;

		try {

			int statusCode = client.executeMethod(get);
			if (statusCode != HttpStatus.SC_OK) {
				System.err.println("Method failed: " + get.getStatusLine());
				return null;
			}
			byte[] responseBody = get.getResponseBody();

			if ((charset == null)
					&& get.getResponseHeader("Content-Type") != null) {
				charset = getProp("charset=(.*)",
						get.getResponseHeader("Content-Type").getValue(), 1);
			}

			if (charset == null || charset.equals("off")) { // Ĭ��
				if (urlStr.indexOf("ifeng.com") != -1) {
					charset = "utf8";
				}

				if (isUTF8(responseBody)) {
					charset = "utf-8";
				} else {
					charset = "gbk";
				}
			}

			results = new String(responseBody, charset.trim());
			resultStream = get.getResponseBodyAsStream();

		} catch (Throwable e) {
			logger.error(e.getMessage(), e);
		} finally {
			// Release the connection.
			get.releaseConnection();
			if (returnString != null) {
				returnString.append(results);
			}

		}

		return resultStream;
	}

	/*
	 * 简单分析一下该算法，大致就是 11xxxxxx 10xxxxxx good++; 0xxxxxxx 10xxxxxx bad++;
	 * 11xxxxxx 0xxxxxxx bad++; 11xxxxxx 11xxxxxx bad++;
	 */
	public boolean isUTF8(byte[] data) {
		int count_good_utf = 0;
		int count_bad_utf = 0;
		byte current_byte = 0x00;
		byte previous_byte = 0x00;
		for (int i = 1; i < data.length; i++) {
			current_byte = data[i];
			previous_byte = data[i - 1];
			// 10xxxxxx
			if ((current_byte & 0xC0) == 0x80) {
				// 11xxxxxx
				if ((previous_byte & 0xC0) == 0xC0) {
					count_good_utf++;
					// 0xxxxxxx
				} else if ((previous_byte & 0x80) == 0x00) {
					count_bad_utf++;
				}
				// 11xxxxxx
			} else if ((previous_byte & 0xC0) == 0xC0) {
				count_bad_utf++;
			}
		}
		// System.out.println(count_good_utf);
		// System.out.println(count_bad_utf);
		if (count_good_utf > count_bad_utf) {
			return true;
		} else {
			return false;
		}
	}

	public String downloadAsString(String url, String charSet) {
		StringBuffer buffer = new StringBuffer();
		this.download(url, null, buffer, charSet);
		if (buffer.length() > 0)
			return buffer.toString();
		return null;
	}
	
	public String downloadAsString(String url) {
		StringBuffer buffer = new StringBuffer();
		this.download(url, null, buffer, null);
		if (buffer.length() > 0)
			return buffer.toString();
		return null;
	}

	/**
	 * 使用正则来匹配并获得网页中的字符串
	 */
	private static String getProp(String pattern, String match, int index) {
		Pattern sp = Pattern.compile(pattern);
		Matcher matcher = sp.matcher(match);
		while (matcher.find()) {
			return matcher.group(index);
		}
		return null;
	}

	public static String Download(int index, String url, String lang)
			throws IOException {

		HttpDownloader httpDownloader = new HttpDownloader();
		httpDownloader.add("X-Apple-Store-Front", lang);
		// commonHttpDownloader.add("X-Apple-Store-Front","143441-1,5");
		httpDownloader
				.add("Referer",
						"http://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewAutoSourcedGenrePage?id=6018&mt=8");
		httpDownloader.add("Accept-Language",
				"zh-cn, zh;q=0.75, en-us;q=0.50, en;q=0.25");
		httpDownloader.add("X-Apple-Validation",
				"E3264056-300D870906835D5C8B91048A2939D552");
		httpDownloader.add("X-Apple-Tz", "0");
		httpDownloader
				.add("User-Agent",
						"iTunes/9.0.3 (Windows; Microsoft Windows XP Professional Service Pack 3 (Build 2600))AppleWebKit/531.21.8");
		httpDownloader
				.add("Cookie",
						"hsaccnt=1; mz_at0=AQQCAAFfkwAAAABL2RG94T3XVgolIx38U0/gG6r3kZqDnsI=; mzf_in=190804; X-Dsid=1074023676; a=AAAAAgAAB9MAIDU1M2JlODFjN2I2YTM2MmM3MDRmMTk2MzRhZjI0NDNiACrErgAAAShD7xII; Pod=19; s_cc=true; s_sq=%5B%5BB%5D%5D; s_vi=[CS]v1|25D63101850135AC-40000113400DDD4A[CE]; s_vnum_us=ch%3Ditunes.welcomescreen%26vn%3D1%3B; seoReferrer=http%3A%2F%2Fwww.findaproperty.com%2Fsearch-tools%2Fiphone-app%2F");
		httpDownloader.add("Accept-Encoding", "gzip");
		httpDownloader.add("X-Dsid", "1074023676");
		// commonHttpDownloader.add("X-Apple-Store-Front","143465-19,5");
		httpDownloader.add("Host", "ax.itunes.apple.com");
		String content = httpDownloader.downloadAsString(url);
		return content;
		// FileUtils.writeStringToFile(new File("appBoard"+index+".xml"),
		// content);
	}

	// 评论
	// http://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/customerReviews?displayable-kind=11&id=335019557&page-number=2&sort-key=1
	// 儿童专区
	// http://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewRoom?fcId=373218127&id=25129&mt=8
	public static void main(String args[]) throws IOException,
			InterruptedException {

		String content = Download(
				1,
				"http://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/customerReviews?displayable-kind=11&id=290060090&page=1&sort=4",
				null);
		// System.out.println(content);

		// http://ax.search.itunes.apple.com/WebObjects/MZSearch.woa/wa/search?entity=software&media=all&page=3&restrict=true&startIndex=361&term=%E4%B8%AD%E5%9B%BD
		// http://ax.search.itunes.apple.com/WebObjects/MZSearch.woa/wa/search?entity=software&media=all&page=2&restrict=true&startIndex=181&term=%E4%B8%AD%E5%9B%BD
		// http://ax.search.itunes.apple.com/WebObjects/MZSearch.woa/wa/search?entity=software&media=all&restrict=true&submit=seeAllLockups&term=%E4%B8%AD%E5%9B%BD
		// http://ax.search.itunes.apple.com/WebObjects/MZSearch.woa/wa/search?entity=song&media=all&page=2&restrict=true&startIndex=200&term=%E4%B8%AD%E5%9B%BD
		// http://ax.search.itunes.apple.com/WebObjects/MZSearch.woa/wa/advancedSearch?deviceTerm=iPad&entity=software&genreIndex=1&media=software&restrict=true&softwareTerm=%E8%BD%AF%E4%BB%B6&submit=seeAllLockups
		// http://ax.search.itunes.apple.com/WebObjects/MZSearch.woa/wa/advancedSearch?deviceTerm=iPad&entity=software&genreIndex=1&media=software&page=2&restrict=true&softwareTerm=%E4%B8%AD&startIndex=180
		//
		FileUtils.writeStringToFile(new File("d://path"), content);

	}
}
