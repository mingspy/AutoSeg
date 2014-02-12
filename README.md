AutoSeg - auto Chinese segmentor
=======

#### A fast Chinese segmentor in c++.
Auto Chinese segmentor is developed by mingspy for auto.sohu.com, otherwise #auto# also abbreviation of automation, it can intelligently deal with ambiguous words.

#### Max split estimate result:
CPU: Intel(R) Core(TM) i5-3470 cpu @ 3.20GHZ 3.20GHZ.
RAM:  4.00GB.
os:  Windows 7 Enterprise sp1, 64 bits.
processes runing on the windows:87.
date:2014-2-11.

load dict elapsed:296 ms.
load test data elapsed:47 ms.
load test refer data elapsed:174 ms.
segment 1842385 bytes data, 318169 words.

###### -----------elapsed---speed----recognized-----corrected------precision----recall-------F2
###### foreward--107 ms---16.4m/s----311932 words---241890 words---0.775457----0.760256----0.767782
###### inverse---111 ms---15.8m/s----312716 words---247683 words---0.792038----0.778464----0.785192
