sampleobjects = buffer_manager.o file_manager.o bulkLoading.o

sample_run : $(sampleobjects)
	     g++ -std=c++11 -o rtree $(sampleobjects)

rtree.o : bulkLoading.cpp
	g++ -std=c++11 -c bulkLoading.cpp

buffer_manager.o : buffer_manager.cpp
	g++ -std=c++11 -c buffer_manager.cpp

file_manager.o : file_manager.cpp
	g++ -std=c++11 -c file_manager.cpp

clean :
	rm -f *.o
	rm -f sample_run
