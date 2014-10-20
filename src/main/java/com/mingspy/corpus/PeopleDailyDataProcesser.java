package com.mingspy.corpus;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.Set;
import java.util.TreeSet;

import com.mingspy.utils.FolderUtils;
import com.mingspy.utils.MSTimer;
import com.mingspy.utils.io.LineFileReader;
import com.mingspy.utils.io.LineFileWriter;

public class PeopleDailyDataProcesser
{
    private static final String TMP_FOLDER_NAME = "tmpCorupsAnalysis";
    private static final String NATURE_HEAD = "@WordPOS\t";
    private static final String COMPOSE_START = "comps";

    private Set<String> natures = new TreeSet<String>();
    private WordInfoMap coreWords = new WordInfoMap();
    private WordInfoMap bigramWords = new WordInfoMap();
    private WordInfoMap lexicalInfo = new WordInfoMap();
    private WordInfoMap composedInfo = new WordInfoMap();
    private int seqNo = 0;

    private char[] symbols = "（）“”《》‘’；，：、。【】？！~～()\"\"<>'':;.,?!"
                             .toCharArray();
    private Map<String, String> natureTrans = new HashMap<String, String>();

    /**
     * Ag 形语素 a 形容词 ad 副形词 an 名形词 Bg 区别语素 b 区别词 c 连词 Dg 副语素 d 副词 e 叹词 f 方位词 g 语素
     * h 前接成分 i 成语 j 简略语 k 后接成分 l 习用语 Mg 数语素 m 数词 Ng 名语素 n 名词 nr 人名 ns 地名 nt
     * 机构团体 nx 外文字符 nz 其它专名 o 拟声词 p 介词 Qg 量语素 q 量词 Rg 代语素 r 代词 s 处所词 Tg 时间语素 t
     * 时间词 Ug 助语素 u 助词 Vg 动语素 v 动词 vd 副动词 vn 名动词 w 标点符号 x 非语素字 Yg 语气语素 y 语气词 z
     * 状态词
     *
     */
    public PeopleDailyDataProcesser()
    {
        // 由于2014年标注的过细致，与98不兼容，所以添加转换函数
        // ude1,ude2,ude3,udeng,udh,uguo,ule,ulian,uls,usuo,uyy,uzhe,uzhi
        natureTrans.put("ude1", "u");
        natureTrans.put("ude2", "u");
        natureTrans.put("ude3", "u");
        natureTrans.put("udeng", "u");
        natureTrans.put("udh", "u");
        natureTrans.put("uguo", "u");
        natureTrans.put("ule", "u");
        natureTrans.put("ulian", "u");
        natureTrans.put("uls", "u");
        natureTrans.put("usuo", "u");
        natureTrans.put("uyy", "u");
        natureTrans.put("uzhe", "u");
        natureTrans.put("uzhi", "u");

        natureTrans.put("nr1", "nr");
        natureTrans.put("nr2", "nr");

        natureTrans.put("bl", "b");
        natureTrans.put("ag", "Ag");
        natureTrans.put("cc", "c");
        natureTrans.put("dg", "Dg");
        natureTrans.put("dl", "i");
        natureTrans.put("gb", "nz");
        natureTrans.put("gc", "nz");
        natureTrans.put("gb", "nz");
        natureTrans.put("gg", "nz");
        natureTrans.put("gi", "nz");
        natureTrans.put("gm", "nz");
        natureTrans.put("gp", "nz");

        natureTrans.put("mq", "q");

        //nhd,nhm,nis,nit,nmc,nnd,nnt,nr,nr1,nr2,nrf,nrj,ns,nsf,nt,ntc,ntcb,ntcf,ntch,nth,nto,nts,ntu,nx,nz
        natureTrans.put("na", "ns");
        natureTrans.put("ng", "Ng");
        natureTrans.put("nba", "nz");
        natureTrans.put("nbc", "nz");
        natureTrans.put("nhd", "nz");
        natureTrans.put("nhm", "nz");
        natureTrans.put("nis", "nz");
        natureTrans.put("nit", "nz");
        natureTrans.put("nmc", "nz");
        natureTrans.put("nnd", "nz");
        natureTrans.put("nnt", "nz");
        natureTrans.put("ntc", "nt");
        natureTrans.put("ntcb", "nt");
        natureTrans.put("ntcf", "nt");
        natureTrans.put("ntch", "nt");
        natureTrans.put("nth", "nt");
        natureTrans.put("nto", "nt");
        natureTrans.put("ntu", "nt");
        natureTrans.put("nsf", "ns");
        natureTrans.put("nts", "nt");

        natureTrans.put("pba", "p");
        natureTrans.put("pbei", "p");
        natureTrans.put("qt", "q");
        natureTrans.put("qv", "q");

        //r,rr,ry,rys,ryt,ryv,rz,rzs,rzt,rzv,
        natureTrans.put("rr", "r");
        natureTrans.put("ry", "r");
        natureTrans.put("rys", "r");
        natureTrans.put("ryt", "r");
        natureTrans.put("ryv", "r");
        natureTrans.put("rz", "r");
        natureTrans.put("rzs", "r");
        natureTrans.put("rzt", "r");
        natureTrans.put("rzv", "r");

        natureTrans.put("tg", "Tg");
        natureTrans.put("vg", "Vg");

        natureTrans.put("vshi", "v");
        natureTrans.put("vyou", "v");
        natureTrans.put("vx", "v");
        natureTrans.put("vl", "v");
        natureTrans.put("vf", "v");
        natureTrans.put("vi", "v");

        natureTrans.put("al", "a");
    }

    /**
     * 从训练语料中提取词典所需要的信息，包括词，词频，词性，二元词频，标注角色，二元角色词频。
     * 词频统计时，对于组合词不进行组合操作，专门提取一个组合词典，用于组合词的构成语法分析。 统计词的前缀，后缀等信息用于识别组合词(包括人名)
     *
     * @param directory
     *            训练语料目录。
     * @param output
     * @throws Exception
     */
    public void trianCorpusData(String directory, String output)
    throws Exception
    {
        MSTimer timer = new MSTimer();
        String tmpDir = FolderUtils.combine(output, TMP_FOLDER_NAME);
        FolderUtils.deleteFolder(tmpDir);
        // 由于处理大量文件时，内存和数量集几何增长，耗时较长，改用分步处理。
        // 先写到临时文件，然后在合并。
        analysisAndGenTmpResults(directory, tmpDir);
        clearWordsInfos();
        System.out.println("trying to combine results...");
        combineTmpResults(tmpDir, output);
        FolderUtils.deleteFolder(tmpDir);
        System.out.println("total used: " + timer);
    }

    private void combineTmpResults(String tmpDir, String output)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        List<File> files = FolderUtils.listFiles(tmpDir, false);
        for (File f : files) {
            if (f.getName().startsWith("lexi")) {
                readWordsInfo(lexicalInfo, f.getAbsolutePath());
                System.out.println("combined " + f.getName());
            }
        }
        lexicalInfo.prune(1);
        writeWordInfo(lexicalInfo, FolderUtils.combine(output, "/lexical.txt"));
        lexicalInfo.clear();

        for (File f : files) {
            if (f.getName().startsWith("core")) {
                readWordsInfo(coreWords, f.getAbsolutePath());
                System.out.println("combined " + f.getName());
            }
        }
        // prune(coreWords, 1);
        writeWordInfo(coreWords, FolderUtils.combine(output, "/corewords.txt"));
        System.out.println("TotalFreq of core dic: "+coreWords.getTotalFreq());
        coreWords.clear();

        for (File f : files) {
            if (f.getName().startsWith("comp")) {
                readWordsInfo(composedInfo, f.getAbsolutePath());
                System.out.println("combined " + f.getName());
            }
        }
        // prune(coreWords, 1);
        composedInfo.prune(10);
        writeWordInfo(composedInfo,
                      FolderUtils.combine(output, "/composed.txt"));
        coreWords.clear();

        for (File f : files) {
            if (f.getName().startsWith("bi")) {
                readWordsInfo(bigramWords, f.getAbsolutePath());
                System.out.println("combined " + f.getName());
            }
        }
        bigramWords.prune(1);
        writeWordInfo(bigramWords, FolderUtils.combine(output, "/biwords.txt"));
        bigramWords.clear();
    }

    private void analysisAndGenTmpResults(String directory, String tmpDir)
    throws Exception, UnsupportedEncodingException,
        FileNotFoundException
    {
        double totalsize = 0;
        List<File> files = FolderUtils.listFiles(directory, true);
        for (File file : files) {
            try {
                if (file.isFile()) {
                    FileInputStream fis = new FileInputStream(file);
                    double fsize = fis.available() / 1024 / 1024;
                    fis.close();

                    if (totalsize > 0 && (fsize + totalsize) > 30) {
                        outputTmpResult(tmpDir);
                        clearWordsInfos();
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

    private void clearWordsInfos()
    {
        coreWords.clear();
        bigramWords.clear();
        lexicalInfo.clear();
        composedInfo.clear();
    }

    private void outputTmpResult(String tmpdir)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        if (coreWords.size() == 0)
            return;
        File dir = new File(tmpdir);
        dir.mkdir();

        writeWordInfo(
            coreWords,
            FolderUtils.combine(dir.getAbsolutePath(), "/corewords_"
                                + seqNo + ".tmp"));
        writeWordInfo(
            bigramWords,
            FolderUtils.combine(dir.getAbsolutePath(), "/biwords_" + seqNo
                                + ".tmp"));

        writeWordInfo(
            lexicalInfo,
            FolderUtils.combine(dir.getAbsolutePath(), "/lexical_" + seqNo
                                + ".tmp"));
        writeWordInfo(
            composedInfo,
            FolderUtils.combine(dir.getAbsolutePath(), "/composed_" + seqNo
                                + ".tmp"));
        seqNo++;
    }

    private void writeWordInfo(WordInfoMap coreWords, String output)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        // out put
        LineFileWriter writer = new LineFileWriter(output);
        String nature = NATURE_HEAD;
        for (String na : natures) {
            nature += na + ",";
        }
        writer.writeLine(nature);
        coreWords.writeWordInfo(writer);
        writer.close();
    }

    private void readWordsInfo(WordInfoMap coreWords, String input)
    throws UnsupportedEncodingException, FileNotFoundException
    {
        // out put
        LineFileReader reader = new LineFileReader(input);
        String line = reader.nextLine();
        int idx = line.indexOf(NATURE_HEAD);
        line = line.substring(idx + NATURE_HEAD.length());
        String[] nts = line.split(",");
        for (String nt : nts) {
            natures.add(nt);
        }
        nts = null;

        coreWords.readWordInfo(reader);
        reader.close();
        reader = null;
        System.gc();
    }

    private void processAFile(File file) throws Exception
    {
        try {
            LineFileReader reader = new LineFileReader(file, "utf-8");
            String line = null;
            while ((line = reader.nextLine()) != null) {
                List<SplitWord> splited_words = changeToSplitedWord(line.trim());
                if (splited_words == null) {
                    continue;
                }

                // 提取核心词典信息
                extractCoreWords(splited_words);

                // 提取二元词典信息
                extractBigramWords(splited_words);

                // 提取词性信息
                extractLexicalContext(splited_words);

                // 提取组合词信息
                extractComposedLexicalPatterns(splited_words);
                // 提取人名词典
                // extractPersonNameWords(words);
                // 提取组织机构词典
                // extractOrganizationWords(words);
                // 提取译名词典
            }

        } catch (IOException ee) {
            ee.printStackTrace();
        }
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
                String markedWord = marked_words[i].trim();
                if (markedWord.isEmpty()) {
                    continue;
                }
                int idx = markedWord.lastIndexOf('/');
                int idx_f = markedWord.indexOf('/');
                if (idx < 0) {
                    continue;
                }

                String word = null;
                String nature = null;

                boolean isComposeEnd = false;
                String composeNature = null;
                // 处理组合词的开头和结尾
                // handle [中央/n 人民/n 广播/vn 电台/n]nt
                // handle [中央/n 人民/n 广播/vn 电台/n]/nt
                // "[中央/n"匹配这种形式的，为组合词开头
                if (markedWord.matches("\\[.+/([\\w])+")) {
                    splited_words.add(new SplitWord("[", COMPOSE_START));
                    word = markedWord.substring(1, idx);
                    nature = markedWord.substring(idx + 1);
                } else if (markedWord.matches(".+/[\\w]+\\]/?[\\w]+")) {
                    // 组合词结尾
                    word = markedWord.substring(0, idx_f);
                    nature = markedWord.substring(idx_f + 1);
                    isComposeEnd = true;
                    int compose_end_indx = nature.indexOf(']');
                    composeNature = nature.substring(compose_end_indx + 1);
                    nature = nature.substring(0, compose_end_indx);
                    if (composeNature.charAt(0) == '/') {
                        composeNature = composeNature.substring(1);
                    }
                } else if (markedWord.matches("\\[.+]/?[\\w]+")) {
                    int idx_s = markedWord.indexOf('[');
                    int idx_e = markedWord.indexOf(']');

                    word = markedWord.substring(idx_s + 1, idx_e);
                    int idx_m = word.indexOf('/');
                    if (idx_m >= 0) {
                        nature = word.substring(idx_m + 1, idx_e);
                        word = word.substring(0, idx_m);
                    } else {
                        nature = word.substring(idx_e + 1);
                    }
                } else {
                    nature = markedWord.substring(idx + 1);
                    if (nature.equals("%") && idx_f < idx) {
                        nature = markedWord.substring(idx_f + 1, idx);
                        word = markedWord.substring(0, idx_f);
                    } else {
                        word = markedWord.substring(0, idx);
                    }
                }

                if (word.isEmpty())
                    continue;

                // 处理人工标记错误,去掉词尾的一些标点符号
                if (!nature.equals("w"))
                    word = fixManualErrors(word);
                if (!word.isEmpty())
                    splited_words.add(new SplitWord(word,
                                                    transferNature(nature)));
                if (isComposeEnd) {
                    splited_words.add(new SplitWord("]", transferNature(composeNature)));
                }
            }
        } catch (Exception e) {
            System.out.println("error with:" + line);
            throw e;
        }

        marked_words = null;
        return splited_words.isEmpty() ? null : splited_words;
    }

    private String transferNature(String nature)
    {
        String trans = natureTrans.get(nature);
        if (trans != null) {
            return trans;
        }
        return nature;
    }

    private String fixManualErrors(String word)
    {
        for (char ch : symbols) {
            if (word.charAt(0) == ch) {
                word = word.substring(1);
                break;
            }

            if (word.charAt(word.length() - 1) == ch) {
                word = word.substring(0, word.length() - 1);
                break;
            }
        }
        return word;
    }

    /**
     * 提取核心词典信息 先提取出非特殊词 加入词库由于人名的姓和名是分开的，所以对于人名进行合并，加入词库，并增加PER的出现次数。
     * 对于地名和组织机构名，不合并，加入核心词典, 然后增加对应ORG和LOC的频率。 时间和数词不加入核心词典，只增加NUM,TIME的频率。
     *
     * @param splited_words
     */
    private void extractCoreWords(final List<SplitWord> words)
    {
        for (int i = 0; i < words.size(); i++) {
            SplitWord sw = words.get(i);
            String word = sw.getWord();
            String nature = sw.getFlag();
            natures.add(nature);
            if (word.equals("[") || word.equals("]") || nature.equals("w")) {
                continue;
            }

            coreWords.addWordInfo(word, nature, 1);
            if (nature.equals("nr") && i < words.size() - 1) {
                SplitWord next = words.get(i + 1);
                if (next.getFlag().equals("nr")
                        && (next.getWord().length() + word.length()) <= 3) {
                    coreWords.addWordInfo(word + next.getWord(), nature, 1);
                }
            }
        }

    }

    /**
     * 提取二元共献词典 把属于特殊词类的所有词都替换成特殊词的标记，进行统计。 其他按正常词类计算。标点不统计，作为切分点。
     * 停用词也不统计，作为切分点。
     *
     * @param words
     * @throws Exception
     */
    private void extractBigramWords(final List<SplitWord> words)
    {
        for (int i = 0; i < words.size() - 1;) {
            SplitWord cur = words.get(i);
            if (cur.getFlag().equals("w") || cur.getWord().equals("[")) {
                i++;
                continue;
            }

            i++;
            SplitWord next = null;
            while (i < words.size() && next == null) {
                next = words.get(i);
                if (next.getFlag().equals("w") || next.getWord().equals("[")
                        || next.getWord().equals("]")) {
                    i++;
                    next = null;
                    continue;
                }
                break;
            }

            if (next != null) {
                bigramWords.addWordInfo(cur.getWord(), next.getWord(), 1);
            }

        }
    }

    private void extractLexicalContext(final List<SplitWord> words)
    {
        String flag = null;
        for (Iterator<SplitWord> it = words.iterator(); it.hasNext();) {
            SplitWord sw = it.next();
            if (sw.getWord().equals("[") || sw.getWord().equals("]")) {
                continue;
            }

            if (flag != null) {
                lexicalInfo.addWordInfo(flag, sw.getFlag(), 1);
            }

            flag = sw.getFlag();
        }
    }

    private void extractComposedLexicalPatterns(List<SplitWord> words)
    {
        for (int i = 0; i < words.size() - 1; i++) {
            SplitWord sw = words.get(i);
            if (sw.getFlag().equals(COMPOSE_START)) {
                int j = i + 1;
                StringBuilder sb = new StringBuilder();
                for (; j < words.size(); j++) {
                    sw = words.get(j);
                    if (sw.getWord().equals("]")) {
                        composedInfo
                        .addWordInfo(sb.toString(),sw.getFlag(),1);
                        break;
                    }
                    if (sb.length() > 0) {
                        sb.append("@");
                    }
                    sb.append(sw.getFlag());
                }
                i = j;
            }
        }
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

    public static void statisticTotalFreq()
    {
        LineFileReader reader = new LineFileReader("d:/autoseg/data/words/corewords.txt");
        reader.nextLine();
        WordInfoMap map = new WordInfoMap();
        map.readWordInfo(reader);
        System.out.println(map.getTotalFreq());
    }
    public static void main(String[] args) throws Exception
    {

        MSTimer timer = new MSTimer();
        PeopleDailyDataProcesser peopleDaily = new PeopleDailyDataProcesser();
        /*
         * peopleDaily.preHandleCorpus("D:/autoseg/data/corpus/orign",
         * "D:/autoseg/data/corpus/trian");
         */

        // peopleDaily.prepairTrainTestData("D:/autoseg/data/corpus/trian",
        // "D:/autoseg/data/estimate/");

        peopleDaily.trianCorpusData("D:/autoseg/data/corpus/trian",
                                    "D:/autoseg/data/words/");

        //statisticTotalFreq();

        System.out.println("done!!used : " + timer);

    }
}
