reverseOrder := reverseOrder.cpp
increasingEvenOdd := increasingEvenOdd.cpp
decreasingOddEven := decreasingOddEven.cpp
oddDecrEvenIncr := oddDecrEvenIncr.cpp
evenDecrOddIncr := evenDecrOddIncr.cpp
evenDecrOddDecr := evenDecrOddDecr.cpp
evenIncrOddDecr := evenIncrOddDecr.cpp

outputs := reverseOrder increasingEvenOdd decreasingOddEven oddDecrEvenIncr evenDecrOddIncr evenDecrOddDecr

all: reverseOrder increasingEvenOdd decreasingOddEven oddDecrEvenIncr evenDecrOddIncr evenDecrOddDecr

reverseOrder: $(reverseOrder)
	g++ -std=c++11 -g $(reverseOrder) -o reverseOrder

increasingEvenOdd: $(increasingEvenOdd)
	g++ -std=c++11 -g $(increasingEvenOdd) -o increasingEvenOdd

decreasingOddEven: $(decreasingOddEven)
	g++ -std=c++11 -g $(decreasingOddEven) -o decreasingOddEven

oddDecrEvenIncr: $(oddDecrEvenIncr)
	g++ -std=c++11 -g $(oddDecrEvenIncr) -o oddDecrEvenIncr

evenDecrOddIncr: $(evenDecrOddIncr)
	g++ -std=c++11 -g $(evenDecrOddIncr) -o evenDecrOddIncr

evenDecrOddDecr: $(evenDecrOddDecr)
	g++ -std=c++11 -g $(evenDecrOddDecr) -o evenDecrOddDecr

evenIncrOddDecr: $(evenIncrOddDecr)
	g++ -std=c++11 -g $(evenIncrOddDecr) -o evenIncrOddDecr

clean:
	rm $(outputs)