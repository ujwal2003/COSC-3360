/*
* I am Thread 3
* I am Thread 1
* I am Thread 4
* I am Thread 2
* I am Thread 0
*/

#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

static pthread_mutex_t bsem;
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
static int turn;
static int nThreads;

void* printDecreasingOddEven(void* void_ptr_arg) {
    int currTurn = *(int*) void_ptr_arg;

    pthread_mutex_lock(&bsem);
    while(turn != currTurn) {
        pthread_cond_wait(&waitTurn, &bsem);
    }
    cout << "I am Thread " << turn << endl;
    pthread_mutex_unlock(&bsem);
    
    //lock, check if last odd (goto first even), if not last odd (decrement turn by 2), if turn even (decrement by 2), b & unlock
    pthread_mutex_lock(&bsem);
    
    //last odd
    if(turn == 1) {
        if(nThreads % 2 == 0)
            turn = nThreads - 2; //if nThreads is even, decrement 2 to goto first even
        else
            turn = nThreads - 1; //if nThreads is odd, decrement 1 to goto first even
    }

    //not last odd
    else if(turn%2 != 0) {
        turn = turn - 2;
    }

    //even
    else if(turn%2 == 0) {
        turn = turn - 2;
    }

    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);

    return NULL;
}

int main(int argc, char *argv[]) {
    cin >> nThreads;
    pthread_mutex_init(&bsem, NULL);
    pthread_t* tid = new pthread_t[nThreads];
    int* threadNumber = new int[nThreads];

    if((nThreads-1)%2 != 0) {
        turn = nThreads - 1;
        turn = (turn < 0) ? 0 : turn;
    } else {
        turn = nThreads - 2;
        turn = (turn < 0) ? 0 : turn;
    }

    for(int i=0; i<nThreads; i++) {
        threadNumber[i] = i;
        if(pthread_create(&tid[i], NULL, printDecreasingOddEven, &threadNumber[i])) {
            cout << "failed to create thread" << endl;
            return 1;
        }
    }

    for(int i=0; i<nThreads; i++)
        pthread_join(tid[i], NULL);

    delete [] threadNumber;
    delete [] tid;
    return 0;
}