// Description: Example usage of mutable keyword
// Date:        May 12, 2021
// Author:      Caglayan DOKME

#include <iostream>

class Base{
public:
    // Constructor
    Base(int _a) : a(_a), a_refCounter(0) 
    { 
        std::cout << "Constructed with " << a << std::endl; 
    }

    // Destructor
    ~Base() { std::cout << "Destructed" << std::endl; }

    // Getter functions
    int getA() const 
    { 
        ++a_refCounter; // Wouldn't compile without mutable keyword 

        return a; 
    }

    int getReferenceCount() const
    {
        return a_refCounter;
    }

    // Setter function
    void setA(int _a) 
    { 
        a = _a; 
    }

private:
    int a;
    mutable int a_refCounter;   // Reference counter of member a
};

int main()
{
    std::cout << "Program started" << std::endl;

    Base b0(3);

    std::cout << "b0: " << b0.getA() << std::endl;
    std::cout << "b0: " << b0.getA() << std::endl;
    std::cout << "Reference Counter: " << b0.getReferenceCount() << std::endl;    

    std::cout << "Program ended" << std::endl;
    return 0;
}
