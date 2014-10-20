package com.mingspy.nlp.spam;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.UnsupportedEncodingException;
import java.util.Random;

import com.mingspy.utils.MSTimer;
import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

public class SpamFilterEstimator
{

    int corrects = 0;
    int goodCorrects = 0;
    int goodTototal = 0;
    int docs = 0;

    /**
     * 估计SpamFilter的准确率。<br>
     * 测试文件格式固定为用一行标识文本信息，必须开头用@class标识文本内容垃圾还是正常文件，<br>
     * spam表示垃圾，health表示正常。<br>
     * @param filter
     * @param testDataFolder
     * @return
     * @throws Exception
     */
    public double estimate(ISpamFilter filter, String testDataFolder) throws Exception
    {
        MSTimer timer = new MSTimer();
        File folder = new File(testDataFolder);
        File[] files = null;
        if (folder.isDirectory()) {
            files = folder.listFiles();
        } else {
            files = new File[1];
            files[0] = folder;
        }

        for (File file : files) {
            // load test datas
            LineFileReader reader = new LineFileReader(file.getAbsolutePath());
            String line = null;
            StringBuilder doc = new StringBuilder();
            String docinfo = null;
            while ((line = reader.nextLine()) != null) {
                if (line.contains("@class:")) {
                    estimateDoc(filter, doc, docinfo);
                    doc = new StringBuilder();
                    docinfo = line.trim();
                    continue;
                }

                doc.append(line.trim().replace(" ", ""));
            }

            estimateDoc(filter, doc, docinfo);
            reader.close();
        }

        double precision = corrects / (docs + 0.0001);
        System.out.println("\n======================="+filter.getFilterName()+"================================");
        System.out.println("total test docs:" + docs + " estimate corrects:" + corrects + " precision:"
                           + precision);
        System.out.println("total health docs:" + goodTototal + " recognized:" + goodCorrects + " recall:"
                           + (goodCorrects / (goodTototal + 0.000001)));
        System.out.println("total spam docs:" + (docs - goodTototal) + " recognized:"
                           + (corrects - goodCorrects) + " recall:"
                           + ((corrects - goodCorrects) / (docs - goodTototal + 0.0001)));
        System.out.println("used total " + timer);

        return precision;
    }

    private void estimateDoc(ISpamFilter filter, StringBuilder doc, String docinfo)
    {
        if(docinfo == null || doc.length() == 0) return;
        docs++;
        double prob = filter.filter(doc.toString());
        boolean testIsSpam = filter.isSpamProb(prob);
        boolean docIsSpam = docinfo.contains(":spam");

        if (!docIsSpam) {
            goodTototal++;
        }
        if (testIsSpam == docIsSpam) {
            corrects++;
            if (!docIsSpam) {
                goodCorrects++;
            }
        }
    }

    public static void prepairTestDoc(String inpath, String outpath, boolean isSpam) throws UnsupportedEncodingException, FileNotFoundException
    {
        LineFileReader reader = new LineFileReader(inpath);
        LineFileWriter writer = new LineFileWriter(outpath);
        String line = null;
        Random r = new Random();
        String header = null;
        if(isSpam) {
            header = "@class:spam";
        } else {
            header = "@class:health";
        }
        while((line = reader.nextLine()) != null) {
            if(r.nextInt() % 10 == 1) {
                writer.writeLine(header);
                writer.writeLine(line);
            }
        }
    }
    public static void main(String[] args) throws Exception
    {
        prepairTestDoc("e:/tmp/spam/saa/spam_questions.txt","e:/tmp/spam/saa/estimate/estimate_spam.txt", true);
        prepairTestDoc("e:/tmp/spam/saa/health_questions.txt","e:/tmp/spam/saa/estimate/estimate_health.txt", false);
        new SpamFilterEstimator().estimate(new PGSpamFilter(), "e:/tmp/spam/saa/estimate/");
        new SpamFilterEstimator().estimate(new NBSpamFilter(), "e:/tmp/spam/saa/estimate/");
    }
}
