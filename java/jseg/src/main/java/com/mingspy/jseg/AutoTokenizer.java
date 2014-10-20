package com.mingspy.jseg;

import java.util.ArrayList;
import java.util.List;

public class AutoTokenizer implements ITokenizer
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


    public static void main(String[] args)
    {
        AutoTokenizer tokenizer = new AutoTokenizer();

        System.out.println(tokenizer.maxSplit("他说的确实在理"));
        System.out.println(tokenizer.fullSplit("他说的确实在理"));
        System.out.println(tokenizer.uniGramSplit("他说的确实在理"));
        System.out.println(tokenizer.biGramSplit("他说的确实在理"));
        System.out.println(tokenizer.mixSplit("奥迪q7多少钱"));
        System.out.println(tokenizer.POSTagging("李岚清将在年会期间出席中国"
                                                +"经济专题讨论会和世界经济论坛关于中国经济问题的全会，并在全会上发表演讲。他还将在"
                                                +"这里会见世界经济论坛主席施瓦布和出席本次年会的联合国秘书长安南、瑞士联邦主席兼外长"
                                                +"科蒂、一些其他国家的国家元首和政府首脑以及国际组织的领导人，并同他们就中国和世界经济发展问题交换看法。"));
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
