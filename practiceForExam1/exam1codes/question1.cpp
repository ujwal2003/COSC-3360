#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int numGrandchilderen (int process) {
    switch(process) {
        case 0: return 0;
        case 1: return 2;
        case 2: return 4;
        case 3: return 6;
        default: return 0;
    }
} 

int main(int argc, char *argv[]) {
    int pid;
    cout << "I am the parent process" << endl;

    for(int i=0; i<4; i++) {
        pid = fork();
        if(pid == 0) {
            cout << "I am the child process " << i << endl;

            for(int j=0; j<numGrandchilderen(i); j++) {
                pid = fork();
                if(pid == 0) {
                    cout << "I am a grandchild process from child process " << i << endl;
                    _exit(0);
                }
                wait(nullptr);
            }

            _exit(0);
        }
        wait(nullptr);
    }
}
