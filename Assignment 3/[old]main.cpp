#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <sstream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#include "huffmanTree.h"

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

//singleton class to hold the mutex, condition variable, and shared resource
class MutexHolder {
    private:
        MutexHolder() {}
        pthread_mutex_t bsem;
        pthread_cond_t conditionVar = PTHREAD_COND_INITIALIZER;
        int sharedResource = 0;

    public:
        //delete the copy constructor, so we can only use a reference of the insatnce
        MutexHolder(const MutexHolder&) = delete;

        static MutexHolder& Get() {
            static MutexHolder instance;
            return instance;
        }

        void mutexInit() {pthread_mutex_init(&bsem, NULL);}
        void mutexDestroy() {pthread_mutex_destroy(&bsem);}
        int getSharedResource() {return sharedResource;} //! only to be used in critical section
        void setSharedResource(int value) {sharedResource = value;} //! only to be used in critical section
        void mutexLock() {pthread_mutex_lock(&bsem);}
        void mutexUnlock() {pthread_mutex_unlock(&bsem);}
        void conditionWait() {pthread_cond_wait(&conditionVar, &bsem);}
        void conditionBroadcast() {pthread_cond_broadcast(&conditionVar);}
};

struct ThreadArgs {
    HuffmanNode* root;
    string parseLine;
    int currTurn;
    vector<char>* originalMsg;
};

void* decompress(void* threadArguments) {
    MutexHolder& mutexVars = MutexHolder::Get();

    //unlock at first line, save locally, decoding
    ThreadArgs* currArgs = (ThreadArgs *) threadArguments;
    int localTurn = currArgs->currTurn;
    string lineToParse = currArgs->parseLine;
    HuffmanNode* treeRoot = currArgs->root;
    mutexVars.mutexUnlock();

    vector<string> numbers = getNumbersFromString(lineToParse);
    pair<char, int> decodedCharAndFreq;
    decodedCharAndFreq = getCharacterFromHuffmanCode(treeRoot, numbers[0], -1);

    //waiting, printing, updating turn variable, and placing char in referenced vector
    mutexVars.mutexLock();
    while(mutexVars.getSharedResource() != localTurn) {
        mutexVars.conditionWait();
    }

    cout << "Symbol: " << decodedCharAndFreq.first << ", Frequency: " << decodedCharAndFreq.second << ", Code: " << numbers[0] << endl;
    for(int i=1; i<numbers.size(); i++) {
        currArgs->originalMsg->at(stoi(numbers[i])) = decodedCharAndFreq.first;
    }
    mutexVars.mutexUnlock();


    mutexVars.mutexLock();
    mutexVars.setSharedResource(mutexVars.getSharedResource()+1);
    mutexVars.conditionBroadcast();
    mutexVars.mutexUnlock();

    return NULL;
}

int main(int argc, char *argv[]) {
    string line;
    int numSymbols;

    getline(cin, line);
    numSymbols = stoi(line);

    priority_queue<HuffmanNode*, vector<HuffmanNode*>, huffmanComparator> huffmanQueue;
    int age = 0;

    //create huffman tree from first n lines
    for(int i=0; i<numSymbols; i++) {
        getline(cin, line);
        char ch = line[0];
        int freq = stoi(line.substr(2, line.size()-1));

        HuffmanNode* temp = createTreeNode(ch, freq, nullptr, nullptr);
        age += 1;
        temp->age = age;
        huffmanQueue.push(temp);
    }

    HuffmanNode* huffmanTreeRoot = createHuffmanTree(huffmanQueue, age);

    MutexHolder::Get().mutexInit();
    vector<pthread_t> threads;
    ThreadArgs argument;
    vector<char> finalMsg(huffmanTreeRoot->frequency);

    //compressed file information
    for(int i=0; i<numSymbols; i++) {
        //read compressed file stuff, lock, save in sinle thread arg
        getline(cin, line);

        MutexHolder::Get().mutexLock();
        argument.currTurn = i;
        argument.root = huffmanTreeRoot;
        argument.parseLine = line;
        argument.originalMsg = &finalMsg;

        pthread_t thread;
        if(pthread_create(&thread, NULL, decompress, &argument)) {
            cout << "failed to make thread" << endl;
            return 1;
        }
        threads.push_back(thread);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    MutexHolder::Get().mutexDestroy();
    delete huffmanTreeRoot;

    //print out original message
    cout << "Original message: ";
    for(char c: finalMsg)
        cout << c;
    cout << endl;
}