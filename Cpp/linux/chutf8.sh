#!/bin/sh
iconv -f gbk -t utf-8 SegTools.cpp > SegToolsu.cpp
iconv -f gbk -t utf-8 PunctionDictionary.hpp > PunctionDictionaryu.hpp
iconv -f gbk -t utf-8 Test.hpp > testu.hpp
#rm SegTools.cpp PunctionDictionary.hpp Test.hpp