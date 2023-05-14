cpp_file := main.cpp

program: $(cpp_file)
	g++ -std=c++11 -g $(cpp_file) -o main
	@test -f main && echo "created executable: main"

clean:
	rm main