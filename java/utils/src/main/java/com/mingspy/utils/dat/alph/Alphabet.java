package com.mingspy.utils.dat.alph;

import java.io.Serializable;

public abstract class Alphabet implements Serializable
{
    private static final long serialVersionUID = -9145264558374763491L;

    /**
     * Size of all character that may appear after the char.<br>
     * For example: if in our alphabet, there are only 4 words:<br>
     * abandon,are,air,abort<br>
     * then, subAlphLength(a) = 4, as b,n,r,i appear after a.<br>
     * subAlphLength(z) = 0, as z not appear in our alphabet.<br>
     * subAlphLength(b) = 2, as a,o appear after b.<br>
     * subAlphLength(r) = 1, as t appear after r. <br>
     *
     * @param ch
     * @return
     */
    public abstract int childSize(int ch);

    /**
     * get the hashed inner code of a char.
     *
     * @param ch
     * @return
     */
    public abstract int getInnerCode(int ch);

    /**
     * translate a string to inner codes, where the last one must be zero.<br>
     * int [] codes = getInnerCodes("xxxx");<br>
     * assert(codes[codes.length -1] == 0);<br>
     *
     * @param str
     * @return
     */
    public final int[] getInnerCodes(String str)
    {
        int[] result = new int[str.length() + 1];
        for (int i = 0; i < str.length(); i++) {
            result[i] = getInnerCode(str.charAt(i));
        }
        result[result.length - 1] = 0;
        return result;
    }

}
