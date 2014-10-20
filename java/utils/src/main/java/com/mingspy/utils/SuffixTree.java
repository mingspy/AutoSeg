package com.mingspy.utils;

import java.util.ArrayList;
import java.util.List;

/**
 * 后缀树的实现<br>
 * 参考论文《On-line construction of suffix trees》by E.Ukkonen<br>
 *
 * 2013-9-25:添加节点归属实现，用于找出多个字符串的公共串<br>
 * 规则如下：<br>
 * 1.对于已存在的节点，且当前添加的后缀为$,并且当前节点不属于正在添加的字符串，<br>
 * belongs++, belongTo=当前字符串.<br>
 * 2.对于切分的节点，如果被切分的节点不属于当前字符串，把新申请的节点belongs＋＋，<br>
 * 同时设置其belongTo＝当前字符串
 *
 * @author xiuleili
 *
 * @param <T>
 */
public class SuffixTree
{
    public static final boolean isPrintDebugInfo = false;
    /**
     * suffix tree 的节点
     *
     * @author xiuleili
     *
     */
    public class Node
    {
        Node(StringBuilder source, int start, int len, int belongTo)
        {
            this.source = source;
            this.start = start;
            this.len = len;
            this.parent = this;
            this.belongs = 1;
            this.belongTo = belongTo;
        }

        private StringBuilder source;
        private int pos;
        private int len;
        private int start;
        private int depth;

        private int belongs;
        private int belongTo;
        private List<Node> childs;
        private Node parent;
        private Node suffix;

        // public int leafnum;
        // public Object additionalData;

        public void removeChild(Node node)
        {
            childs.remove(node);
        }

        public void addChild(Node node)
        {
            if (childs == null) {
                childs = new ArrayList<Node>();
            }
            childs.add(node);
        }

        public boolean isleaf()
        {
            return this.parent != this
                   && (this.childs == null || this.childs.isEmpty());
        }

        public Node matchFirst(char ch)
        {
            if (childs != null) {
                for (Node child : childs) {
                    if (child.source.charAt(child.start) == ch) {
                        return child;
                    }
                }
            }

            return null;
        }

        public char charAt(int index)
        {
            return source.charAt(start + index);
        }

        public void setSource(StringBuilder source, int start, int len)
        {
            this.source = source;
            this.start = start;
            this.len = len;
        }

        @Override
        public String toString()
        {
            if (this.parent == this) {
                return "Root";
            } else {
                if (len == -1) {
                    return source.substring(start);
                } else {
                    return source.substring(start, start + len);
                }
            }
        }

        public int length()
        {
            if (len != -1) {
                return len;
            }

            return source.length() - start;
        }

    }

    private Node root = new Node(new StringBuilder("Root"), 0, 0,
                                 -1);
    private List<StringBuilder> sbs = new ArrayList<StringBuilder>();
    private Node currentNode;
    private Node lastInnerNode;
    private List<Node> leaves;
    private int phase;
    private List<Node> allleaves = new ArrayList<Node>();
    private int belongTo;
    private List<Node> commonNodes = new ArrayList<Node>();

    public SuffixTree()
    {
        root.suffix = root;
    }

    public SuffixTree(String str)
    {
        root.suffix = root;
        addString(str);
    }

    public void addString(String str)
    {
        if(isPrintDebugInfo) System.out.println("add string to suffix tree:" + str);
        addStr(new StringBuilder(str));
    }

    private void addStr(StringBuilder strbuilder)
    {
        strbuilder.append("$");
        sbs.add(strbuilder);
        currentNode = root;
        leaves = new ArrayList<Node>();
        int len = strbuilder.length();
        belongTo = sbs.size();

        // 把字符串从前向后，逐个加入到
        for (phase = 0; phase < len; phase++) {
            boolean undone = true;
            lastInnerNode = null;
            for (int j = leaves.size(); j <= phase && undone; j++) {
                undone = extend(strbuilder, j, phase);
            }
        }

        putEnd();
    }

    /**
     * 获得最后添加的一个字符串与前面所有字符串的共有字符串区间。
     * @return
     */
    public RangeKeeper getLastCommonRanges()
    {
        RangeKeeper keeper = new RangeKeeper();
        String str = sbs.get(belongTo - 1).toString();

        for(Node node : commonNodes) {
            if(node.belongTo == belongTo && node.belongs > 1&& node.parent != node) {
                findAllRange(keeper, str, node);
            }
        }

        return keeper;
    }

    private void findAllRange(RangeKeeper keeper, String source, Node node)
    {
        if(node.parent == node) {
            return;
        }
        String s = node.toString();
        if(s.endsWith("$")) {
            s = s.substring(0, s.length() - 1);
        }
        if(!s.isEmpty()) {
            int k = 0;
            while((k = source.indexOf(s, k)) != -1) {
                keeper.add(new Range(k, k + s.length() - 1));
                k += s.length();
            }
        }
        findAllRange(keeper, source, node.parent);
    }
    /**
     * 扩展字符串 str[start...len], 并把 str[len] 加入到后缀树上.
     *
     * @param source
     * @param start
     * @param len
     * @return 返回true表示扩展成功
     */
    private boolean extend(StringBuilder sb, int start, int end)
    {

        char charToAdd = sb.charAt(end);
        /* for debugging */
        if(isPrintDebugInfo) {
            System.out.println("{{{in phase " + phase + " extending\""
                               + sb.substring(start, end + 1) + "\" start=" + start + " end="
                               + end + "\n" + "  >>adding\'" + charToAdd
                               + "\' to current node-> " + currentNode);
        }
        // 找到当前插入节点.
        Node node = jumpDown(currentNode, sb, start, end, true);
        // 根据节点的类型进行扩展操作.
        if (node.pos != 0) { // 找到插入的位置
            // 新添加字符在节点内部
            if (node.charAt(node.pos) != charToAdd) {
                Node newleaf = spliteNode(node, sb, end);
                node = newleaf.parent;
                if (lastInnerNode != null) {

                    lastInnerNode.suffix = node;
                    if(isPrintDebugInfo) {
                        System.out.println("  ~linking suffix from \""+ lastInnerNode + "\" to \"" + node + "\"");
                    }
                }
                lastInnerNode = node;
                currentNode = node;
                leaves.add(newleaf);
                if(isPrintDebugInfo) {
                    System.out.println("  +add new leaf ->" + newleaf + " to->" + node);
                    markPrint(newleaf, "+");
                }
            } else {

                if(isPrintDebugInfo) {
                    System.out.println("  -suffix already exists->" + charToAdd);
                    System.out.println("done}}}");
                }
                node.pos = 0;
                if (charToAdd == '$' && node.belongTo != belongTo) {
                    node.belongTo = belongTo;
                    node.belongs++;
                    commonNodes.add(node);
                }
                return false;
            }
        } else {
            if (node.belongTo != belongTo && node.parent != node) {
                node.belongTo = belongTo;
                node.belongs++;
                commonNodes.add(node);
            }

            if (lastInnerNode != null) {
                lastInnerNode.suffix = node;
                if(isPrintDebugInfo) {
                    System.out.println("  ~linking suffix from \"" + lastInnerNode
                                       + "\" to \"" + node + "\"");
                }
            }
            lastInnerNode = node;
            if (node.matchFirst(charToAdd) != null) {
                if(isPrintDebugInfo) {
                    System.out.println("  -suffix already exists![" + charToAdd+"]");
                    System.out.println("done}}}");
                }
                currentNode = node;
                return false;
            } else if (node.isleaf()) {
                // 扩展已有的leaf节点, 当添加另外一个新的字符串的时候发生。
                if(isPrintDebugInfo) {
                    System.out.println("  +extend old leaf \"" + node + "\""
                                       + sb.substring(start));
                }
                node.setSource(sb, start, -1);
                // node.belongs++;
                currentNode = node.parent;
                leaves.add(node);
            } else {
                // 添加一个新leaf节点
                Node leaf = new Node(sb, end, -1, belongTo);
                leaf.suffix = root;
                leaf.parent = node;
                leaf.depth = node.depth + node.length();
                node.addChild(leaf);
                currentNode = node;
                leaves.add(leaf);

                // for debug
                if(isPrintDebugInfo) {
                    System.out.println("  +add new leaf ->" + leaf + " to " + node);
                    markPrint(leaf, "+");
                }
            }
        }

        if(isPrintDebugInfo) {
            System.out.println("  ->shift suffix from \"" + currentNode
                               + "\" to \"" + currentNode.suffix + "\"");
            System.out.println("done}}}");
        }
        currentNode = currentNode.suffix;
        return true;
    }

    private Node jumpDown(Node node, StringBuilder sb, int start,
                          int end, boolean fromFront)
    {
        if(isPrintDebugInfo) {
            System.out.println("    jumpping down from->[" + node + "] with \""
                               + sb.substring(start, end + 1) + "\"");
        }

        int i = 0;
        int jumpped = node.depth;
        if (fromFront) {
            if (node != root && node.charAt(0) != sb.charAt(start+jumpped)) {
                markPrint(node,"->");
                throw new RuntimeException("can't find a path->"
                                           + sb.charAt(start));
            }

            i = node.depth+node.length();
        }

        int jump = end - start;

        for (; i < jump;) {
            Node child = node.matchFirst(sb.charAt(start + i));
            if (child == null) {
                throw new RuntimeException("can't find a path->"
                                           + sb.charAt(start));
            } else {

                jumpped += node.length();
                i += child.length();
                node = child;
                if(isPrintDebugInfo) {
                    System.out.println("    jumpe to ->" + node + ", has jumpped="
                                       + jumpped);
                }
            }
        }

        if (i != jump) {
            node.pos = (jump - jumpped);
        }
        return node;
    }

    /**
     * Split one node at pos.<br>
     *
     *  \  <br>
     *   #      &lt- pos - 1  <br>
     *    \ <br>
     *    node <br>
     *
     * 从pos前一位置把原node分开为两部分,<br>
     * 一部分为 internal node, 一部分为new node,<br>
     * 并添加一个新叶节点到internal node. <br>
     * ----------------- <br>
     *
     *    \     <br>
     *   internal node <br>
     *     /         \ <br>
     *    /         leaf<br>
     *    node    <br>
     * @param node
     * @param ch
     * @return
     */
    private Node spliteNode(Node node, StringBuilder sb, int pos)
    {

        Node newNode = new Node(node.source, node.start, node.pos,
                                belongTo);
        Node leaf = new Node(sb, pos, -1, belongTo);
        node.start += node.pos;
        node.len = node.len == -1 ? -1 : node.len - node.pos;
        node.pos = 0;
        newNode.suffix = root;
        newNode.depth = node.depth;
        node.depth += newNode.length();
        leaf.depth = node.depth;
        leaf.suffix = root;

        newNode.parent = node.parent;
        node.parent.removeChild(node);
        newNode.parent.addChild(newNode);
        node.parent = newNode;
        leaf.parent = newNode;
        newNode.addChild(node);
        newNode.addChild(leaf);

        if (node.belongTo != belongTo) {
            newNode.belongTo = belongTo;
            newNode.belongs++;
            commonNodes.add(newNode);
        }
        return leaf;
    }

    private void putEnd()
    {
        allleaves.addAll(leaves);
        int len = sbs.get(sbs.size() - 1).length();
        for (Node leaf : leaves) {
            leaf.len = len - leaf.start;
        }
    }

    public void print()
    {
        print(root, 0, 4, null, "");
    }


    public void markPrint(Node node, String mark)
    {
        print(root, 0, 4, node, mark);
    }

    public static void print(Node node, int margin, int indent, Node markNode, String mark)
    {
        int sub = 0;
        if(node == markNode) {
            System.out.print(mark);
            sub = mark.length();
        }

        for (int i = 0; i < margin - sub; i++) {
            System.out.print(" ");
        }

        System.out.print("["+node + "]:belongs " + node.belongs + " belongTo "
                         + node.belongTo);

        if (node.childs != null) { // 非叶子节点
            System.out.println("{{{");
            for (Node child : node.childs) {
                print(child, margin + indent, indent, markNode, mark);
            }
            for (int i = 0; i < margin; i++) {
                System.out.print(" ");
            }
            System.out.print("}}}");
        }
        System.out.println();
    }

    public static void main(String[] args)
    {
        SuffixTree st = new SuffixTree("我到广州买二手车怎样办理汽车过户手续");
        st.print();
        st.addString("成都二手车代办过户能把二手车的车籍是怎么办理的？");
        st.print();
    }
}
