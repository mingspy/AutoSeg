package com.mingspy.nlp;

import java.util.List;

import com.mingspy.jseg.AutoTokenizer;
import com.mingspy.jseg.ITokenizer;

public class SpliterUtils
{
    public static ITokenizer _tokenizer = new AutoTokenizer();
    public static void setTokenizer(ITokenizer tokenizer)
    {
        _tokenizer = tokenizer;
    }

    public static String stem(String line)
    {
        line = line.toLowerCase();
        // Strip all HTML
        // Looks for any expression that starts with < and ends with > and replace
        // and does not have any < or > in the tag it with a space
        line = line.replaceAll("<[^<>]+>", "");

        // Handle Numbers
        // Look for one or more characters between 0-9
        line = line.replaceAll("[0-9]+", "number");

        // Handle URLS
        // Look for strings starting with http:// or https://

        line = line.replaceAll("(http|https)://[^\\s]*", "httpaddr");
        line = line.replaceAll("www.[^\\s]*", "httpaddr");
        // Handle Email Addresses
        // Look for strings with @ in the middle
        line = line.replaceAll("[^\\s]+@[^\\s]+", "emailaddr");

        // Handle $ sign
        line = line.replaceAll("[$]+", "dollar");
        return line;
    }

    public static List<String> split(String str)
    {
        return _tokenizer.uniGramSplit(str);
    }

    public static void main(String[] args)
    {
        System.out.println(SpliterUtils.split("车不动的时候，会有气门声吗嗒嗒嗒的，有吗"));
    }
}
