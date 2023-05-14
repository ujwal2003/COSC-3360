#include <iostream>
#include <string.h>
#include <vector>
#include <queue>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "huffmanTree.h"

using namespace std;

void fireman(int) {
    while(waitpid(-1, NULL, WNOHANG) > 0) {
        break;
    }
}

int main(int argc, char *argv[]) {
    //make sure port number is provided
    if(argc < 2) {
        cout << "no port number provided, terminating program" << endl;
        return 1;
    }

    //create huffman tree
    int numProcesses = 0;
    int age = 0;
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, huffmanComparator> huffmanQueue;

    string line;
    while(getline(cin, line)) {
        char ch = line[0];
        int freq = stoi(line.substr(2, line.size()-1));

        HuffmanNode* temp = createTreeNode(ch, freq, nullptr, nullptr);
        age += 1;
        temp->age = age;
        huffmanQueue.push(temp);
        numProcesses += 1;
    }

    HuffmanNode* root = createHuffmanTree(huffmanQueue, age);
    printHuffmanTree(root, "");

    //sockets programming
    int sockfd, newsockfd, n;
    int portno;
    char buffer[256];
    char charToSend;

    signal(SIGCHLD, fireman);

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {
        cout << "could not open socket" << endl;
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = stoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "binding failed" << endl;
        exit(1);
    }

    listen(sockfd, numProcesses); 
    clilen = sizeof(cli_addr);

    int pid;
    while(true) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0) {
            cout << "error on accept" << endl;
            exit(1);
        }

        //make child process
        pid = fork();
        if(pid == 0) {
            //get info from client
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 256);
            if(n < 0) {cout << "error on read" << endl; exit(1);}
            string huffCode(buffer);
            charToSend = getCharacterFromHuffmanCode(root, huffCode, -1);

            //send info to client
            bzero(buffer, 256); //clear the buffer
            string temp(1, charToSend);
            strcpy(buffer, temp.c_str());
            n = write(newsockfd, buffer, strlen(buffer));
            if(n < 0){cout << "error on write" << endl; exit(1);}

            close(newsockfd);
            _exit(0);
        }
    }

    // for(int i=0; i<numProcesses; i++) {
    //     wait(nullptr);
    // }

    close(sockfd);
}