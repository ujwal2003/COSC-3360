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

struct ThreadArgs {
    HuffmanNode* root;
    string parseLine;
    int currTurn;
    vector<char>* originalMsg;

    pthread_mutex_t* sem1;
    pthread_mutex_t* sem2;
    pthread_cond_t* waitTurn;
    int* turn_sh;
};

void* decompress(void* threadArguments) {
    ThreadArgs* currArgs = (ThreadArgs *) threadArguments;
    int localTurn = currArgs->currTurn;
    string lineToParse = currArgs->parseLine;
    HuffmanNode* treeRoot = currArgs->root;
    pthread_mutex_unlock(currArgs->sem1); //unlock after local variables have been copied

    vector<string> numbers = getNumbersFromString(lineToParse);
    pair<char, int> decodedCharAndFreq;
    decodedCharAndFreq = getCharacterFromHuffmanCode(treeRoot, numbers[0], -1);

    //wait to make sure its the correct thread
    pthread_mutex_lock(currArgs->sem2);
    while(*currArgs->turn_sh != localTurn) {
        pthread_cond_wait(currArgs->waitTurn, currArgs->sem2);
    }
    pthread_mutex_unlock(currArgs->sem2);

    //print node info
    cout << "Symbol: " << decodedCharAndFreq.first << ", Frequency: " << decodedCharAndFreq.second << ", Code: " << numbers[0] << endl;

    //add to final message vector
    pthread_mutex_lock(currArgs->sem1);
    for(int i=1; i<numbers.size(); i++) {
        currArgs->originalMsg->at(stoi(numbers[i])) = decodedCharAndFreq.first;
    }
    pthread_mutex_unlock(currArgs->sem1);

    //updating turn
    pthread_mutex_lock(currArgs->sem2);
    *currArgs->turn_sh += 1;
    pthread_cond_broadcast(currArgs->waitTurn);
    pthread_mutex_unlock(currArgs->sem2);

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

    pthread_mutex_t semArgs;
    pthread_mutex_init(&semArgs, NULL);
    pthread_mutex_t semForWaiting;
    pthread_mutex_init(&semForWaiting, NULL);

    pthread_cond_t waitForTurn = PTHREAD_COND_INITIALIZER;
    int turn = 0;

    vector<pthread_t> threads;
    ThreadArgs argument;
    vector<char> finalMsg(huffmanTreeRoot->frequency);

    //compressed file info
    for(int i=0; i<numSymbols; i++) {
        getline(cin, line);

        pthread_mutex_lock(&semArgs);
        argument.currTurn = i;
        argument.root = huffmanTreeRoot;
        argument.parseLine = line;
        argument.originalMsg = &finalMsg;
        argument.sem1 = &semArgs;
        argument.sem2 = &semForWaiting;
        argument.waitTurn = &waitForTurn;
        argument.turn_sh = &turn;

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

    pthread_mutex_destroy(&semArgs);
    pthread_mutex_destroy(&semForWaiting);
    delete huffmanTreeRoot;

    //print out original message
    cout << "Original message: ";
    for(char c: finalMsg)
        cout << c;
    cout << endl;
}