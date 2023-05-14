#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[]) {
    int pid;
    cout << "I am the parent process" << endl;

    for(int i=0; i<3; i++) {
        pid = fork();
        if(pid == 0) {
            cout << "I am child process " << i << endl;
            if(i == 0 || i == 1) {
                pid = fork();
                if(pid == 0) {
                    cout << "I am grandchild process " << i << endl;
                    _exit(0);
                }
                wait(nullptr);
            }
            _exit(0);
        }
        wait(nullptr);
    }
}