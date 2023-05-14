#include <pthread.h>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

#define NOPER 4

struct operation
{
	int val1;
	int val2;
    int op;
    double result;
};

void *calculator(void *pos_void_ptr)
{
	operation *pos_ptr = (operation *)pos_void_ptr; // cast pos_void_ptr to a struct operation *
    switch(pos_ptr->op)
    {
        case 0: pos_ptr->result = pos_ptr->val1 + pos_ptr->val2; 
                break;
        case 1: pos_ptr->result = pos_ptr->val1 - pos_ptr->val2; 
                break;
        case 2: pos_ptr->result = pos_ptr->val1 * pos_ptr->val2; 
                break;
        case 3: if (pos_ptr->val2 != 0)
                    pos_ptr->result = (double) pos_ptr->val1 / pos_ptr->val2; 
                else
                    pos_ptr->result = 0;         
                break;
    }
	return NULL;
}

int main()
{
	//static struct operation operations[NOPER];
 	//pthread_t tid[NOPER];
 	static vector<operation*> operations;
 	vector<pthread_t> threads(NOPER);
 	
        
	for(int i=0;i<NOPER;i++)
	{
		operation* ops = new operation();
		ops->op = i;
        std::cin >> ops->val1;
        std::cin >> ops->val2;
        
        pthread_t thread;
        
		if(pthread_create(&thread, NULL, calculator, (void *)ops)) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		
		operations.push_back(ops);
		threads.push_back(thread);
	}
	
	// Wait for the other threads to finish.
    // Call pthread_join here
    for(int i=0; i<NOPER; i++) {
        pthread_join(threads[i], NULL);
    }

    cout << operations[0]->op << endl;
    
    for (int i = 0; i < NOPER; i++)
    {
        switch(operations[i]->op)
        {
            case 0: std::cout << operations[i]->val1 << " + " << operations[i]->val2 << " = " << std::fixed << std::setprecision(2) << operations[i]->result << std::endl;
                    break;
            case 1: std::cout << operations[i]->val1 << " - " << operations[i]->val2 << " = " << std::fixed << std::setprecision(2) << operations[i]->result << std::endl;
                    break;
            case 2: std::cout << operations[i]->val1 << " * " << operations[i]->val2 << " = " << std::fixed << std::setprecision(2) << operations[i]->result << std::endl;
                    break;
            case 3: std::cout << operations[i]->val1 << " / " << operations[i]->val2 << " = " << std::fixed << std::setprecision(2) << operations[i]->result << std::endl;
                    break;
        }
    }
    return 0;
}