#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>

using namespace std;

class MutexSingleton {
    private:
        MutexSingleton() {}
        pthread_mutex_t bsem;
        pthread_cond_t waitTurn = PTHREAD_COND_INITIALIZER;
        int turn = 0;

    public:
        MutexSingleton(const MutexSingleton&) = delete; //deletes the copy constructor
        
        static MutexSingleton& Get() {
            static MutexSingleton instance;
            return instance;
        }

        //initializes the mutex
        void mutexInit() { pthread_mutex_init(&bsem, NULL); }

        //returns the value of the shared resource
        //! ensure this is used in the ciritcal section
        int getSharedResourceValue() { return turn; }

        //changes the value of the shared resource
        //! ensure this is used in the ciritcal section
        void setSharedResourceValue(int value) { turn = value; }

        //locks the mutex
        void mutexLock() { pthread_mutex_lock(&bsem); }

        //unlocks the mutex
        void mutexUnlock() { pthread_mutex_unlock(&bsem); }

        //waits for the condition variable
        void waitForCondition() {pthread_cond_wait(&waitTurn, &bsem); }

        //broadcasts the condition variable
        void broadcastCondition() {pthread_cond_broadcast(&waitTurn); }
};

struct ThreadArgs {
    MutexSingleton& mutexVars = MutexSingleton::Get();
    int threadTurn;
};

void* threadInOrder(void* voidPtrArgs) {
    ThreadArgs* args = (ThreadArgs*) voidPtrArgs;

    args->mutexVars.mutexLock();
    while(args->mutexVars.getSharedResourceValue() != args->threadTurn) {
        args->mutexVars.waitForCondition();
    }
    args->mutexVars.mutexUnlock();

    cout << "I am Thread " << args->mutexVars.getSharedResourceValue() << endl;

    args->mutexVars.mutexLock();
    args->mutexVars.setSharedResourceValue(args->mutexVars.getSharedResourceValue() + 1);
    args->mutexVars.broadcastCondition();
    args->mutexVars.mutexUnlock();

    return NULL;
}

int main(int argc, char *argv[]) {
    MutexSingleton& mutexHolder = MutexSingleton::Get();

    int nThreads;
    cout << "enter number of threads:" << endl;
    cin >> nThreads;

    mutexHolder.mutexInit();
    vector<pthread_t> threads;
    vector<ThreadArgs*> threadReturnVals;

    for(int i=0; i<nThreads; i++) {
        ThreadArgs* funcArgs = new ThreadArgs();
        funcArgs->threadTurn = i;

        pthread_t thread;
        if(pthread_create(&thread, NULL, threadInOrder, (void*)funcArgs)) {
            cout << "failed to make thread" << endl;
            return 1;
        }

        threadReturnVals.push_back(funcArgs);
        threads.push_back(thread);
    }

    for(int i=0; i<nThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    for(ThreadArgs* threadReturn: threadReturnVals) {
        delete threadReturn;
    }
}