#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;

static pthread_mutex_t bsem;
static pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
static int turn;

void* print_in_reverse_order(void* void_ptr_arg) {
    int currTurn = *(int*) void_ptr_arg;

    //lock & unlock for checking shit
    pthread_mutex_lock(&bsem);
    while(turn > currTurn) {
        pthread_cond_wait(&waitTurn, &bsem);
    }
    pthread_mutex_unlock(&bsem);

    cout << "I am Thread " << currTurn << endl;

    //if I'm at the correct turn, lock, print out, broadcast, unlock
    pthread_mutex_lock(&bsem);
    turn = turn - 1;
    pthread_cond_broadcast(&waitTurn);
    pthread_mutex_unlock(&bsem);

    return NULL;
}

int main(int argc, char *argv[]) {
    int nthreads;
    cin >> nthreads;
    pthread_mutex_init(&bsem, NULL); //Initialize access to 1
    pthread_t *tid = new pthread_t[nthreads];
    int* threadNumber = new int[nthreads]; //the arguments

    turn = nthreads - 1;

    for(int i=0; i<nthreads; i++) {
        threadNumber[i] = i;
        if(pthread_create(&tid[i], NULL, print_in_reverse_order, &threadNumber[i])) {
            cout << "failes to make thread" << endl;
            return 1;
        }
    }

    for(int i=0; i<nthreads; i++)
        pthread_join(tid[i], NULL);
    delete [] threadNumber;
    delete [] tid;
    return 0;
}