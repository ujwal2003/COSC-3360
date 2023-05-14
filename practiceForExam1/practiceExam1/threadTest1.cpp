#include <iostream>
#include <pthread.h>
#include <vector>
#include <string>

using namespace std;

#define NUM_THREADS 5

struct thread_data {
   int  thread_id;
   string message;
};

void *PrintHello(void *threadarg) {
   thread_data *my_data = (thread_data *) threadarg;
   string temp = my_data->message + " from thread with id: " + to_string(my_data->thread_id);
   my_data->message = temp;

   return NULL;
}

int main () {
   vector<pthread_t> threads;
   static vector<thread_data*> threadArgs;

   for(int i = 0; i < NUM_THREADS; i++ ) {
      thread_data* td = new thread_data();
      td->thread_id = i;
      td->message = "This is a message";

      pthread_t thread;

      int rc = pthread_create(&thread, NULL, PrintHello, (void *)td);
      
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }

      threads.push_back(thread);
      threadArgs.push_back(td);
   }
   
   for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
   }

   for(thread_data* messages: threadArgs) {
        cout << messages->message << endl;
   }
}