#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main()
{
    // Parent process
    int pid;
    cout << "I am the parent process" << "\n";

    // Parent loop
    for (int i = 0; i < 4; i++)
    {
        pid = fork();
        // Child check
        if (pid == 0)
        {
            cout << "I am the child process " << i << endl;

            // Child loop
            if (i == 1 || i == 2)
            {
                pid = fork();
                if (pid == 0)
                {
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