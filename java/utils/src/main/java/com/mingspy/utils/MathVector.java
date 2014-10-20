package com.mingspy.utils;



public class MathVector
{

    /**
     * 累加和
     * @param x 向量x
     * @return
     */
    public static double cumulativeSum(double[] x)
    {
        double sum = 0;
        for(int i = 0; i < x.length; i++) {
            sum += x[i];
        }
        return sum;
    }

    /**
     * 点积 (内积，交叉乘积)
     * @param x 向量x
     * @param y 向量y
     * @return
     */
    public static double dotProduct(double[] x, double [] y)
    {
        double sum = 0;
        for(int i = 0; i < x.length; i++) {
            sum += x[i]*y[i];
        }
        return sum;
    }

    /**
     * 平方和
     * @param x 向量x
     * @return
     */
    public static double sumOfSquare(double[] x)
    {
        double sum = 0;
        for(int i = 0; i < x.length; i++) {
            sum += Math.pow(x[i], 2.0);
        }
        return sum;
    }

    /**
     * 平方和
     * @param x 向量x
     * @return
     */
    public static double sumOfSquare(Double[] x)
    {
        double sum = 0;
        for(int i = 0; i < x.length; i++) {
            sum += Math.pow(x[i], 2.0);
        }
        return sum;
    }
}
