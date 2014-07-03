package com.mingspy.tokenizers;

import java.io.IOException;
import java.io.Reader;
import java.util.Iterator;
import java.util.List;

import org.apache.lucene.analysis.Tokenizer;
import org.apache.lucene.analysis.tokenattributes.CharTermAttribute;
import org.apache.lucene.analysis.tokenattributes.OffsetAttribute;
import org.apache.lucene.analysis.tokenattributes.TypeAttribute;

import com.mingspy.jseg.AutoTokenizer;
import com.mingspy.jseg.ITokenizer;
import com.mingspy.jseg.Token;

public class JsegTokenizer extends Tokenizer {

	private CharTermAttribute termAtt;
	private OffsetAttribute offsetAtt;
	private TypeAttribute typeAtt;
	private List<Token> _splitResult;
	//分词结果 
    private Iterator<Token> tokens; 
    ITokenizer tokenizer = new AutoTokenizer();
	public JsegTokenizer(Reader input) {
		super(input);
		termAtt = addAttribute(CharTermAttribute.class);
		offsetAtt = addAttribute(OffsetAttribute.class);
		typeAtt = addAttribute(TypeAttribute.class);
		readText(input);		
	}
	
	private void readText(Reader input){
		StringBuilder sb = new StringBuilder();
		char[] buf = new char[8192];
		int d = -1;
		try {
			while((d=input.read(buf)) != -1) {
			    sb.append(buf, 0, d);
			}
			_splitResult = tokenizer.splitTokens(sb.toString());
			tokens = _splitResult != null?_splitResult.iterator():null;			
		}catch(Exception e) {
			tokens = null;
			e.printStackTrace();			
		}
	}
	
	@Override
	public boolean incrementToken() throws IOException {
		clearAttributes();
		Token token = null;
		if ( tokens != null && tokens.hasNext() ) {
			token = tokens.next(); 
			termAtt.append(token.word);
			termAtt.copyBuffer(token.word.toCharArray(), 0, token.len);
			offsetAtt.setOffset(token.off, token.off + token.len);
			typeAtt.setType("word");
			return true;
		} else {			
			end();
			return false;
		}
	}

	@Override
	public void reset(Reader input) throws IOException {
		// TODO Auto-generated method stub
		super.reset(input);
		readText(input);
	}

	@Override
	public void reset() throws IOException {
		// TODO Auto-generated method stub
		super.reset();
		if(_splitResult != null){
			tokens = _splitResult.iterator();
		}
	}
	
	
	
	
}
