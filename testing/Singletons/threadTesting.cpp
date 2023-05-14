#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include <map>

using namespace std;

//singleton class to hold the mutex, condition variable, and shared resource
class MutexHolder {
    private:
        MutexHolder() {}
        pthread_mutex_t bsem;
        pthread_cond_t conditionVar = PTHREAD_COND_INITIALIZER;
        int sharedResource = 0;

    public:
        //delete the copy constructor, so we can only use a reference of the insatnce
        MutexHolder(const MutexHolder&) = delete;

        static MutexHolder& Get() {
            static MutexHolder instance;
            return instance;
        }

        void mutexInit() {pthread_mutex_init(&bsem, NULL);}
        void mutexDestroy() {pthread_mutex_destroy(&bsem);}
        int getSharedResource() {return sharedResource;} //! only to be used in critical section
        void setSharedResource(int value) {sharedResource = value;} //! only to be used in critical section
        void mutexLock() {pthread_mutex_lock(&bsem);}
        void mutexUnlock() {pthread_mutex_unlock(&bsem);}
        void conditionWait() {pthread_cond_wait(&conditionVar, &bsem);}
        void conditionBroadcast() {pthread_cond_broadcast(&conditionVar);}
};

struct ThreadArgs {
    int threadTurn;
    char arg2;
    vector<char>* mainAcc;
    map<int, char>* mainMapAcc;
};

void* threadFunction(void* args) {
    MutexHolder& mutexVars = MutexHolder::Get();

    ThreadArgs* funcArg = (ThreadArgs*) args;
    int localTurn = funcArg->threadTurn;
    char localArg2 = funcArg->arg2;
    mutexVars.mutexUnlock();

    mutexVars.mutexLock();
    while(mutexVars.getSharedResource() != localTurn) {
        mutexVars.conditionWait();
    }
    mutexVars.mutexUnlock();

    cout << "I am Thread " << mutexVars.getSharedResource() << endl;

    mutexVars.mutexLock();
    funcArg->mainAcc->push_back(localArg2);
    mutexVars.setSharedResource(mutexVars.getSharedResource()+1);
    mutexVars.conditionBroadcast();
    mutexVars.mutexUnlock();

    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "no argument for number of threads provided, terminating." << endl;
        return 0;
    }

    int nThreads = stoi(argv[1]);

    MutexHolder& mutexHolder = MutexHolder::Get();
    mutexHolder.mutexInit();

    ThreadArgs argument;
    vector<pthread_t> threads;
    vector<char> returnVals;

    for(int i=0; i<nThreads; i++) {
        mutexHolder.mutexLock();
        argument.threadTurn = i;
        argument.arg2 = '0' + i;
        argument.mainAcc = &returnVals;
        
        pthread_t thread;
        if(pthread_create(&thread, NULL, threadFunction, &argument)) {
            cout << "failed to create thread" << endl;
            return 1;
        }
        threads.push_back(thread);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }
    mutexHolder.mutexDestroy();

    for(char c: returnVals)
        cout << c << ' ';
    cout << endl;
}