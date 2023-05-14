#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main() {
	int pid;
	cout << "I am the parent process" << endl;
	for(int i=0; i<3; i++) {
		pid = fork();
		if(pid == 0) {
			cout << "I am the child process " << i << endl;
			if(i == 1) {
				pid = fork();
				if(pid == 0) {
					cout << "I am grandchild process " << i << endl;
					_exit(0);
				}
				wait(nullptr);
			}
			_exit(0); //ends the child process
		}
		wait(nullptr); // <-- guarantees it will happen in certain order
	}
	
	//always use wait() outside the loop where you call fork()
	//only use wait inside the loop to guarantee a certain order
	//we will use it inside for assignments, exams & extra credit
	/*
	for(int i=0; i<3; i++)
		wait(nullptr); //waits for all child process to complete
	*/
}