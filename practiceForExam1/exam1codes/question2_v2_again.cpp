#include <iostream>
#include <cmath>
#include <vector>
#include <pthread.h>

using namespace std;

struct threadData {
    int initPos;
    int size;
    int* arr;

    threadData(int p, int s, int* a) {
        initPos = p;
        size = s;
        arr = a;
    }
};

void* swaps(void* args) {
    threadData* data = (threadData *)args;
    int pos2 = data->size - 1 - data->initPos;

    int elmSwap1 = data->arr[data->initPos];
    int elmSwap2 = data->arr[pos2];

    data->arr[data->initPos] = elmSwap2;
    data->arr[pos2] = elmSwap1;

    return NULL;
}

int main(int argc, char *argv[]) {
    int s;
    cin >> s;

    int* arr = new int[s];
    for(int i=0; i<s; i++) {
        cin >> arr[i];
    }

    vector<pthread_t> threads;
    vector<threadData*> threadArgs;

    int numThreads = floor((double)s/2.0);
    for(int i=0; i<numThreads; i++) {
        threadData* arg = new threadData(i, s, arr);

        pthread_t thread;
        pthread_create(&thread, NULL, swaps, (void *) arg);

        threads.push_back(thread);
        threadArgs.push_back(arg);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    for(threadData* val: threadArgs) {
        delete val;
    }

    for(int i=0; i<s; i++) {
        cout << arr[i] << ' ';
    }
    cout << endl;
}