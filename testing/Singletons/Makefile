singleton_file := singletonEx1.cpp
threadInOrder_file := MutexSingleton.cpp
threadInOrder2_file := MutexSingleton2.cpp
threadTestFile := threadTesting.cpp

singleton: $(singleton_file)
	g++ -std=c++11 -g $(singleton_file) -o singletonEx1

threadInOrder: $(threadInOrder_file)
	g++ -std=c++11 -g $(threadInOrder_file) -o threadInOrder -lpthread

threadInOrder2: $(threadInOrder2_file)
	g++ -std=c++11 -g $(threadInOrder2_file) -o threadInOrder2 -lpthread

threadTesting: $(threadTestFile)
	g++ -std=c++11 -g $(threadTestFile) -o threadTesting -lpthread