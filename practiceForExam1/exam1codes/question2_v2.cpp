#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <pthread.h>
#include <cmath>
#include <utility>

using namespace std;

struct threadData {
    int pos;
    int arrSize;
    vector<int> array;
    pair<int, int> swapElms;

    threadData(int p, int s, vector<int> arr) {
        pos = p;
        arrSize = s;
        array = arr;
    }
};

void* swaps(void* args) {
    threadData* data = (threadData *)args;

    int posSwap1 = data->pos;
    int posSwap2 = data->arrSize - 1 - posSwap1;

    data->swapElms = make_pair(posSwap1, posSwap2);

    return NULL;
}

int main(int argc, char *argv[]) {
    int s = 5;
    string inArray = "1 2 3 4 5";
    
    vector<int> array;
    stringstream ss(inArray);
    string elm;
    while(ss >> elm) {
        array.push_back(stoi(elm));
    }

    vector<pthread_t> threads;
    vector<threadData*> threadArgs;

    int numThreads = floor(s/2);
    for(int i=0; i<numThreads; i++) {
        threadData* threadFuncArg = new threadData(i, s, array);

        pthread_t thread;
        pthread_create(&thread, NULL, swaps, (void *)threadFuncArg);

        threads.push_back(thread);
        threadArgs.push_back(threadFuncArg);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    for(threadData* returnVal: threadArgs) {
        int firstSwapVal = array[returnVal->swapElms.first];
        int secondSwapVal = array[returnVal->swapElms.second];

        array[returnVal->swapElms.first] = secondSwapVal;
        array[returnVal->swapElms.second] = firstSwapVal;

        delete returnVal;
    }

    for(int i: array)
        cout << i << ' ';
    cout << endl;
}