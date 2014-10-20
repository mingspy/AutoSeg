package com.mingspy.utils;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class FolderUtils
{
    public static String combine(String folder, String path)
    {
        String fol = folder.replace('\\', '/');
        String pa = path.replace('\\', '/');
        String co = fol + "/" + pa;
        return co.replaceAll("//", "/");
    }

    public static List<File> listFiles(String folder, boolean isRecursive)
    {
        List<File> res = new ArrayList<File>();
        File dir = new File(folder);
        if (!dir.exists()) {
            return res;
        }

        if(dir.isDirectory()) {
            File[] files = dir.listFiles();
            if(files != null) {
                for(File f : files) {
                    if(!f.isHidden()) {
                        res.add(f);
                    }

                    if(isRecursive && f.isDirectory()) {
                        res.addAll(listFiles(f.getAbsolutePath(), true));
                    }
                }
            }

        } else {
            res.add(dir);
        }

        return res;
    }

    public static List<File> listSubFolders(String folder)
    {
        List<File> res = new ArrayList<File>();
        File dir = new File(folder);
        if (!dir.exists()) {
            return res;
        }

        if(dir.isDirectory()) {
            File[] files = dir.listFiles();
            if(files != null) {
                for(File f : files) {
                    if(!f.isHidden() && f.isDirectory()) {
                        res.add(f);
                    }
                }
            }
        }

        return res;
    }

    public static boolean deleteFolder(String folder)
    {
        return deleteFolder(new File(folder));
    }

    public static boolean deleteFolder(File folder)
    {
        if(!folder.exists() || !folder.isDirectory()) {
            return false;
        }
        File [] files = folder.listFiles();
        if(files != null) {
            for(File f : files) {
                boolean isDeleted = false;
                if(f.isFile()) {
                    isDeleted = f.delete();
                } else if(f.isDirectory()) {
                    isDeleted = deleteFolder(f);
                }

                if(!isDeleted) {
                    return false;
                }
            }
        }

        return folder.delete();
    }
    public static void main(String[] args)
    {
        System.out.println(listFiles("D:/autoseg/segtool", true));
    }
}
