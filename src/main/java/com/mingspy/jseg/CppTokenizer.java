package com.mingspy.jseg;

import java.util.ArrayList;
import java.util.List;

/**
 * 调用底层cpp实现的分词器。
 * 由于安装分词不太方便，废弃掉这种方式。改用纯java编写。
 * @author xiuleili
 *
 */
@Deprecated
public class CppTokenizer implements ITokenizer
{

    @Override
    public List<String> maxSplit(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.MaxSplit(str);
        return genWords(str, res);
    }


    @Override
    public List<String> fullSplit(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.FullSplit(str);
        return genWords(str, res);
    }

    @Override
    public List<String> uniGramSplit(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.UniGramSplit(str);
        return genWords(str, res);
    }

    private List<String> genWords(String str, List<Token> res)
    {
        ArrayList<String> words = new ArrayList<String>(res.size());
        for(Token t:res) {
            words.add(str.substring(t.off, t.off + t.len));
        }
        return words;
    }


    @Override
    public List<String> biGramSplit(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.BiGramSplit(str);
        return genWords(str, res);
    }


    @Override
    public List<String> mixSplit(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.MixSplit(str);
        return genWords(str, res);
    }

    @Override
    public List<Token> POSTagging(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.POSTagging(str);
        for(Token t:res) {
            t.word = str.substring(t.off, t.off + t.len);
        }

        return res;
    }


    @Override
    public List<Token> splitTokens(String str)
    {
        if(str == null || str.isEmpty()) {
            return null;
        }

        List<Token> res = JSegJNI.FullSplit(str);
        for(Token t:res) {
            t.word = str.substring(t.off, t.off + t.len);
        }

        return res;
    }





}
