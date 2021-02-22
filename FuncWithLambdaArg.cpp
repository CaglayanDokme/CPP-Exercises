// Description: Dummy code for functions that accept lambda expressions as input parameter
// Date:        February 20, 2021
// Author:      Caglayan DOKME

#include <iostream>
#include <vector>
#include <iterator>

using namespace std;

template<typename LambdaType>
void Function(vector<int>& container, LambdaType lambda)
{
    /*  Iterate over the given integer vector and call the lambda expression using the pointed data. */
    for(vector<int>::iterator itr = container.begin(); itr != container.end(); itr++)
        if(lambda(*itr) == true)
            cout << *itr << endl;
}

int main(int argc, char const *argv[]) {
    vector<int> v1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    Function(v1, [](int value) { return (value > 5); });

    return 0;
}
