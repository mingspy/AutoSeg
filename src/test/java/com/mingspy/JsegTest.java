package com.mingspy;

import org.junit.Ignore;
import org.junit.Test;

import com.mingspy.jseg.CppTokenizer;

public class JsegTest
{
    CppTokenizer tokenizer = new CppTokenizer();
    @Ignore
    @Test
    public void testAutoSeg()
    {

        String ss = "他说的确实在理";
        String ss1 = "李岚清将在年会期间出席中国"
                     +"经济专题讨论会和世界经济论坛关于中国经济问题的全会，并在全会上发表演讲。他还将在"
                     +"这里会见世界经济论坛主席施瓦布和出席本次年会的联合国秘书长安南、瑞士联邦主席兼外长"
                     +"科蒂、一些其他国家的国家元首和政府首脑以及国际组织的领导人，并同他们就中国和世界经济发展问题交换看法。";

        //---------------------------------
        doTestSplit(ss);
        System.out.println("------------------------------------------------------------");
        doTestSplit(ss1);
        System.out.println("mixSplit=>"+tokenizer.mixSplit("奥迪q7多少钱"));
        System.out.println(tokenizer.maxSplit("中国"));
    }

    private void doTestSplit(String ss)
    {
        System.out.println("maxSplit=>"+tokenizer.maxSplit(ss));
        System.out.println("fullSplit=>"+tokenizer.fullSplit(ss));
        System.out.println("unigram=>"+tokenizer.uniGramSplit(ss));
        System.out.println("bigram=>"+tokenizer.biGramSplit(ss));

        System.out.println(tokenizer.POSTagging(ss));
    }

    @Test

    public void testUserDicts()
    {
        doTestSplit("宝马多少钱");
    }

    @Test
    @Ignore
    public void testAmbiguous()
    {
        //doTestSplit("北京大学");
        //doTestSplit("北京大学生");
        doTestSplit("发展中国家");
    }
}
