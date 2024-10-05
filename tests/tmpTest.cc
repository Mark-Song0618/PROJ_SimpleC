#include <iostream>
class Base {
    public:
        virtual void p() { std::cout << "b" << std::endl; }
};

class Derive : public Base {
    public:
        virtual void p() { std::cout << "d" << std::endl; }
};

int main() {
    Base* b;
    Derive* d = new Derive;
    b = d;
    b->p();
    return 0; 
}
