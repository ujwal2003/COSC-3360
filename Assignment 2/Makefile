server_files := server.cpp huffmanTree.h huffmanTree.cpp
client_files := client.cpp

server: $(server_files)
	g++ -std=c++11 -g $(server_files) -o server -lpthread

client: $(client_files)
	g++ -std=c++11 -g $(client_files) -o client -lpthread

clean:
	rm server client