package com.mingspy.utils;

import java.io.IOException;

import org.junit.Assert;
import org.junit.Test;

import com.mingspy.utils.html.HttpDownloader;

public class HttpDownloaderTest {
	
	@Test
	public void downloadBaidu() throws IOException{
		HttpDownloader downloader = new HttpDownloader();
		String str = downloader.downloadAsString("http://www.baidu.com");
		Assert.assertNotNull(str);
		System.out.println(str);
	}
}
