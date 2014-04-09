INCLUDE=-I/opt/apps_install/jdk-1.6.0_25/include -I/opt/apps_install/jdk-1.6.0_25/include/linux
#LIBDIR=-L/opt/sogou_seg_with_tagger/lib64
#LIB=-lencoding -lssplatform -lIQSegmentor -lCoreSegmentor -lunicode-encoding -lsgtagger -lbasic_util -lboost_iostreams
#CC=g++ -O3 $(INCLUDE) $(LIB) $(LIBDIR)
CC=g++ -O3 $(INCLUDE)

segtool:SegToolsu.cpp libJSegJNI.so
	$(CC) SegToolsu.cpp -o segtool

libJSegJNI.so:JSegJNI.cpp
	$(CC) -fPIC -shared JSegJNI.cpp -o libJSegJNI.so 

clean:
	rm segtool libJSegJNI.so

