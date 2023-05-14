#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]) {
    int pid;
    cout << "I am the parent process" << endl;

    for(int i=0; i<4; i++) {
        pid = fork();
        if(pid == 0) {
            cout << "I am the child process " << i << endl;
            for(int j=0; j<i*2; j++) {
                pid = fork();
                if(pid == 0) {
                    cout << "I am a grandchild process from the child process " << i << endl;
                    _exit(0);
                }
                wait(nullptr);
            }
            _exit(0);
        }
        wait(nullptr);
    }
}
