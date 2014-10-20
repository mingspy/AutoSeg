package com.mingspy.utils;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.GregorianCalendar;

public final class DateUtils
{
    private DateUtils()
    {
    }

    private static final long DAY_OF_MILLIS = 1000 * 60 * 60 * 24;
    private static final String SHORT_DAY_FORMAT = "yyyyMMdd";
    private static final SimpleDateFormat SHORT_TIME_FORMAT = new SimpleDateFormat(SHORT_DAY_FORMAT);

    private static final String LONG_DAY_FORMAT = "yyyyMMddHHmmss";
    private static final SimpleDateFormat LONG_TIME_FORMAT = new SimpleDateFormat(LONG_DAY_FORMAT);

    public static final Date convertStr2Date(final String dateString)
    {
        if (dateString == null || dateString.isEmpty()) {
            return null;
        }
        Date dete = null;
        try {
            dete = SHORT_TIME_FORMAT.parse(dateString);
        } catch (ParseException e) {
        }
        return dete;
    }

    public static final String convertDate2Str(final Date date)
    {
        if (date == null) {
            return "";
        }
        return SHORT_TIME_FORMAT.format(date);
    }

    public static final String convertDate2StrLong(final Date date)
    {
        if (date == null) {
            return "";
        }
        return LONG_TIME_FORMAT.format(date);
    }


    public static final int[] getIntDateOfLateDate(final int days)
    {
        int[] ret = new int[days];
        for (int i = 0; i < days; i++) {
            ret[i] = getYearMonthDay(getLastDate(i));
        }
        return ret;
    }

    public static final int getYearMonthDay(final Date date)
    {
        String dateStr = SHORT_TIME_FORMAT.format(date);
        int ret = Integer.valueOf(dateStr);
        return ret;
    }

    public static final long getZeroTime(final Date date)
    {
        GregorianCalendar c = new GregorianCalendar();
        c.setTime(date);
        c.set(GregorianCalendar.HOUR_OF_DAY, 0);
        c.set(GregorianCalendar.MINUTE, 0);
        c.set(GregorianCalendar.SECOND, 0);
        c.set(GregorianCalendar.MILLISECOND, 0);

        return c.getTimeInMillis();
    }

    public static final long getLastTime(final long startTime, final long time)
    {
        if (startTime > time) {
            return startTime - time;
        }
        return startTime;
    }

    public static final Date getLastDate(final int days)
    {
        long time = System.currentTimeMillis() - DAY_OF_MILLIS * days;
        return new Date(time);
    }

}
