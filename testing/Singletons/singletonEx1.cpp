#include <iostream>

using namespace std;

class Singleton {
    private:
        Singleton() {}
        int variable = 0;

    public:
        Singleton(const Singleton&) = delete; // deletes the copy constructor

        static Singleton& Get() {
            static Singleton instance;
            return instance;
        }

        void testFunction() {
            cout << "test" << endl;
        }

        void setVar(int val) {
            variable = val;
        }

        int getVar() {
            return variable;
        }
};

int main(int argc, char *argv[]) {
    Singleton& inst = Singleton::Get();
    inst.testFunction();

    cout << "int variable was initially: " << inst.getVar() << endl;
    inst.setVar(5);
    cout << "int variable is now: " << inst.getVar() << endl;

    Singleton& newInst = Singleton::Get();
    newInst.setVar(3);
    cout << "the in variable is now: " << inst.getVar() << endl;
}