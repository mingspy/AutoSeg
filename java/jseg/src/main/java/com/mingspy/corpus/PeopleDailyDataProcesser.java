package com.mingspy.corpus;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import java.util.Set;
import java.util.TreeSet;

import com.mingspy.utils.FolderUtils;
import com.mingspy.utils.MSTimer;
import com.mingspy.utils.MapSorter;
import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

public class PeopleDailyDataProcesser
{

    private Map<String, WordInfo> coreWords = new HashMap<String, WordInfo>();
    private Set<String> natures = new TreeSet<String>();
    private boolean compose = false;

    private Map<String, WordInfo> bigramWords = new HashMap<String, WordInfo>();
    private Map<String, WordInfo> lexicalInfo = new HashMap<String, WordInfo>();

    private static final String TMP_FOLDER_NAME = "tmpCorupsAnalysis";
    private static final String NATURE_HEAD = "@WordPOS\t";

    public void trianCorpusData(String directory, String output)
    throws Exception
    {
        MSTimer timer = new MSTimer();
        String tmpDir = FolderUtils.combine(output, TMP_FOLDER_NAME);
        analysisAndGenTmpResults(directory, tmpDir);
        clearWordsInfo();
        System.out.println("trying to combine results...");
        System.gc();
        System.gc();
        combineTmpResults(tmpDir);
        System.out.println("out puts results...");
        outputResult(output);

        FolderUtils.deleteFolder(tmpDir);
        System.out.println("total used: " + timer);
    }

    private void combineTmpResults(String tmpDir)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        List<File> files = FolderUtils.listFiles(tmpDir, false);
        for (File f : files) {
            if (f.getName().startsWith("core")) {
                readWordsInfo(coreWords, f.getAbsolutePath());
            } else if (f.getName().startsWith("bi")) {
                readWordsInfo(bigramWords, f.getAbsolutePath());
            } else if (f.getName().startsWith("lexi")) {
                readWordsInfo(lexicalInfo, f.getAbsolutePath());
            }

            System.out.println("combined " + f.getName());
        }
    }

    private void analysisAndGenTmpResults(String directory, String tmpDir)
    throws Exception, UnsupportedEncodingException,
        FileNotFoundException
    {
        List<File> files = FolderUtils.listFiles(directory, true);
        double totalsize = 0;
        // 由于处理大量文件时，内存和数量集几何增长，耗时较长，改用分步处理。
        // 先写到临时文件，然后在合并。
        for (File file : files) {
            try {
                if (file.isFile()) {
                    FileInputStream fis = new FileInputStream(file);
                    double fsize = fis.available() / 1024 / 1024;
                    fis.close();

                    if (totalsize > 0 && (fsize + totalsize) > 70) {
                        outputTmpResult(tmpDir);
                        clearWordsInfo();
                        totalsize = 0;
                    }

                    System.out.println(System.currentTimeMillis()
                                       + "\tprocessing " + file.getName());
                    processAFile(file);
                    totalsize += fsize;

                }
            } catch (Exception e) {
                System.out.println("error in:" + file);
                throw e;
            }
        }
        outputTmpResult(tmpDir);
    }

    private void clearWordsInfo()
    {
        coreWords.clear();
        bigramWords.clear();
        lexicalInfo.clear();
        natures.clear();

    }

    private void outputTmpResult(String tmpdir)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        if (natures.size() == 0 && coreWords.size() == 0
                && bigramWords.size() == 0 && lexicalInfo.size() == 0) {
            return;
        }
        File dir = new File(tmpdir);
        dir.mkdir();

        Date date = new Date();
        SimpleDateFormat df = new SimpleDateFormat("MMddhhmmss");
        String now = df.format(date);
        writeWordInfo(
            coreWords,
            FolderUtils.combine(dir.getAbsolutePath(), "/corewords_" + now
                                + ".tmp"));
        // prune(bigramWords, 1);
        writeWordInfo(
            bigramWords,
            FolderUtils.combine(dir.getAbsolutePath(), "/biwords_" + now
                                + ".tmp"));

        // prune(lexicalInfo, 1);
        writeWordInfo(
            lexicalInfo,
            FolderUtils.combine(dir.getAbsolutePath(), "/lexical_" + now
                                + ".tmp"));
    }

    private void outputResult(String output)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        if (natures.size() == 0 && coreWords.size() == 0
                && bigramWords.size() == 0 && lexicalInfo.size() == 0) {
            return;
        }

        // prune(coreWords, 1);
        writeWordInfo(coreWords, FolderUtils.combine(output, "/corewords.txt"));
        prune(bigramWords, 1);
        writeWordInfo(bigramWords, FolderUtils.combine(output, "/biwords.txt"));

        prune(lexicalInfo, 1);
        writeWordInfo(lexicalInfo, FolderUtils.combine(output, "/lexical.txt"));
    }

    private void prune(Map<String, WordInfo> coreWords, int limit)
    {
        List<String> keys = new ArrayList<String>(1000);
        for (Entry<String, WordInfo> en : coreWords.entrySet()) {
            if (en.getValue().sumFreq() <= limit) {
                keys.add(en.getKey());
            }
        }

        for (String key : keys) {
            coreWords.remove(key);
        }
    }

    private void writeWordInfo(Map<String, WordInfo> coreWords, String output)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        MapSorter<String, WordInfo> sorter = new MapSorter<String, WordInfo>();
        List<Entry<String, WordInfo>> res = sorter.sortByKeyASC(coreWords);
        // out put
        LineFileWriter writer = new LineFileWriter(output);
        String nature = NATURE_HEAD;
        for (String na : natures) {
            nature += na + ",";
        }
        writer.writeLine(nature);

        for (Entry<String, WordInfo> en : res) {
            writer.writeLine(en.getValue().toString());
        }
        writer.close();
    }

    private void readWordsInfo(Map<String, WordInfo> coreWords, String input)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        // out put
        LineFileReader reader = new LineFileReader(input);
        String line = reader.nextLine();
        if (line == null)
            return;
        int idx = line.indexOf(NATURE_HEAD);
        if (idx < 0)
            return;
        line = line.substring(idx + NATURE_HEAD.length());
        String[] nts = line.split(",");
        for (String nt : nts) {
            natures.add(nt);
        }

        while ((line = reader.nextLine()) != null) {
            int wordIdx = line.indexOf('\t');
            String word = line.substring(0, wordIdx);
            line = line.substring(wordIdx + 1);
            String[] infos = line.split(",");
            for (String inf : infos) {
                int inIdx = inf.indexOf(":");
                String nt = inf.substring(0, inIdx);
                String frq = inf.substring(inIdx + 1);
                addWordInfo(coreWords, word, nt, Integer.parseInt(frq));
                nt = null;
                frq = null;
            }

            infos = null;
            word = null;
        }
        reader.close();
        reader = null;
        System.gc();
    }

    private void processAFile(File file) throws Exception
    {
        BufferedReader reader = null;
        try {
            // String coding = Utilities.getFileCoding(file);
            reader = new BufferedReader(new InputStreamReader(
                                            new FileInputStream(file), "utf-8"));
            String line = null;
            while ((line = reader.readLine()) != null) {
                List<SplitWord> splited_words = changeToSplitedWord(line.trim());
                if (splited_words == null) {
                    continue;
                }

                trianCorpusFromMarkedWords(splited_words);
            }

        } catch (IOException ee) {
            ee.printStackTrace();
        } finally {
            try {
                if (reader != null)
                    reader.close();
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
        }
    }

    private void trianCorpusFromMarkedWords(List<SplitWord> words)
    {
        // 提取核心词典信息
        extractCoreWords(words);

        // 提取二元词典信息
        extractBigramWords(words);

        // 提取词性信息
        extractLexicalContext(words);

        // 提取人名词典
        // extractPersonNameWords(words);
        // 提取组织机构词典
        // extractOrganizationWords(words);
        // 提取译名词典
    }

    /**
     * 提取标注的字串,按顺序加入词链
     *
     * @param line
     *            is same like
     *            "19980131-04-006-001/m  飞雪/n  迎/v  春/Tg  到/v  心潮/n  逐/Vg  浪/n  高/a  （/w  书法/n  ）/w"
     * @return
     * @throws Exception
     */
    private List<SplitWord> changeToSplitedWord(String line) throws Exception
    {
        String[] marked_words = null;
        int start_idx = 0;
        if (line.contains("19980")) {
            marked_words = line.split("  ");
            start_idx = 1;
        } else { // 2014年的
            marked_words = line.split(" ");
            start_idx = 0;
        }

        List<SplitWord> splited_words = new LinkedList<SplitWord>();

        try {

            for (int i = start_idx; i < marked_words.length; i++) {
                if (marked_words[i].isEmpty()) {
                    continue;
                }
                int idx = marked_words[i].lastIndexOf('/');
                if (idx < 0) {
                    continue;
                }

                String word = marked_words[i].substring(0, idx);
                String nature = marked_words[i].substring(idx + 1);
                if (word.isEmpty() || nature.isEmpty()) {
                    continue;
                }

                // handle [中央/n 人民/n 广播/vn 电台/n]nt
                // handle [中央/n 人民/n 广播/vn 电台/n]/nt
                if (word.startsWith("[") && word.length() > 1) {
                    splited_words.add(new SplitWord("[", "w"));
                    word = word.substring(1);
                }

                // 电台/n]nt => word = 电台 nature = n]nt
                if (nature.contains("]")) {
                    String[] nas = nature.split("]");
                    splited_words.add(new SplitWord(word, nas[0]));
                    word = "]";
                    nature = nas[1];
                }
                // 电台/n]/nt word = 电台/n], nature = nt
                if (word.length() > 1 && word.charAt(word.length() - 1) == ']') {
                    int idx2 = word.lastIndexOf('/');
                    if (idx2 > 0) {
                        String w2 = word.substring(0, idx2);
                        String na2 = word
                                     .substring(idx2 + 1, word.length() - 1);
                        splited_words.add(new SplitWord(w2, na2));
                        word = "]";
                    }

                }

                splited_words.add(new SplitWord(word, nature));

            }
        } catch (Exception e) {
            System.out.println("error with:" + line);
            throw e;
        }
        return splited_words.isEmpty() ? null : splited_words;
    }

    /**
     * 提取核心词典信息 先提取出非特殊词 加入词库由于人名的姓和名是分开的，所以对于人名进行合并，加入词库，并增加PER的出现次数。
     * 对于地名和组织机构名，不合并，加入核心词典, 然后增加对应ORG和LOC的频率。 时间和数词不加入核心词典，只增加NUM,TIME的频率。
     *
     * @param splited_words
     */
    private void extractCoreWords(final List<SplitWord> words)
    {
        // TODO 训练实际的分词时，决定是否组合专有名词和人名。设置compose = true;则组合。
        int composeIndex = -1;
        for (int i = 0; i < words.size(); i++) {
            SplitWord sw = words.get(i);
            String word = sw.getWord();

            if (word.equals("[")) {
                composeIndex = i + 1;
                continue;
            }

            String nature = sw.getFlag();
            if (nature.equals("%")) {
                int idx = nature.indexOf('/');
                if (idx > 0) {
                    nature = nature.substring(0, idx);
                    sw.setFlag(nature);
                }
            }

            if (compose) {
                // 如果是人名，则把人名合并
                if (nature.equals("nr") && (i + 1) < words.size()) {
                    sw = words.get(i + 1);
                    if (!sw.getFlag().equals("nr")) {
                        word += sw.getWord();
                        i++;
                    }
                }
            }
            // 如果词非空,不是标点
            // 则加入核心词库.
            if (!word.isEmpty() && !word.equals("]")) {
                addWordInfo(coreWords, word, nature, 1);
            }

            // 添加组合词
            if (compose) {
                if (word.equals("]")) {
                    word = "";
                    if (composeIndex >= 0) {
                        for (int j = composeIndex; j < i; j++) {
                            if (!words.get(j).getFlag().equals("w"))
                                word += words.get(j).getWord();
                        }
                        addWordInfo(coreWords, word, nature, 1);
                    }
                    composeIndex = -1;
                }
            }
        }

    }

    /**
     * 提取二元共献词典 把属于特殊词类的所有词都替换成特殊词的标记，进行统计。 其他按正常词类计算。标点不统计，作为切分点。
     * 停用词也不统计，作为切分点。 不加开始结束标志，亦可正确分词。
     *
     * @param words
     */
    private void extractBigramWords(final List<SplitWord> words)
    {
        String currWord = null;
        String nextWord = null;
        for (currentIndex = 0; currentIndex < words.size() - 1;) {
            // 获取当前词
            if (currWord == null) {
                currWord = nextBigramWord(words);
            }

            // 获取下一词
            nextWord = nextBigramWord(words);
            if (currWord != null && nextWord != null) {
                String biWord = currWord + "@" + nextWord;
                addWordInfo(bigramWords, biWord, WordInfo.FIELD_TWOFREQ, 1);
            }
            // 把下一词赋值给当前词
            currWord = nextWord;
            nextWord = null;
        }
    }

    private int currentIndex = 0;

    private String nextBigramWord(final List<SplitWord> words)
    {
        if (currentIndex >= words.size())
            return null;
        SplitWord sw = words.get(currentIndex++);
        String currWord = sw.getWord();

        // 处理组合词 [ ]
        if (compose && currWord.equals("[")) {
            currWord = "";
            while (currentIndex < words.size()) {
                sw = words.get(currentIndex++);
                if (sw.getWord().equals("]")) {
                    break;
                }

                currWord += sw.getWord();

                String nxtWord = words.get(currentIndex).getWord();
                if (!nxtWord.equals("]")) {
                    addWordInfo(bigramWords, sw.getWord() + "@" + nxtWord,
                                WordInfo.FIELD_TWOFREQ, 1);
                }
            }
        }

        if (!compose && currWord.equals("]")) {
            return nextBigramWord(words);
        }

        String flag = sw.getFlag();
        if (flag.equals("w")) {
            if (currWord.equals("["))
                return nextBigramWord(words);
            // 标点符号去掉
            return null;
        }

        // 如果是人名，则把人名合并

        if (compose && flag.equalsIgnoreCase("nr")
                && currentIndex < words.size()) {
            sw = words.get(currentIndex);
            if (!sw.getFlag().equalsIgnoreCase("nr")) {
                currWord += sw.getWord();
                currentIndex++;
            }

        }

        return currWord;
    }

    private void addWordInfo(Map<String, WordInfo> coreWords, String word,
                             String nature, int frq)
    {
        natures.add(nature);
        WordInfo info = coreWords.get(word);
        if (info == null) {
            info = new WordInfo(word);
        }
        info.addNature(nature, frq);
        coreWords.put(word, info);
    }

    public void prepairTrainTestData(String orign_data_dir, String out_dir)
    throws IOException
    {
        MSTimer timer = new MSTimer();

        File dir = new File(orign_data_dir);
        if (!dir.exists() || !dir.isDirectory()) {
            System.out.println(orign_data_dir + " not a directory!!!");
            return;
        }

        LineFileWriter testData = new LineFileWriter(out_dir + "test_data.txt");
        LineFileWriter testRefer = new LineFileWriter(out_dir
                + "test_refer.txt");
        Random r = new Random();
        File[] files = dir.listFiles();

        for (File file : files) {
            LineFileReader reader = new LineFileReader(file.getAbsolutePath());
            String line = null;
            while ((line = reader.nextLine()) != null) {
                if (r.nextInt() % 10 == 1) {
                    String[] tokens = null;
                    if (line.contains("19980")) {
                        line = line.substring(line.indexOf("  ") + 2);
                        tokens = line.split("  ");
                    } else { // 2014年的
                        tokens = line.split(" ");
                    }

                    testRefer.writeLine(line);
                    StringBuilder strBuilder = new StringBuilder();
                    for (String token : tokens) {
                        String[] parts = token.split("/");
                        if (parts[0].startsWith("[")) {
                            parts[0] = parts[0].substring(1);
                        }
                        strBuilder.append(parts[0]);
                    }
                    testData.writeLine(strBuilder.toString());
                }
            }

            reader.close();
        }

        testData.close();
        testRefer.close();

        System.out.println(timer);
    }

    private void extractLexicalContext(final List<SplitWord> words)
    {
        String flag = null;
        for (Iterator<SplitWord> it = words.iterator(); it.hasNext();) {
            SplitWord sw = it.next();
            if (sw.getWord().equals("[")) {
                continue;
            } else if (sw.getWord().equals("]")) {
                flag = sw.getFlag();
                continue;
            }

            if (flag != null) {
                addWordInfo(lexicalInfo, flag, sw.getFlag(), 1);
            }

            flag = sw.getFlag();
        }
    }

    /**
     * 预处理标注的格式错误：标点符号后面没有加上/w标志。
     *
     * @param folder
     * @throws FileNotFoundException
     * @throws UnsupportedEncodingException
     */
    public void preHandleCorpus(String folder, String output)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        String punctions = "，，。、？！‘’；：《》【】“”?,!（）()……";
        List<File> folders = FolderUtils.listSubFolders(folder);
        LineFileWriter errors = new LineFileWriter(FolderUtils.combine(folder,
                "corrected.txt"));
        for (File fold : folders) {
            List<File> files = FolderUtils.listFiles(fold.getAbsolutePath(),
                               true);
            LineFileWriter res = new LineFileWriter(FolderUtils.combine(output,
                                                    fold.getName() + ".txt"));
            for (File f : files) {
                LineFileReader reader = new LineFileReader(f.getAbsolutePath());
                String line = null;
                while ((line = reader.nextLine()) != null) {
                    StringBuilder builder = new StringBuilder();
                    boolean hasErrorMark = false;
                    for (int i = 0; i < line.length(); i++) {
                        char ch = line.charAt(i);
                        builder.append(ch);
                        if (punctions.indexOf(ch) > 0) {
                            if (i == line.length() - 1
                                    || line.charAt(i + 1) != '/') {
                                if (fold.getName().startsWith("19")) {
                                    builder.append("/w  ");
                                } else {
                                    builder.append("/w ");
                                }
                                hasErrorMark = true;
                            }
                        }
                    }
                    res.writeLine(builder.toString());
                    if (hasErrorMark) {
                        errors.writeLine(line + "===>" + builder.toString());
                    }
                }
            }
            res.close();
        }

        errors.close();

    }

    public static void main(String[] args) throws Exception
    {
        MSTimer timer = new MSTimer();
        PeopleDailyDataProcesser peopleDaily = new PeopleDailyDataProcesser();
        // peopleDaily.preHandleCorpus("D:/autoseg/data/corpus/orign",
        // "D:/autoseg/data/corpus/trian");

        //peopleDaily.prepairTrainTestData("D:/autoseg/data/corpus/trian",
        //		"D:/autoseg/data/estimate/");

        peopleDaily.trianCorpusData("D:/autoseg/data/corpus/trian",
                                    "D:/autoseg/data/words/98/");

        System.out.println("done!!used : " + timer);

    }
}
