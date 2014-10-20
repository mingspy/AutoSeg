package com.mingspy.nlp.spam;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.ResourceBundle;
import java.util.Set;

import com.mingspy.nlp.SpliterUtils;
import com.mingspy.nlp.StopWords;
import com.mingspy.utils.io.LineFileReader;

public class SpamProbTrainer
{

    public class Freq
    {
        public int badFreq;
        public int goodFreq;
    }

    private Map<String, Freq> wordFreqMap = new HashMap<String, Freq>();
    private Map<String, Double> PGProbMap = new HashMap<String, Double>();
    private int badWordsTotal;
    private int goodWordsTotal;
    private int badDocTotal;
    private int goodDocTotal;
    private Map<String, Freq> docFreqMap = new HashMap<String, Freq>();
    private Map<String, Double> spamprobMap = new HashMap<String, Double>();
    private Map<String, Double> healthprobMap = new HashMap<String, Double>();



    /**
     * (let ((g (* 2 (or (gethash word good) 0))) <br>
     * (b (or (gethash word bad) 0))) <br>
     * (unless (&lt (+ g b) 5) <br>
     * (max .01 (min .99 <br>
     * (float (/ (min 1 (/ b nbad))<br>
     * (+ (min 1 (/ g ngood)) (min 1 (/ b nbad))))) ) ) ) )
     *
     * @see paul{@link http://www.paulgraham.com/spam.html}
     * @param path
     * @throws IOException
     */
    public void genPaulGrahamProb(String path) throws IOException
    {
        double priorBad = 0.5;// badDocTotal / (badDocTotal + goodDocTotal +
        // 10.0);
        System.out.println("badPorb = " + priorBad + " bad docs:" + badDocTotal
                           + " good docs:" + goodDocTotal);
        for (Entry<String, Freq> entry : wordFreqMap.entrySet()) {
            Freq freq = entry.getValue();
            if (freq.badFreq + freq.goodFreq < 5) {
                continue;
            }

            double probbad = Math.min(1, freq.badFreq / (badWordsTotal + 1.0))
                             * priorBad;
            double probgood = Math.min(1, freq.goodFreq * 2
                                       / (goodWordsTotal + 1.0))
                              * (1 - priorBad);
            double prob = Math.min(0.99, probbad / (probbad + probgood));
            prob = Math.max(0.01, prob);

            PGProbMap.put(entry.getKey(), prob);
        }

        FileOutputStream fileOutputStream = new FileOutputStream(path);
        ObjectOutputStream writer = new ObjectOutputStream(fileOutputStream);
        writer.writeObject(PGProbMap);
        writer.close();
    }

    public void genBayesProb(String path) throws IOException
    {
        double priorBad = badDocTotal / (badDocTotal + goodDocTotal + 10.0);
        for (Entry<String, Freq> entry : docFreqMap.entrySet()) {
            Freq freq = entry.getValue();
            if (freq.badFreq + freq.goodFreq < 5) {
                continue;
            }

            double probbad = Math.min(1, freq.badFreq / (badDocTotal + 1.0));
            spamprobMap.put(entry.getKey(), probbad);
            double probhealth = Math.min(1, freq.goodFreq
                                         / (goodDocTotal + 1.0));
            healthprobMap.put(entry.getKey(), probhealth);
        }

        FileOutputStream fileOutputStream = new FileOutputStream(path);
        ObjectOutputStream writer = new ObjectOutputStream(fileOutputStream);
        writer.writeObject(spamprobMap);
        writer.writeObject(healthprobMap);
        writer.writeDouble(priorBad);
        writer.close();
    }

    /**
     * 统计标注的文本。文本格式为<br>
     * 分类标签<br>
     * 文档1<br>
     * 分类标签2<br>
     * 文档2<br>
     * ....<br>
     * 其中分类标签用@class:spam 或 @class:health标注。
     *
     * @param markedfolder
     * @param coding
     */
    public void statisticMarkedFiles(String markedfolder, String coding)
    {
        File dataDir = new File(markedfolder);
        File[] files = null;
        if (dataDir.isDirectory()) {
            files = dataDir.listFiles();
        } else {
            files = new File[1];
            files[0] = dataDir;
        }
        for (File file : files) {
            System.out.println("statisticMarkedFiles folder:" + file.getName());
            statisticAMarkedFile(file, coding);
        }
    }

    private void addWordFreq(Map<String, Freq> map, String word, int freq,
                             boolean isBad)
    {
        if(StopWords.contains(word)) {
            return;
        }

        Freq wordfreq = map.get(word);
        if (wordfreq == null) {
            wordfreq = new Freq();
            map.put(word, wordfreq);
        }

        if (isBad) {
            wordfreq.badFreq += freq;
            badWordsTotal += freq;
        } else {
            wordfreq.goodFreq += freq;
            goodWordsTotal += freq;
        }
    }

    /**
     * 统计一个普通文档,每一行一个文档
     *
     * @param file
     * @param isbad
     * @param codding
     */
    public void statisticAFile(File file, boolean isbad, String codding)
    {
        LineFileReader reader = new LineFileReader(file.getAbsolutePath(),
                codding);

        int count = 0;
        String line = null;
        while ((line = reader.nextLine()) != null) {
            // line = line.replace(" ", "");
            // System.out.println(line);
            // 只统计包含中文的词。
            line = SpliterUtils.stem(line);
            //System.out.println("line"+count+"->"+line);
            List<String> words = SpliterUtils.split(line);
            if(words == null) continue;
            Set<String> docword = new HashSet<String>();
            for (String word : words) {

                addWordFreq(wordFreqMap, word, 1, isbad);
                docword.add(word);
            }

            if(++count % 10000 == 0) {
                System.out.println("handled lines:"+count);
            }

            if (isbad) {
                badDocTotal++;
            } else {
                goodDocTotal++;
            }

            for (String w : docword) {
                addWordFreq(docFreqMap, w, 1, isbad);
            }
        }

    }

    /**
     * 统计一个标注的文件，一个文件中可能包含多篇文章。
     *
     * @param file
     * @param codding
     */
    private void statisticAMarkedFile(File file, String codding)
    {
        LineFileReader reader = new LineFileReader(file.getAbsolutePath(),
                codding);
        String line = null;
        String header = null;
        StringBuilder builder = new StringBuilder();
        while ((line = reader.nextLine()) != null) {
            if (line.contains("@class:")) {
                statisticDoc(header, builder);
                builder = new StringBuilder();
                header = line;
                continue;
            }
            //line = line.replace(" ", "");

            // System.out.println(line);
            // 只统计包含中文的词。
            builder.append(line);
        }

        statisticDoc(header, builder);
    }



    private void statisticDoc(String header, StringBuilder builder)
    {
        if (header == null || builder.length() == 0)
            return;
        String line = SpliterUtils.stem(builder.toString());
        List<String> words = SpliterUtils.split(line);
        if (words != null) {
            boolean isBad = header.contains("@class:spam");
            if (isBad) {
                badDocTotal++;
            } else {
                goodDocTotal++;
            }
            Set<String> docwords = new HashSet<String>();
            for (String word : words) {
                // System.out.println(word);
                // System.out.println("->added");
                addWordFreq(wordFreqMap, word, 1, isBad);
                docwords.add(word);
            }
            for (String word : docwords) {
                addWordFreq(docFreqMap, word, 1, isBad);
            }
        }
    }

    private String makepath(String dir, String path)
    {
        String fullpath = dir.replace('\\', '/') + "/"
                          + path.replace('\\', '/');
        fullpath = fullpath.replace("//", "/");
        fullpath = fullpath.replace("//", "/");
        return fullpath;
    }

    public static void main(String[] args) throws IOException
    {
        /*String line = "<body>1234 www.baidu.com http://dd.org  mingpsy@163.com 大中国 </body>";
        line = SpliterUtils.stem(line);
        System.out.println(line);
        */


        SpamProbTrainer trainer = new SpamProbTrainer();
        ResourceBundle bundle = ResourceBundle.getBundle("spam");
//		String markedFolder = bundle.getString("SPAM_TRAIN_MARKED_FOLDER");
//		if (markedFolder != null) {
//			trainer.statisticMarkedFiles(markedFolder, "utf-8");
//		}


        System.out.println("stastic spam files");
        trainer.statisticAFile(new File("E:/tmp/spam/saa/spam_questions.txt"), true, "utf-8");


        System.out.println("stastic health files");
        trainer.statisticAFile(new File("E:/tmp/spam/saa/health_questions.txt"), false, "utf-8");





        // trainer.statisticTrecSpamWords(bundle.getString("SPAM_TRAIN_SPAM_FOLDER"),
        // "gbk");
        System.out.println("genPaulGrahamProb");
        trainer.genPaulGrahamProb(bundle.getString("SPAM_PROB_TABLE_PATH"));
        System.out.println("genBayesProb");
        trainer.genBayesProb(bundle.getString("SPAM_NBPROB_TABLE_PATH"));
        System.out.println("done!");

    }
}
