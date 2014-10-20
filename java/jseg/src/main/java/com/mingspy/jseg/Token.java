package com.mingspy.jseg;

public class Token
{
    public int off;
    public int len;
    public String nature;
    public String word;
    //public int attr;
    public Token(int off, int len)
    {
        this.off = off;
        this.len = len;
    }
    public Token(int off, int len, String nature)
    {
        this.off = off;
        this.len = len;
        this.nature = nature;
    }
    @Override
    public String toString()
    {
        StringBuilder builder = new StringBuilder();
        builder.append("(");
        builder.append(off);
        builder.append(",");
        builder.append(len);
        if(word != null) {
            builder.append("=>");
            builder.append(word);
        }
        if(nature != null) {
            builder.append("/");
            builder.append(nature);
        }
        builder.append(")");
        return builder.toString();
    }

}
