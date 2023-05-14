#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>

using namespace std;

struct threadArgs {
    int threadTurn;

    pthread_mutex_t* mutexSem;
    pthread_cond_t* condVar;
    int* sharedResource;

    threadArgs(int _threadTurn) {
        threadTurn = _threadTurn;
    }
};

void* print_in_correct_order(void* void_ptr_arg) {
    threadArgs* arg = (threadArgs*) void_ptr_arg;
    int currTurn = arg->threadTurn;

    pthread_mutex_t* bsem = arg->mutexSem;
    pthread_cond_t* waitTurn = arg->condVar;
    int turn = *(arg->sharedResource);

    //lock & unlock for checking shit
    pthread_mutex_lock(bsem);
    while(turn != currTurn) {
        cout << "waiting for turn: " << currTurn << " the turn right now is " << turn << endl;
        pthread_cond_wait(waitTurn, bsem);
    }
    pthread_mutex_unlock(bsem);

    cout << "I am Thread " << currTurn << endl;

    pthread_mutex_lock(bsem);
    turn = turn + 1;
    cout << "about to broadcast " << turn << endl;
    pthread_cond_broadcast(waitTurn);
    pthread_mutex_unlock(bsem);
    cout << "unlocked" << endl;

    return NULL;
}

int main(int argc, char *argv[]) {
    cout << "enter number of threads: " << endl;
    int nthreads;
    cin >> nthreads;

    vector<threadArgs*> threadReturnVals(nthreads);
    vector<pthread_t> tid;

    static pthread_mutex_t bSem;
    pthread_mutex_init(&bSem, nullptr);
    static pthread_cond_t wait_turn = PTHREAD_COND_INITIALIZER;
    int turn = 0;

    for(int i=0; i<nthreads; i++) {
        threadReturnVals[i] = new threadArgs(i);
        threadReturnVals[i]->sharedResource = &turn;
        threadReturnVals[i]->mutexSem = &bSem;
        threadReturnVals[i]->condVar = &wait_turn;
        pthread_t thread;
        if(pthread_create(&thread, NULL, print_in_correct_order, (void*)threadReturnVals[i])) {
            cout << "failed to make thread" << endl;
            return 1;
        }
        pthread_mutex_lock(threadReturnVals[i]->mutexSem);
        *(threadReturnVals[i]->sharedResource)++;
        pthread_cond_broadcast(&wait_turn);
        pthread_mutex_unlock(threadReturnVals[i]->mutexSem);
        tid.push_back(thread);
    }

    for(int i=0; i<nthreads; i++)
        pthread_join(tid[i], NULL);
    return 0;
}