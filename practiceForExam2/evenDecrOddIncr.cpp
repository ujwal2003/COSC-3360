/*
* I am Thread 4
* I am Thread 2
* I am Thread 0
* I am Thread 1
* I am Thread 3
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

void* evenDecrOddIncr(void* void_ptr_arg) {
    int currTurn = *(int*) void_ptr_arg;

    //TODO lock, check if correct turn, print,  unlock
    pthread_mutex_lock(&bsem);
    while(turn != currTurn) {
        pthread_cond_wait(&waitTurn, &bsem);
    }
    cout << "I am Thread " << turn << endl;
    pthread_mutex_unlock(&bsem);

    //TODO lock, if even (decrement 2), if 0 (goto 1), if odd (increment 2), broadcast, unlock
    pthread_mutex_lock(&bsem);

    //even and not 0
    if(turn % 2 == 0 && turn != 0) {
        turn = turn - 2;
    }

    //zero
    else if(turn == 0) {
        turn = 1;
    }

    //odd
    else if(turn%2 != 0) {
        turn += 2;
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

    if((nThreads-1)%2 == 0) {
        turn = nThreads - 1;
        turn = (turn < 0) ? 0 : turn;
    } else if((nThreads-1)%2 != 0) {
        turn = nThreads - 2;
        turn = (turn < 0) ? 0 : turn;
    }

    for(int i=0; i<nThreads; i++) {
        threadNumber[i] = i;
        if(pthread_create(&tid[i], NULL, evenDecrOddIncr, &threadNumber[i])) {
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