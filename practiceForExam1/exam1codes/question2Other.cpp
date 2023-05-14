#include <iostream>
#include <pthread.h>
#include <vector>
#include <vector>
#include <stack>

using namespace std;

struct threadData {
    char octalDigit;
    string* binaryDigit;
};

void* octToBin(void* args) {
    threadData* numbers = (threadData *)args;

    int decimal = numbers->octalDigit - '0';
    stack<char> binary;

    while(decimal != 0) {
        binary.push((char(decimal % 2) + '0'));
        decimal /= 2;
    }

    while(binary.size() < 3)
        binary.push('0');

    while(binary.size() != 0) {
        *(numbers->binaryDigit) += binary.top();
        binary.pop();
    }    

    return NULL;
}

int main(int argc, char *argv[]) {
    string input = "205";

    vector<pthread_t> threads;
    vector<threadData*> threadArgs;

    for(char c: input) {
        threadData* arg = new threadData();
        arg->octalDigit = c;
        string b = "";
        arg->binaryDigit = &b;

        pthread_t thread;
        pthread_create(&thread, NULL, octToBin, (void *)arg);

        threads.push_back(thread);
        threadArgs.push_back(arg);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    for(threadData* threadReturnVal: threadArgs) {
        string* b = threadReturnVal->binaryDigit;
        cout << threadReturnVal->octalDigit << " (base 8) = " << &b << endl;
        delete threadReturnVal;
    }
    
}