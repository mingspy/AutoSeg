package com.mingspy.tokenizers;

import java.io.IOException;
import java.io.Reader;
import java.util.Map;

import org.apache.lucene.analysis.Tokenizer;
import org.apache.lucene.analysis.util.TokenizerFactory;
import org.apache.lucene.util.AttributeSource.AttributeFactory;

public class JsegTokenizerFactory extends TokenizerFactory{
	protected JsegTokenizerFactory(Map<String, String> args) {
		super(args);
	}

	private ThreadLocal<JsegTokenizer> tokenizerLocal = new ThreadLocal<JsegTokenizer>();
	
	
	private JsegTokenizer newTokenizer(Reader input) {
		JsegTokenizer tokenizer = new JsegTokenizer(input);
		tokenizerLocal.set(tokenizer);
		return tokenizer;
	}


	@Override
	public Tokenizer create(AttributeFactory arg0, Reader input) {
		JsegTokenizer tokenizer = newTokenizer(input);
	    return tokenizer;
	}
}
