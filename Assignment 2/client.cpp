#include <iostream>
#include <string.h>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <map>
#include <vector>

using namespace std;

//extracts all numbers from a string and puts them into a vector
vector<string> getNumbersFromString(string line) {
    vector<string> v;
    stringstream sstream;
    sstream << line;

    string temp;
    string foundInt;
    while(!sstream.eof()) {
        sstream >> temp;
        if(stringstream(temp) >> foundInt)
            v.push_back(foundInt);
        temp = "";
    }
    return v;
}

struct ThreadArgs {
    string line;
    char* hostName;
    int portnum;
    map<int, char> decodedMap;
};

void* decompress(void* threadArguments) {
    ThreadArgs* threadArg = (ThreadArgs *)threadArguments;
    vector<string> lineNumbers = getNumbersFromString(threadArg->line);

    //create socket
    int sockfd, n, portno;
    char buffer[256];

    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = threadArg->portnum;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0) {cout << "error opening socket" << endl; exit(1);}

    server = gethostbyname(threadArg->hostName);
    if(server == NULL) {cout << "error, no such host" << endl; exit(1);}

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    //try connecting to the server
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {cout << "connection failed" << endl; exit(1);}

    //send message to server
    bzero(buffer, 256);
    strcpy(buffer, lineNumbers[0].c_str()); //put huffman code in buffer
    n = write(sockfd, buffer, strlen(buffer));
    if(n < 0){cout << "error on write" << endl; exit(1);}

    //read message from server
    bzero(buffer, 256);
    n = read(sockfd, buffer, 256);
    if(n < 0) {cout << "error on read" << endl; exit(1);}
    for(int i=1; i<lineNumbers.size(); i++) {
        threadArg->decodedMap[stoi(lineNumbers[i])] = buffer[0];
    }

    close(sockfd);

    return(NULL);
}

int main(int argc, char *argv[]) {
    //check if enough command line arguments are passed
    if(argc < 3) {
        cout << "not enough arguments provided. Expected: hostname, port number" << endl;
        return 1;
    }

    //setup code for pthreads
    vector<pthread_t> threads;
    static vector<ThreadArgs*> threadReturnVals;

    //read compressed file and create threads
    string line;
    while(getline(cin, line)) {
        ThreadArgs* funcArgs = new ThreadArgs();
        funcArgs->line = line;
        funcArgs->hostName = argv[1];
        funcArgs->portnum = stoi(argv[2]);

        pthread_t thread;
        if(pthread_create(&thread, NULL, decompress, (void *)funcArgs)) {
            cout << "could not make thread" << endl;
            return -1;
        }

        threads.push_back(thread);
        threadReturnVals.push_back(funcArgs);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    map<int, char> finalMessage;
    for(ThreadArgs* vals: threadReturnVals) {
        for(auto elm: vals->decodedMap) {
            finalMessage[elm.first] = elm.second;
        }
        delete vals;
    }

    //output message
    cout << "Original message: ";
    for(const auto& elm: finalMessage)
        cout << elm.second;
    cout << endl;
}