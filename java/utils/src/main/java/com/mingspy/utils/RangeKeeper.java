package com.mingspy.utils;

import java.util.ArrayList;
import java.util.List;

public class RangeKeeper
{
    private List<Range> ranges = new ArrayList<Range>();
    public RangeKeeper()
    {

    }

    public void add(Range range)
    {
        int start = range.getStart();
        int end = range.getEnd();
        int i = 0;
        int first = -1;
        int last = -1;
        Range r = null;
        // [x---------y] 找到start所在的区间，如图所示
        // .....^
        // .....|
        // ...start
        for(; i < ranges.size()&&(r = ranges.get(i)).getStart() <= start; i++) {
            if(r.getEnd() >= start) {
                first = i;
                break;
            }

        }

        if(first == -1 && i < ranges.size()) {
            r = ranges.get(i);
            if(r.getStart() <= end) {
                r.setStart(start);
                first = i;
            }
        }

        // 存在与[start,end]交错的区间，那么尝试合并这些区间
        if(first >= 0) {
            // 找到结束位置
            for(; i < ranges.size()&& (r = ranges.get(i)).getStart() <= end;) {
                if(r.getEnd() >= end) {
                    last = i;
                    break;
                } else {
                    if(i != first) {
                        ranges.get(first).setEnd(r.getEnd());
                        ranges.remove(i);
                        continue;
                    }
                }

                i++;
            }
        }

        if(first != -1 && last != -1) {
            if(first != last) {
                ranges.get(first).setEnd(ranges.get(last).getEnd());
                ranges.remove(last);
            }
        } else if(first == -1 && last == -1) {
            ranges.add(i,range);
        } else if(first != -1) {
            ranges.get(first).setEnd(end);
        }
    }

    public void remove(Range range)
    {
        int start = range.getStart();
        int end = range.getEnd();
        int first = -1;
        int i = 0;
        Range r = null;
        for(; i < ranges.size()&& ranges.get(i).getEnd() <= start; i++) {
        }

        if(i < ranges.size()) {
            first = i;
            i++;
        }

        // 删掉位于[start,end]内部的节点
        for(; i < ranges.size()&& ranges.get(i).getEnd() <= end;) {
            ranges.remove(i);
        }

        // 最后一个特殊处理
        if(i < ranges.size()) {
            r = ranges.get(i);
            if(r.getStart() <= end) {
                if(r.getEnd() == end + 1) {
                    ranges.remove(i);
                } else {
                    r.setStart(end + 1);
                }
            }
        }

        // 对第一个特殊处理
        if(first >= 0) {
            r = ranges.get(first);
            int rs = r.getStart();
            int re = r.getEnd();
            if(rs >= start) {
                if(re <= end) {
                    ranges.remove(first);
                } else if(rs <= end) {
                    if(r.getEnd() == end + 1) {
                        ranges.remove(i);
                    } else {
                        r.setStart(end + 1);
                    }
                }
            } else {
                r.setEnd(start - 1);
                if(end < re) {
                    Range nr = new Range(end + 1, re);
                    ranges.add(first+1, nr);
                }
            }
        }


    }

    public boolean exists(Range range)
    {
        return false;
    }

    public List<Range> getRanges()
    {
        return ranges;
    }

    public int size()
    {
        return ranges.size();
    }

    @Override
    public String toString()
    {
        StringBuilder sb = new StringBuilder("range:");
        for(Range r:ranges) {
            sb.append(" (");
            sb.append(r.getStart());
            sb.append(",");
            sb.append(r.getEnd());
            sb.append(")");
        }
        return sb.toString();
    }


    public static void main(String args[])
    {
        RangeKeeper rk = new RangeKeeper();
        rk.add(new Range(1, 30));
        rk.add(new Range(70,80));
        rk.add(new Range(40,50));
        rk.add(new Range(10,40));
        rk.add(new Range(10,40));
        rk.add(new Range(10,20));
        rk.add(new Range(-10,-1));
        rk.add(new Range(-1, -1));
        rk.add(new Range(1,100));
        System.out.println(rk);

        RangeKeeper rr = new RangeKeeper();
        rr.add(new Range(1, 200));
        System.out.println(rr);
        rr.remove(new Range(101, 200));
        System.out.println(rr);
        rr.remove(new Range(50, 50));
        System.out.println(rr);
        rr.remove(new Range(10, 20));
        System.out.println(rr);
        rr.remove(new Range(30, 60));
        System.out.println(rr);
        rr.remove(new Range(5, 600));
        System.out.println(rr);
    }

}
