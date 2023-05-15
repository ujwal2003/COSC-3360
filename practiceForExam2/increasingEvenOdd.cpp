/*
* I am Thread 0 
* I am Thread 2 
* I am Thread 4 
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
static int turn = 0;
static int nThreads;

void* printIncreasingEvenOdd(void* void_ptr_arg) {
    int currTurn = *(int*) void_ptr_arg;

    //lock check if I'm at the correct turn (loop: wait)
    //increment turn by 2, then unlock
    pthread_mutex_lock(&bsem);
    while(turn != currTurn) {
        pthread_cond_wait(&waitTurn, &bsem);
    }
    pthread_mutex_unlock(&bsem);

    cout << "I am Thread " << currTurn << endl;
    
    //check if turn is even (and if its the last even), if at last even, do the odds
    pthread_mutex_lock(&bsem);
    if(turn % 2 == 0 && turn + 2 >= nThreads) {
        //the last even
        turn = 1;
    } else if(turn % 2 == 0) {
        //not at last even
        turn += 2;
    } else if(!(turn + 2 >= nThreads)){
        //odds
        turn += 2;
    }

    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);

    return NULL;
}

int main(int argc, char *argv[]) {
    cin >> nThreads;
    pthread_mutex_init(&bsem, NULL);
    pthread_t *tid = new pthread_t[nThreads];
    int* threadNumber = new int[nThreads]; //the arguments

    for(int i=0; i<nThreads; i++) {
        threadNumber[i] = i;
        if(pthread_create(&tid[i], NULL, printIncreasingEvenOdd, &threadNumber[i])) {
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