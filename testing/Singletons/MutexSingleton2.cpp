#include <iostream>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>

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
    string someOtherData;
    vector<string>* mainThreadAcc;
};

void* threadInOrder(void* args) {
    MutexHolder::Get().mutexUnlock();
    ThreadArgs* funcArg = (ThreadArgs*) args;
    MutexHolder& mutexVars = MutexHolder::Get();

    string localData = funcArg->someOtherData;
    int localTurn = funcArg->threadTurn;
    int shared_resource = mutexVars.getSharedResource();
    int* sharedResourceAddr = &shared_resource;
    vector<string>* toMain = funcArg->mainThreadAcc;

    mutexVars.mutexLock();
    while(*(sharedResourceAddr) != localTurn) {
        mutexVars.conditionWait();
    }
    mutexVars.mutexUnlock();

    localData = "this is local data from Thread " + to_string(localTurn);

    mutexVars.mutexLock();
    cout << "I am Thread " << *(sharedResourceAddr) << endl;
    *(sharedResourceAddr) += 1;
    toMain->push_back(localData);
    mutexVars.conditionBroadcast();
    mutexVars.mutexUnlock();

    return NULL;
}

int main(int argc, char *argv[]) {
    int nThreads;
    cout << "enter number of threads:" << endl;
    cin >> nThreads;

    MutexHolder::Get().mutexInit();
    vector<pthread_t> threads;
    struct ThreadArgs argument;
    vector<string> returns;
    argument.mainThreadAcc = &returns;

    for(int i=0; i<nThreads; i++) {
        MutexHolder::Get().mutexLock();
        argument.threadTurn = i;
        //cout << "the turn being assignment to the thread is: " << argument->threadTurn << endl;

        pthread_t thread;
        if(pthread_create(&thread, NULL, threadInOrder, &argument)) {
            cout << "failed to make thread" << endl;
            return 1;
        }

        threads.push_back(thread);
    }

    for(int i=0; i<threads.size(); i++) {
        pthread_join(threads[i], NULL);
    }

    MutexHolder::Get().mutexDestroy();
}