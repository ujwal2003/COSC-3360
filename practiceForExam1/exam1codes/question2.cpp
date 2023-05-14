#include <iostream>
#include <string>
#include <pthread.h>
#include <vector>

using namespace std;

struct threadData {
    char charToSearch;
    string msg;
    int count = 0;

    threadData(char inChar, string inMsg) {
        charToSearch = inChar;
        msg = inMsg;
    }
};

void* countFreq(void* args) {
    threadData* data = (threadData *)args;
    for(char c: data->msg) {
        if(c == data->charToSearch) {
            data->count += 1;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    string charToSearch = "C 3";
    string message = "COSC 3360 FALL 2022 - RINCON";

    vector<pthread_t> threads;
    static vector<threadData*> threadArgs;

    for(char c: charToSearch) {
        threadData* args = new threadData(c, message);
        pthread_t thread;

        if(pthread_create(&thread, NULL, countFreq, (void *)args)) {
            cout << "failed to create thread" << endl;
        }

        threads.push_back(thread);
        threadArgs.push_back(args);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    for(threadData* data: threadArgs) {
        cout << data->charToSearch << ": " << data->count << endl;
        delete data;
    }
}